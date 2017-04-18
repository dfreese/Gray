
#include <stdio.h>
#include <string.h>
#include <Random/Random.h>
#include <Gray/Config.h>
#include <Gray/LoadDetector.h>
#include <Gray/GammaMaterial.h>
#include <Graphics/SceneDescription.h>
#include <Graphics/TransformViewable.h>
#include <Graphics/VisiblePoint.h>
#include <Graphics/ViewableBase.h>
#include <Graphics/ViewableSphere.h>
#include <Graphics/ViewableEllipsoid.h>
#include <Graphics/ViewableTriangle.h>
#include <Graphics/ViewableCylinder.h>
#include <Output/DetectorArray.h>
#include <Output/Output.h>
#include <Sources/AnnulusCylinderSource.h>
#include <Sources/AnnulusEllipticCylinderSource.h>
#include <Sources/BeamPointSource.h>
#include <Sources/CylinderSource.h>
#include <Sources/EllipsoidSource.h>
#include <Sources/EllipticCylinderSource.h>
#include <Sources/RectSource.h>
#include <Sources/SphereSource.h>
#include <Sources/VectorSource.h>
#include <Sources/VoxelSource.h>
#include <Sources/SourceList.h>
#include <VrMath/LinearR3.h>
#include <stack>
#include <sstream>

void LoadDetector::ApplyTranslation(const VectorR3&t,
                                    RigidMapR3 & current_matrix)
{

    RigidMapR3 trans;
    trans.ApplyTranslationLeft(t);
    current_matrix *= trans;

}

void LoadDetector::ApplyRotation(const VectorR3& axis, double theta,
                                 RigidMapR3 & current_matrix)
{

    //FIXME: This became a mega hack to try and get rotations working correctly
    // When using Rotation Maps, translations were not being operated on
    // So I had to transition to 4x4 matricies, and hardcode it in

    RotationMapR3 rot;
    RigidMapR3 cur, cur2;
    LinearMapR4 t1,t2,t3;
    rot.Set(axis, -1.0*theta);

    t1.SetColumn1(rot.Column1().x, rot.Column1().y, rot.Column1().z, 0.0);
    t1.SetColumn2(rot.Column2().x, rot.Column2().y, rot.Column2().z, 0.0);
    t1.SetColumn3(rot.Column3().x, rot.Column3().y, rot.Column3().z, 0.0);
    t1.SetColumn4(0.0, 0.0, 0.0, 1.0);

    cur = current_matrix;
    t2.SetColumn1(cur.Column1().x, cur.Column1().y, cur.Column1().z, cur.m14);
    t2.SetColumn2(cur.Column2().x, cur.Column2().y, cur.Column2().z, cur.m24);
    t2.SetColumn3(cur.Column3().x, cur.Column3().y, cur.Column3().z, cur.m34);
    t2.SetColumn4(0.0 , 0.0, 0.0, 1.0);

    t2 *= t1;
    cur.m14 = t2.m41;
    cur.m24 = t2.m42;
    cur.m34 = t2.m43;

    cur2 = current_matrix;

    rot.Set(axis, theta);
    t1.SetColumn1(rot.Column1().x, rot.Column1().y, rot.Column1().z, 0.0);
    t1.SetColumn2(rot.Column2().x, rot.Column2().y, rot.Column2().z, 0.0);
    t1.SetColumn3(rot.Column3().x, rot.Column3().y, rot.Column3().z, 0.0);
    t1.SetColumn4(0.0, 0.0, 0.0, 1.0);

    t2.SetColumn1(cur2.Column1().x, cur2.Column1().y, cur2.Column1().z, cur2.m14);
    t2.SetColumn2(cur2.Column2().x, cur2.Column2().y, cur2.Column2().z, cur2.m24);
    t2.SetColumn3(cur2.Column3().x, cur2.Column3().y, cur2.Column3().z, cur2.m34);
    t2.SetColumn4(0.0 , 0.0, 0.0, 1.0);

    t2 *= t1;

    // FIXME: Mega hackage

    cur.SetColumn1(t2.Column1().x,t2.Column1().y, t2.Column1().z);
    cur.SetColumn2(t2.Column2().x,t2.Column2().y, t2.Column2().z);
    cur.SetColumn3(t2.Column3().x,t2.Column3().y, t2.Column3().z);

    current_matrix = cur;


}

bool LoadDetector::Load(const std::string & filename,
                        SceneDescription & theScene,
                        SourceList & sources, Config & config,
                        DetectorArray & detector_array)
{
    std::string filename_detector = "";
    string filename_basic_map;
    double polygonScale = 1.0;
    double actScale = 1.0;
    unsigned int block_id = 0;

    // Setup the matrix stack with one initial identity matrix.
    std::stack<RigidMapR3> MatrixStack;
    MatrixStack.push(RigidMapR3());
    MatrixStack.top().SetIdentity();


    stack<ifstream> file_stack;
    stack<size_t> file_lines_read_stack;
    stack<std::string> filename_stack;
    file_stack.emplace(filename.c_str());
    file_lines_read_stack.push(0);
    filename_stack.push(filename);

    if (!file_stack.top()) {
        cerr << "LoadDffFile: Unable to open file: " << filename << endl;
        return false;
    }

    auto print_parse_error = [&filename_stack, &file_lines_read_stack](
            const std::string & line)
    {
        cerr << "Parse error in NFF file \"" << filename_stack.top()
             << "\", line_no: " << file_lines_read_stack.top()
             << " line: " << line << endl;
    };

    std::string file_dir = "";
    size_t dir_pos = filename.find_last_of('/');
    if (dir_pos != std::string::npos) {
        // Include everything, including slash
        file_dir = filename.substr(0, dir_pos + 1);
    }

    int viewCmdStatus = false;		// True if currently handling a "v" command
    VectorR3 viewPos;
    VectorR3 lookAtPos;
    VectorR3 upVector;
    double fovy;		// Field of view angle (in radians)
    int screenWidth, screenHeight;
    double hither;

    // Vectorial Source parsing
    bool parse_VectorSource = false;
    VectorSource * curVectorSource = NULL;

    
    GammaMaterial* curMaterial = dynamic_cast<GammaMaterial*>(&theScene.GetMaterial(0));

    struct RepeatInfo {
        int no_repeats;
        int no_complete;
        size_t start_idx;
        string starting_file;
        size_t starting_line;
    };
    stack<RepeatInfo> repeat_info_stack;
    vector<string> repeat_buffer;
    size_t current_idx = 0;

    while (!file_stack.empty()) {
        string line;
        bool read_from_file = true;
        if (!repeat_info_stack.empty()) {
            if (current_idx < repeat_buffer.size()) {
                line = repeat_buffer[current_idx++];
                read_from_file = false;
            }
        }
        if (read_from_file) {
            if (!getline(file_stack.top(), line)) { // read a line of the file
                file_stack.pop();
            }
            file_lines_read_stack.top()++;

            if (!repeat_info_stack.empty()) {
                repeat_buffer.push_back(line);
                current_idx = repeat_buffer.size();
            }
        }

        // Ignore blank lines, including just all whitespace
        if (line.find_first_not_of(" ") == string::npos) {
            continue;
        }
        // Remove leading spaces, and anything after a comment
        line = line.substr(line.find_first_not_of(" "),
                           line.find_first_of("#"));
        // Ignore blank lines again after removing comments
        if (line.empty()) {
            continue;
        }

        stringstream line_ss(line);
        string command;
        if ((line_ss >> command).fail()) {
            print_parse_error(line);
            cerr << "Unable to parse command" << endl;
            return(false);
        }

        string args = ScanForSecondField(line);
        int global_id = -1;
        int scanCode;

        if (command == "begin_repeat") {
            int no_repeats;
            if ((line_ss >> no_repeats).fail()) {
                print_parse_error(line);
                cerr << "Unable to parse number of repeats" << endl;
                return(false);
            }
            repeat_info_stack.push(RepeatInfo());
            repeat_info_stack.top().no_repeats = no_repeats;
            repeat_info_stack.top().no_complete = 0;
            repeat_info_stack.top().starting_file = filename_stack.top();
            repeat_info_stack.top().starting_line = file_lines_read_stack.top();
            // The first line will be the next line put into the buffer
            repeat_info_stack.top().start_idx = current_idx;
        } else if (command == "end_repeat") {
            if (repeat_info_stack.empty()) {
                print_parse_error(line);
                cerr << "Found unpaired end_repeat" << endl;
                return(false);
            }
            repeat_info_stack.top().no_complete++;
            if (repeat_info_stack.top().no_complete <
                repeat_info_stack.top().no_repeats)
            {
                current_idx = repeat_info_stack.top().start_idx;
            } else {
                repeat_info_stack.pop();
                if (repeat_info_stack.empty()) {
                    repeat_buffer.clear();
                    current_idx = 0;
                }
            }
        } else if (command == "echo") {
            cout << "echo: " << args << endl;
        } else if (command == "isotope") {
            char string[256];
            int scanCode = sscanf(args.c_str(), "%s", string);
            if (scanCode != 1) {
                print_parse_error(line);
                return(false);
            }
            if (!sources.SetCurIsotope(string)) {
                print_parse_error(line);
                cerr << "invalid isotope: " << string << endl;
                return(false);
            }
            cout << "Debug: set isotope: " << string << endl;
        } else if (command == "voxel_src") {
            char string[256];
            VectorR3 position;
            position.SetZero();
            int dims[3];
            VectorR3 voxelsize;
            double activity;
            int scanCode = sscanf(args.c_str(), "%s %d %d %d %lf %lf %lf %lf",
                                  string,
                                  &dims[0],
                                  &dims[1],
                                  &dims[2],
                                  &voxelsize.x,
                                  &voxelsize.y,
                                  &voxelsize.z,
                                  &activity);
            if (scanCode != 8) {
                print_parse_error(line);
                return(false);
            }
            VoxelSource * s = new
            VoxelSource(position,dims,voxelsize,activity);
            if (s->Load(string)) {
                s->SetMaterial(curMaterial);
                sources.AddSource(s);
            }
        } else if (command == "include") {
            char string[256];
            int scanCode = sscanf(args.c_str(), "%s", string);
            // Reference all of the include files to the directory of the
            // top level file.
            std::string include_filename = file_dir + std::string(string);
            if (scanCode != 1) {
                print_parse_error(line);
                return(false);
            }
            file_stack.emplace(include_filename.c_str());
            if (!file_stack.top()) {
                print_parse_error(line);
                cerr << "Include File doesn't exist: "
                     << include_filename << endl;
                return(false);
            }
            file_lines_read_stack.push(0);
            filename_stack.push(include_filename);
            // Drop the include line from the repeat buffer if it's there, as
            // we just assume including a file is like dropping all of the text
            // onto that line.
            if (!repeat_buffer.empty()) {
                repeat_buffer.pop_back();
                current_idx--;
            }
        } else if (command == "increment") {
            VectorR3 StartPos;
            VectorR3 UnitSize;
            StartPos.x = 0.0;
            StartPos.y = 0.0;
            StartPos.z = 0.0;
            UnitSize.x = 1.0;
            UnitSize.y = 1.0;
            UnitSize.z = 1.0;

            global_id = detector_array.AddDetector(StartPos, UnitSize,
                                                   MatrixStack.top(),
                                                   0, 0, 0, 0);
        } else if (command == "ellipse") {
            VectorR3 center;
            VectorR3 axis1;
            VectorR3 axis2;
            double radius1;
            double radius2;
            double radius3;

            int scanCode = sscanf(args.c_str(),
                                  "%lf %lf %lf %lf %lf %lf %lf %lf %lf %lf %lf %lf",
                                  &(center.x), &(center.y), &(center.z),
                                  &(axis1.x), &(axis1.y), &(axis1.z),
                                  &(axis2.x), &(axis2.y), &(axis2.z),
                                  &radius1, &radius2, &radius3);
            if (scanCode != 12) {
                print_parse_error(line);
                return(false);
            }
            ViewableEllipsoid *ve = new ViewableEllipsoid();
            ve->SetCenter(center);
            ve->SetAxes(axis1, axis2);
            ve->SetRadii(radius3, radius2, radius1);
            ve->SetMaterial(curMaterial);
            TransformWithRigid(ve,MatrixStack.top());
            theScene.AddViewable(ve);
        } else if (command == "sp_ellipse") {
            VectorR3 center;
            VectorR3 axis1;
            VectorR3 axis2;
            double radius1;
            double radius2;
            double radius3;
            double activity;

            int scanCode = sscanf(args.c_str(),
                                  "%lf %lf %lf %lf %lf %lf %lf %lf %lf %lf %lf %lf %lf",
                                  &(center.x), &(center.y), &(center.z),
                                  &(axis1.x), &(axis1.y), &(axis1.z),
                                  &(axis2.x), &(axis2.y), &(axis2.z),
                                  &radius1, &radius2, &radius3, &activity);
            if (scanCode != 13) {
                print_parse_error(line);
                return(false);
            }
            cout << " New Ellipsoid Source " << endl;
            cout << " New Center :: "  << (double)center.x  << "  " ;
            cout << (double) center.y << "  " << (double) center.z <<  endl;
            cout << " New Axis   :: "  << (double)axis1.x  << "  " ;
            cout << (double) axis1.y << "  " << (double) axis1.z <<  endl;
            cout << " New Radius   :: "  << (double)radius1  << "  " ;
            cout << (double) radius2 << "  " << (double)radius3 <<  endl;
            EllipsoidSource *ve = new EllipsoidSource(
                    center, axis1, axis2, radius1, radius2, radius3,
                    actScale*activity);
            ve->SetMaterial(curMaterial);
            sources.AddSource(ve);
        } else if (command == "elliptic_cyl") {
            VectorR3 center;
            VectorR3 axis;
            double radius1;
            double radius2;
            double height;
            int scanCode = sscanf(args.c_str(),
                                  "%lf %lf %lf %lf %lf %lf %lf %lf %lf",
                                  &(center.x), &(center.y), &(center.z),
                                  &(axis.x), &(axis.y), &(axis.z),
                                  &radius1, &radius2, &height);
            cout << "Reading Cylinder Source " <<endl;
            if (scanCode != 9) {
                print_parse_error(line);
                return(false);
            }
            ViewableCylinder *vc = new ViewableCylinder();
            vc->SetCenterAxis(axis);
            vc->SetCenter(center);
            vc->SetRadii(radius2,radius1);
            vc->SetHeight(height);
            vc->SetMaterial(curMaterial);
            TransformWithRigid(vc,MatrixStack.top());
            theScene.AddViewable(vc);
        } else if (command == "sp_elliptic_cyl") {
            VectorR3 center;
            VectorR3 axis;
            double radius1;
            double radius2;
            double height;
            double activity;
            int scanCode = sscanf(args.c_str(),
                                  "%lf %lf %lf %lf %lf %lf %lf %lf %lf %lf",
                                  &(center.x), &(center.y), &(center.z),
                                  &(axis.x), &(axis.y), &(axis.z),
                                  &radius1, &radius2, &height, &activity);
            cout << "Reading Elliptic Cylinder Source " <<endl;
            if (scanCode != 10) {
                print_parse_error(line);
                return(false);
            }
            MatrixStack.top().Transform(&center);
            MatrixStack.top().Transform3x3(&axis);
            axis *= height;
            cout << " New Center :: "  << (double)center.x  << "  " ;
            cout << (double) center.y << "  " << (double) center.z <<  endl;
            cout << " New Axis   :: "  << (double)axis.x  << "  " ;
            cout << (double) axis.y << "  " << (double) axis.z <<  endl;
            EllipticCylinderSource * cyl = new EllipticCylinderSource(
                    center, radius1, radius2, axis, actScale*activity);
            cyl->SetMaterial(curMaterial);
            sources.AddSource(cyl);
        } else if (command == "sp_annulus_ell") {
            VectorR3 center;
            VectorR3 axis;
            double radius1;
            double radius2;
            double height;
            double activity;
            int scanCode = sscanf(args.c_str(),
                                  "%lf %lf %lf %lf %lf %lf %lf %lf %lf %lf",
                                  &(center.x), &(center.y), &(center.z),
                                  &(axis.x), &(axis.y), &(axis.z),
                                  &radius1, &radius2, &height, &activity);
            cout << "Reading Annulus Elliptic Cylinder Source " <<endl;
            if (scanCode != 10) {
                print_parse_error(line);
                return(false);
            }
            MatrixStack.top().Transform(&center);
            MatrixStack.top().Transform3x3(&axis);
            axis *= height;
            cout << " New Center :: "  << (double)center.x  << "  " ;
            cout << (double) center.y << "  " << (double) center.z <<  endl;
            cout << " New Axis   :: "  << (double)axis.x  << "  " ;
            cout << (double) axis.y << "  " << (double) axis.z <<  endl;
            AnnulusEllipticCylinderSource * cyl =
                    new AnnulusEllipticCylinderSource(center, radius1, radius2,
                                                      axis, actScale*activity);
            cyl->SetMaterial(curMaterial);
            sources.AddSource(cyl);
        } else if (command == "sp_annulus_cyl") {
            VectorR3 center;
            VectorR3 axis;
            double radius;
            double height;
            double activity;
            int scanCode = sscanf(args.c_str(),
                                  "%lf %lf %lf %lf %lf %lf %lf %lf %lf",
                                  &(center.x), &(center.y), &(center.z),
                                  &(axis.x), &(axis.y), &(axis.z),
                                  &radius, &height, &activity);
            cout << "Reading Annulus Cylinder Source " <<endl;
            if (scanCode != 9) {
                print_parse_error(line);
                return(false);
            }

            MatrixStack.top().Transform(&center);
            MatrixStack.top().Transform3x3(&axis);
            axis *= height;
            cout << " New Center :: "  << (double)center.x  << "  " ;
            cout << (double) center.y << "  " << (double) center.z <<  endl;
            cout << " New Axis   :: "  << (double)axis.x  << "  " ;
            cout << (double) axis.y << "  " << (double) axis.z <<  endl;
            AnnulusCylinderSource * cyl = new AnnulusCylinderSource(
                    center, radius, axis, actScale*activity);
            cyl->SetMaterial(curMaterial);
            sources.AddSource(cyl);
        } else if (command == "hits_format") {
            std::string format_identifier;
            if ((line_ss >> format_identifier).fail()) {
                print_parse_error(line);
                cerr << "Invalid format identifier: " << format_identifier
                     << endl;
                return(false);
            }
            Output::Format hits_format;
            if (Output::GetFormat(format_identifier, hits_format) < 0) {
                print_parse_error(line);
                cerr << "Invalid format identifier: " << format_identifier
                     << endl;
                return(false);
            }
            config.set_format_hits(hits_format);
        } else if (command == "singles_format") {
            std::string format_identifier;
            if ((line_ss >> format_identifier).fail()) {
                print_parse_error(line);
                cerr << "Invalid format identifier: " << format_identifier
                << endl;
                return(false);
            }
            Output::Format singles_format;
            if (Output::GetFormat(format_identifier, singles_format) < 0) {
                print_parse_error(line);
                cerr << "Invalid format identifier: " << format_identifier
                << endl;
                return(false);
            }
            config.set_format_singles(singles_format);
        } else if (command == "hits_output") {
            std::string filename;
            if ((line_ss >> filename).fail()) {
                print_parse_error(line);
                cerr << "Invalid filename: " << filename << endl;
                return(false);
            }
            config.set_filename_hits(filename);
        } else if (command == "hits_singles") {
            std::string filename;
            if ((line_ss >> filename).fail()) {
                print_parse_error(line);
                cerr << "Invalid filename: " << filename << endl;
                return(false);
            }
            config.set_filename_singles(filename);
        } else if (command == "pipeline_file") {
            std::string filename;
            if ((line_ss >> filename).fail()) {
                print_parse_error(line);
                cerr << "Invalid filename: " << filename << endl;
                return(false);
            }
            filename = file_dir + filename;
            config.set_filename_pipeline(filename);
        }  else if (command == "pipeline_config") {
            // Any line prefaced with pipeilne_config will be processed as a
            // pipeline file.
            config.set_filename_pipeline(args);
        } else if (command == "mapping_file") {
            std::string filename;
            if ((line_ss >> filename).fail()) {
                print_parse_error(line);
                cerr << "Invalid filename: " << filename << endl;
                return(false);
            }
            config.set_filename_mapping(filename);
        } else if (command == "save_detector") {
            char filename[256];
            int scanCode = sscanf(args.c_str(), "%s", filename);
            if (scanCode != 1) {
                print_parse_error(line);
                return(false);
            }
            filename_detector = std::string(filename);
        } else if (command == "save_basic_map") {
            if ((line_ss >> filename_basic_map).fail()) {
                print_parse_error(line);
                return(false);
            }
        } else if (command == "scale_act") {
            double t_actScale = -1.0;
            int scanCode = sscanf(args.c_str(), "%lf", &t_actScale);
            if (scanCode != 1) {
                print_parse_error(line);
                return(false);
            }
            actScale = t_actScale;
            cout << "scale act:" << actScale << "\n";
        } else if (command == "sphere") {
            // Sphere object
            VectorR3 position;
            double radius = -1.0;
            scanCode = sscanf(args.c_str(), "%lf %lf %lf %lf",
                              &(position.x), &(position.y), &(position.z),
                              &radius);
            if (scanCode != 4) {
                print_parse_error(line);
                return(false);
            }
            ViewableSphere * s = new ViewableSphere(position, radius);
            s->SetMaterial(curMaterial);
            TransformWithRigid(s,MatrixStack.top());
            theScene.AddViewable(s);
        } else if (command == "beam") {
            // beam pos_x pos_y pos_z axis_x axis_y axis_z angle activity
            // Beam source
            VectorR3 position;
            VectorR3 axis;
            double angle = -1.0;
            double activity = -1.0;
            scanCode = sscanf(args.c_str(), "%lf %lf %lf %lf %lf %lf %lf %lf",
                              &(position.x), &(position.y), &(position.z),
                              &(axis.x), &(axis.y), &(axis.z),
                              &angle,
                              &activity );
            if (scanCode != 8) {
                print_parse_error(line);
                return(false);
            }
            axis.Normalize();
            MatrixStack.top().Transform(&position);
            MatrixStack.top().Transform3x3(&axis);
            BeamPointSource * s = new BeamPointSource(position, axis, angle,
                                                      actScale*activity);
            s->SetMaterial(curMaterial);
        } else if (command == "acolinearity") {
            double acon = -1.0;
            scanCode = sscanf(args.c_str(), "%lf", &acon);
            if (scanCode != 1) {
                print_parse_error(line);
                return(false);
            }
            sources.SetAcolinearity(acon);
        } else if (command == "log_all") {
            Output::SetLogAll(true);
        } else if (command == "start_vecsrc") {
            double activity = -1.0;
            int scanCode = sscanf(args.c_str(), "%lf", &activity);
            if (scanCode != 1) {
                print_parse_error(line);
                return(false);
            }
            cout << "Starting Vector Source\n";
            curVectorSource = new VectorSource(actScale*activity);
            curVectorSource->SetMaterial(curMaterial);
            parse_VectorSource = true;
        } else if (command == "end_vecsrc") {
            char string[256];
            int scanCode = sscanf(args.c_str(), "%s", string);
            if ((scanCode != 1) || (curVectorSource == NULL)) {
                print_parse_error(line);
                return(false);
            }
            sources.AddSource(curVectorSource);
            cout << "Ending Vector Source:\n";
            cout << curVectorSource->GetMin();
            cout << "\n";
            cout << curVectorSource->GetMax();
            cout << "\n";
            parse_VectorSource = false;
            curVectorSource = NULL;
        } else if (command == "time") {
            // simulation time in seconds
            double simulationTime = 1.0;
            int scanCode = sscanf(args.c_str(), "%lf", &simulationTime);
            if (scanCode != 1) {
                print_parse_error(line);
                return(false);
            }
            sources.SetSimulationTime(simulationTime);
        } else if (command == "start_time") {
            // simulation time in seconds
            double start_time;
            if ((line_ss >> start_time).fail()) {
                print_parse_error(line);
                cerr << "Invalid start time" << endl;
                return(false);
            }
            sources.SetStartTime(start_time);
        } else if (command == "v") {
            viewCmdStatus = true;
        } else if (command == "scale") {
            double t_polygonScale = -1.0;
            int scanCode = sscanf(args.c_str(), "%lf", &t_polygonScale);
            if (scanCode != 1) {
                print_parse_error(line);
                return(false);
            }
            polygonScale = t_polygonScale;
        } else if (command == "seed") {
            unsigned long seed = 0;
            int scanCode = sscanf(args.c_str(), "%ld", &seed);
            if (scanCode != 1) {
                print_parse_error(line);
                return(false);
            }
            Random::Seed((unsigned long)seed);
        } else if (command == "log_positron") {
            Output::SetLogPositron(true);
        } else if (command == "sp_src") {
            // Sphere source
            VectorR3 position;
            double radius = -1.0;
            double activity = -1.0;
            scanCode = sscanf(args.c_str(), "%lf %lf %lf %lf %lf",
                              &(position.x), &(position.y), &(position.z),
                              &radius, &activity);
            if (scanCode != 5) {
                print_parse_error(line);
                return(false);
            }
            if (activity < 0.0) {
                cout << "Negative Source " << activity << " uCi" << endl;
            }

            ViewableSphere * sp = new ViewableSphere(position, radius);
            TransformWithRigid(sp,MatrixStack.top());

            MatrixStack.top().Transform(&position);
            SphereSource * s = new SphereSource(position, radius,
                                                actScale*activity);
            s->SetMaterial(curMaterial);
            sources.AddSource(s);
        } else if (command == "rect_src") {
            // rectangular source
            VectorR3 baseCenter;
            VectorR3 baseSize;
            double activity = -1.0;
            scanCode = sscanf(args.c_str(), "%lf %lf %lf %lf %lf %lf %lf",
                              &baseCenter.x, &baseCenter.y, &baseCenter.z,
                              &baseSize.x, &baseSize.y, &baseSize.z, &activity);
            if (scanCode != 7) {
                print_parse_error(line);
                return(false);
            }
            //TODO: FIX RECTANGULAR SOURCE ROTATION!
            MatrixStack.top().Transform(&baseCenter);
            RectSource * s = new RectSource(baseCenter, baseSize,
                                            actScale*activity);
            s->SetMaterial(curMaterial);
            sources.AddSource(s);
        } else if (command == "array") {
            // repeat detector in 3d
            VectorR3 UnitStep; // center to center between repeated cell
            VectorR3 UnitSize; // detector size
            int num_x = -1;
            int num_y = -1;
            int num_z = -1;
            scanCode = sscanf(args.c_str(), "%d %d %d %lf %lf %lf %lf %lf %lf",
                              &num_x, &num_y, &num_z,
                              &(UnitStep.x), &(UnitStep.y), &(UnitStep.z),
                              &(UnitSize.x), &(UnitSize.y), &(UnitSize.z));
            if (scanCode != 9) {
                print_parse_error(line);
                return(false);
            }
            VectorR3 StartPos;
            StartPos.x = 0.0;
            StartPos.y = 0.0;
            StartPos.z = 0.0;
            VectorR3 CurrentPos;

            UnitStep *= polygonScale;
            UnitSize *= polygonScale;

            StartPos.x = -1.0 * (double)(num_x-1) * UnitStep.x / 2.0;
            StartPos.y = -1.0 * (double)(num_y-1) * UnitStep.y / 2.0;
            StartPos.z = -1.0 * (double)(num_z-1) * UnitStep.z / 2.0;
            for (int i = 0; i < num_x; i++) {
                for (int j = 0; j < num_y; j++) {
                    for (int k = 0; k < num_z; k++) {
                        CurrentPos= StartPos;
                        CurrentPos.x += (double)i * UnitStep.x;
                        CurrentPos.y += (double)j * UnitStep.y;
                        CurrentPos.z += (double)k * UnitStep.z;
                        if (curMaterial->log_material == true) {
                            global_id = detector_array.AddDetector(
                                    CurrentPos, UnitSize, MatrixStack.top(),
                                    i, j, k, block_id);
                            ProcessDetector(CurrentPos, UnitSize,
                                            curMaterial, global_id,
                                            theScene, MatrixStack.top());
                        }  else {
                            ProcessDetector(CurrentPos, UnitSize,
                                            curMaterial, -1,
                                            theScene, MatrixStack.top());
                        }
                    }
                }
            }
            // Increment block detector id after a repeat statement
            if (curMaterial->log_material == true) {
                block_id++;
            }
        } else if (command == "cyl") {
            VectorR3 center;
            VectorR3 axis;
            double radius;
            double height;
            int scanCode = sscanf(args.c_str(),
                                  "%lf %lf %lf %lf %lf %lf %lf %lf",
                                  &(center.x), &(center.y), &(center.z),
                                  &(axis.x), &(axis.y), &(axis.z),
                                  &radius, &height);
            if (scanCode != 8) {
                print_parse_error(line);
                return(false);
            }
            ViewableCylinder *vc = new ViewableCylinder();
            vc->SetRadius(radius);
            vc->SetCenterAxis(axis);
            vc->SetCenter(center);
            vc->SetHeight(height);
            vc->SetMaterial(curMaterial);
            TransformWithRigid(vc,MatrixStack.top());
            theScene.AddViewable(vc);
        } else if (command == "cyl_src") {
            VectorR3 center;
            VectorR3 axis;
            double radius;
            double height;
            double activity;
            int scanCode = sscanf(args.c_str(),
                                  "%lf %lf %lf %lf %lf %lf %lf %lf %lf",
                                  &(center.x), &(center.y), &(center.z),
                                  &(axis.x), &(axis.y), &(axis.z),
                                  &radius, &height, &activity);
            cout << "Reading Cylinder Source " <<endl;
            if (scanCode != 9) {
                print_parse_error(line);
                return(false);
            }
            ViewableCylinder *vc = new ViewableCylinder();
            vc->SetRadius(radius);
            vc->SetCenterAxis(axis);
            vc->SetCenter(center);
            vc->SetHeight(height);
            vc->SetMaterial(curMaterial);
            TransformWithRigid(vc,MatrixStack.top());

            MatrixStack.top().Transform(&center);
            MatrixStack.top().Transform3x3(&axis);
            axis *= height;
            cout << " New Center :: "  << (double)center.x  << "  " ;
            cout << (double) center.y << "  " << (double) center.z <<  endl;
            cout << " New Axis   :: "  << (double)axis.x  << "  " ;
            cout << (double) axis.y << "  " << (double) axis.z <<  endl;
            CylinderSource * cyl = new CylinderSource(center, radius, axis,
                                                      actScale*activity);
            cyl->SetMaterial(curMaterial);
            sources.AddSource(cyl);
        } else if (command == "l") {
            // light
            VectorR3 lightPos, lightColor;
            scanCode = sscanf(args.c_str(), "%lf %lf %lf %lf %lf %lf",
                              &(lightPos.x), &(lightPos.y), &(lightPos.z),
                              &(lightColor.x), &(lightColor.y),
                              &(lightColor.z));
            if ((scanCode != 3) && (scanCode != 6)) {
                print_parse_error(line);
                return(false);
            }
            Light * aLight = new Light();
            aLight->SetPosition(lightPos);
            if (scanCode == 6) {
                aLight->SetColor(lightColor);
            }
            theScene.AddLight(aLight);
        } else if (command == "t") {
            // translate
            VectorR3 trans;
            scanCode = sscanf(args.c_str(), "%lf %lf %lf",
                              &(trans.x), &(trans.y), &(trans.z));
            if (scanCode != 3) {
                print_parse_error(line);
                return(false);
            }

            trans *= polygonScale;
            MatrixStack.top().ApplyTranslationLeft(trans);
        } else if (command == "push") {
            // push matrix
            // Add a copy of the current matrix to the top of the stack
            MatrixStack.push(MatrixStack.top());
        } else if (command == "pop") {
            MatrixStack.pop();
        } else if (command == "b") {
            // background color
            VectorR3 bgColor;
            int scanCode = sscanf(args.c_str(), "%lf %lf %lf",
                                  &(bgColor.x), &(bgColor.y), &(bgColor.z));
            if (scanCode != 3) {
                print_parse_error(line);
                return(false);
            }
            theScene.SetBackGroundColor(bgColor);
        } else if (command == "raxis") {
            // raxis
            VectorR3 axis;
            double degree;
            int scanCode = sscanf(args.c_str(), "%lf %lf %lf %lf",
                                  &(axis.x), &(axis.y), &(axis.z), &degree);
            if (scanCode != 4) {
                print_parse_error(line);
                return(false);
            }
            ApplyRotation(axis, degree * (M_PI/180.0), MatrixStack.top());
        } else if (command == "from") {
            int scanCode = sscanf(args.c_str(), "%lf %lf %lf",
                                  &(viewPos.x), &(viewPos.y), &(viewPos.z));
            if (scanCode != 3) {
                print_parse_error(line);
                return(false);
            }
        } else if (command == "at") {
            int scanCode = sscanf(args.c_str(), "%lf %lf %lf", &(lookAtPos.x),
                                  &(lookAtPos.y), &(lookAtPos.z));
            if (scanCode != 3) {
                print_parse_error(line);
                return(false);
            }
        } else if (command == "up") {
            int scanCode = sscanf(args.c_str(), "%lf %lf %lf",
                                  &(upVector.x), &(upVector.y), &(upVector.z));
            if (scanCode != 3) {
                print_parse_error(line);
                return(false);
            }
        } else if (command == "angle") {
            // Angle in degrees
            int scanCode = sscanf(args.c_str(), "%lf", &fovy);
            if (scanCode != 1) {
                print_parse_error(line);
                return(false);
            }
            fovy *= M_PI / 180.0;
        } else if (command == "hither") {
            int scanCode = sscanf(args.c_str(), "%lf", &hither);
            if (scanCode != 1) {
                print_parse_error(line);
                return(false);
            }
        } else if (command == "resolution") {
            int scanCode = sscanf(args.c_str(), "%d %d",
                                  &screenWidth, &screenHeight);
            if (scanCode != 2) {
                print_parse_error(line);
                return(false);
            }
        } else if (command == "p") {
            int numVerts;
            const int maxNumVerts = 256;
            int scanCode = sscanf(args.c_str(), "%d", &numVerts);
            if (scanCode != 1) {
                print_parse_error(line);
                return(false);
            }
            if (numVerts < 3) {
                print_parse_error(line);
                cerr << "Number of vertices is less than 3" << endl;
                return(false);
            }
            if (numVerts > maxNumVerts) {
                print_parse_error(line);
                cerr << "Number of vertices is greater than " << maxNumVerts
                     << endl;
                return(false);
            }
            // FIXED: arbitrary triangles must use increment to advance detector ids
            // FIXED: detector only is used when material is sensitive
            if (curMaterial->log_material) {
                ProcessFaceDFF(numVerts, curMaterial, file_stack.top(),
                               curVectorSource, parse_VectorSource,
                               global_id, theScene, polygonScale,
                               MatrixStack.top());
            } else {
                ProcessFaceDFF(numVerts, curMaterial, file_stack.top(),
                               curVectorSource, parse_VectorSource, -1,
                               theScene, polygonScale,
                               MatrixStack.top());
            }
        } else if (command == "m") {
            // material index
            int matIndex = theScene.GetMaterialIndex(args);
            if (matIndex < 0) {
                print_parse_error(line);
                cerr << "Invalid material: " << args << endl;
                return(false);
            }
            curMaterial = dynamic_cast<GammaMaterial*>(&theScene.GetMaterial(matIndex));
        } else if (command == "color") {
            VectorR3 ambient;
            VectorR3 diffuse;
            VectorR3 spec;
            VectorR3 trans;
            VectorR3 reflec;
            double ior;
            int scanCode = sscanf(args.c_str(),
                                  "%lf %lf %lf %lf %lf %lf %lf %lf %lf %lf %lf %lf %lf %lf %lf %lf",
                                  &(ambient.x), &(ambient.y), &(ambient.z),
                                  &(diffuse.x), &(diffuse.y), &(diffuse.z),
                                  &(spec.x), &(spec.y), &(spec.z),
                                  &(trans.x), &(trans.y), &(trans.z),
                                  &(reflec.x), &(reflec.y), &(reflec.z),
                                  &ior);
            if (scanCode != 16) {
                print_parse_error(line);
                return(false);
            }
            curMaterial->SetColorAmbient(ambient);
            curMaterial->SetColorDiffuse(diffuse);
            curMaterial->SetColorSpecular(spec);
            curMaterial->SetColorTransmissive(trans);
            curMaterial->SetColorReflective(reflec);
            curMaterial->SetIndexOfRefraction(ior);
        } else if (command == "k") {
            VectorR3 baseCenter;
            VectorR3 baseSize;
            int scanCode = sscanf(args.c_str(), "%lf %lf %lf %lf %lf %lf",
                                  &baseCenter.x, &baseCenter.y, &baseCenter.z,
                                  &baseSize.x, &baseSize.y, &baseSize.z);
            if (scanCode != 6) {
                print_parse_error(line);
                return(false);
            }
            baseCenter *= polygonScale;
            baseSize *= polygonScale;
            // FIXED: detector only is used when material is sensitive
            if (curMaterial->log_material) {
                global_id = detector_array.AddDetector(baseCenter,
                                                       baseSize,
                                                       MatrixStack.top(),
                                                       0, 0, 0,
                                                       block_id);
                block_id++;
                ProcessDetector(baseCenter, baseSize, curMaterial,
                                global_id, theScene, MatrixStack.top());
            } else {
                ProcessDetector(baseCenter, baseSize, curMaterial, -1,
                                theScene, MatrixStack.top());
            }
        } else {
            print_parse_error(line);
            cerr << "command not found: " << command << endl;
            return(false);
        }
    }

    if (!repeat_info_stack.empty()) {
        cerr << "unpaired begin_repeat found in: \""
             << repeat_info_stack.top().starting_file << "\", line: "
             << repeat_info_stack.top().starting_line << endl;
        return(false);
    }

    if (viewCmdStatus) {
        SetCameraViewInfo(theScene.GetCameraView(),
                          viewPos, lookAtPos, upVector, fovy,
                          screenWidth, screenHeight, hither);
    }
    if (filename_detector != "") {
        ofstream det_file(filename_detector.c_str());
        det_file << detector_array;
        det_file.close();
    }
    if (filename_basic_map != "") {
        ofstream map_file(filename_basic_map);
        if (!map_file) {
            cerr << "Unable to open basic map file: " << filename_basic_map
                 << endl;
            return(false);
        }
        detector_array.WriteBasicMap(map_file, "detector", "block", "bx", "by",
                                     "bz");

    }
    return(true);
}

void LoadDetector::ProcessDetector(const VectorR3 & detCenter,
                                   const VectorR3 & detSize,
                                   const Material * curMaterial,
                                   int id,
                                   SceneDescription & scene,
                                   const RigidMapR3 & current_matrix)
{

    ViewableTriangle *vt[12];
    for (int i = 0; i < 12; i++) {
        vt[i] = new ViewableTriangle();
        vt[i]->SetMaterialFront(curMaterial);
        vt[i]->SetMaterialBack(curMaterial);
        vt[i]->SetDetectorId(id);
    }

    VectorR3 va;
    VectorR3 vb;
    VectorR3 vc;
    VectorR3 vd;
    VectorR3 ve;
    VectorR3 vf;
    VectorR3 vg;
    VectorR3 vh;

    VectorR3 sz_tmp = detSize;
    sz_tmp /= 2.0;

    va = vb = vc = vd = ve = detCenter;
    vf = vg = vh = detCenter;
    va.x -= sz_tmp.x;
    va.y -= sz_tmp.y;
    va.z += sz_tmp.z;
    vb.x += sz_tmp.x;
    vb.y -= sz_tmp.y;
    vb.z += sz_tmp.z;
    vc.x -= sz_tmp.x;
    vc.y += sz_tmp.y;
    vc.z += sz_tmp.z;
    vd.x -= sz_tmp.x;
    vd.y -= sz_tmp.y;
    vd.z -= sz_tmp.z;

    ve.x += sz_tmp.x;
    ve.y += sz_tmp.y;
    ve.z += sz_tmp.z; // same plane as A C B
    vf.x -= sz_tmp.x;
    vf.y += sz_tmp.y;
    vf.z -= sz_tmp.z; // same plane as A C D
    vg.x += sz_tmp.x;
    vg.y += sz_tmp.y;
    vg.z -= sz_tmp.z; // same plane as B E H
    vh.x += sz_tmp.x;
    vh.y -= sz_tmp.y;
    vh.z -= sz_tmp.z; // same plane as B E G

    vt[0]->Init(va, vb, vc);
    vt[1]->Init(va, vc, vd);
    vt[2]->Init(va, vd, vb);
    vt[3]->Init(ve, vc, vb);
    vt[4]->Init(vf, vd, vc);
    vt[5]->Init(vc, ve, vf);

    vt[6]->Init(vg, vf, ve);

    vt[7]->Init(ve, vb, vg);
    vt[8]->Init(vh, vg, vb);

    vt[9]->Init(vg, vh, vf);
    vt[10]->Init(vd, vf, vh);
    vt[11]->Init(vh, vb, vd);

    for (int i = 0; i < 12; i++) {
        TransformWithRigid(vt[i], current_matrix);
        scene.AddViewable(vt[i]);
    }
}

void LoadDetector::SetCameraViewInfo(CameraView& theView,
                                     const VectorR3& viewPos,
                                     const VectorR3& lookAtPos,
                                     const VectorR3& upVector, double fovy,
                                     int screenWidth, int screenHeight,
                                     double nearClipping)
{
    theView.SetLookAt( viewPos, lookAtPos, upVector );

    bool IgnoreResolution = true;

    if ( IgnoreResolution ) {
        double screenDistance = theView.GetScreenDistance();
        // NFF files always use aspect ratio of one
        // Resize view of theView to encompass the NFF window and more
        // But do not change the aspect ratio or pixel dimensions
        double nffScreenSize = 2.0*screenDistance*tan(fovy*0.5);
        double r = theView.GetAspectRatio();
        if ( r<=1.0 ) {
            theView.SetScreenDimensions( nffScreenSize, nffScreenSize/r );
        } else {
            theView.SetScreenDimensions( r*nffScreenSize, nffScreenSize );
        }
        theView.SetClippingDistances( theView.GetNearClippingDist(), -100.0 );
    } else {
        theView.SetViewFrustum( 1.0, fovy );
        theView.SetScreenPixelSize( screenWidth, screenHeight );
    }
}

bool LoadDetector::ProcessFaceDFF(int numVerts,
                                  const Material* curMaterial,
                                  std::ifstream & curFile,
                                  VectorSource *s,
                                  bool parse_VectorSource,
                                  unsigned id,
                                  SceneDescription & scene,
                                  double polygonScale,
                                  const RigidMapR3 & current_matrix)
{
    VectorR3 firstVert, prevVert, thisVert;
    if ( !ReadVertexR3(firstVert, curFile) ) {
        return false;
    }
    if ( !ReadVertexR3(prevVert, curFile) ) {
        return false;
    }
    int i;
    for ( i=2; i<numVerts; i++ ) {
        if ( !ReadVertexR3(thisVert, curFile) ) {
            return false;
        }
        firstVert *= polygonScale;
        prevVert *= polygonScale;
        thisVert *= polygonScale;

        ViewableTriangle* vt = new ViewableTriangle();
        vt->SetDetectorId(id);
        vt->Init( firstVert, prevVert, thisVert );
        vt->SetMaterialFront( curMaterial );
        vt->SetMaterialBack( curMaterial );
        TransformWithRigid(vt, current_matrix);

        if (parse_VectorSource) {
            s->SetMin(vt->GetVertexA());
            s->SetMax(vt->GetVertexA());
            s->SetMin(vt->GetVertexB());
            s->SetMax(vt->GetVertexB());
            s->SetMin(vt->GetVertexC());
            s->SetMax(vt->GetVertexC());
            vt->SetSrcId(1);
        } else {
            vt->SetSrcId(0);
        }

        scene.AddViewable( vt );
        prevVert = thisVert;
    }
    return true;
}

bool LoadDetector::ReadVertexR3(VectorR3& vert, std::ifstream & curFile)
{
    string line;
    if (!getline(curFile, line)) {
        return false;
    }
    int scanCode;
    scanCode = sscanf(line.c_str(), "%lf %lf %lf", &vert.x, &vert.y, &vert.z );
    return (scanCode == 3);
}

std::string LoadDetector::ScanForSecondField(const std::string & inbuf)
{
    size_t white_space = inbuf.find_first_of(' ');
    if (white_space == string::npos) {
        return("");
    }
    white_space += inbuf.substr(white_space).find_first_not_of(' ');
    if (white_space == string::npos) {
        return("");
    }
    return (inbuf.substr(white_space));
}
