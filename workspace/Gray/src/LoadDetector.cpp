
#include <stdio.h>
#include <string.h>
#include <Random/Random.h>
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

namespace {
const int numCommands = 60;
const char * dffCommandList[numCommands] = {
    "p",                // 0 Polygon patches
    "m",                // 1 Material index
    "color",            // 2 Color and reflection and transmission
    "k",                // 3 detector position and size
    "from",             // 4 Parameter to "v" command: eye position
    "at",               // 5 Parameter to "v" command: "look at" position
    "up",               // 6 Parameter to "v" command: "up" direction
    "angle",            // 7 Parameter to "v" command: fov angle
    "hither",           // 8 Parameter to "v" command: near clipping distance
    "resolution",       // 9 Parameter to "v" command: resolution
    "l",                // 10 add lightsource
    "t",                // 11 translate current matrix stack by coordinates
    "push",             // 12 push matrix
    "pop",              // 13 pop matrix
    "b",                // 14 Background color
    "raxis",            // 15 rotate around axis, axis x,y,z theta
    "sp_src",           // 16 sphere source
    "rect_src",         // 17 rectangular source
    "array",            // 18 array of detectors
    "cyl",              // 19 add a cylinder
    "cyl_src",          // 20 cylinder source
    "#",                // 21 comments
    "time",             // 22 Set Simulation Time
    "v",                // 23 view matrix
    "scale",            // 24 set polygon scale
    "seed",             // 25 set integer seed
    "log_positron",     // 26 turn on positron logging
    "sphere",           // 27 load sphere geometry
    "beam",             // 28 beam source
    "acolinearity",     // 29 acolinearity
    "log_all",          // 30 log all interactions
    "binary_output",    // 31 enable binary data output
    "binning",          // 32 enable binning x
    "start_vecsrc",     // 33 start vector source
    "end_vecsrc",       // 34 end vector source
    "log_det_id",       // 35 log detector id x
    "log_det_coord",    // 36 log detector coord x
    "save_detector",	// 37 save detector to a file
    "scale_act",		// 38 scale activity
    "save_coinc",		// 39 set coincidence file x
    "save_singles",		// 40 set singles file x
    "save_cp",          // 41 set coincidence process file x
    "time_resolution",	// 42 set time_resolution of detectors x
    "eres",             // 43 set energy_resolution of detectors x
    "time_gate",		// 44 set coincidence time gate x
    "energy_gate",		// 45 set coincidence energy gate x
    "del_window",		// 46 set coincidence delayed window x
    "pos_range",		// 47 set positron range x
    "setFBP2D",         // 48 set 2D mode coincidence x
    "isotope",          // 49 set isotope
    "voxel_src",		// 50 load voxel array source
    "include",          // 51 include a dff file into current file
    "increment",		// 52 increment detector id
    "ellipse",          // 53 load ellipse geometry
    "sp_ellipse",		// 54 ellipse source
    "elliptic_cyl",		// 55 elliptic cylinder
    "sp_elliptic_cyl",	// 56 elliptic cylinder source
    "sp_annulus_ell", 	// 57 annulus ellipse source
    "sp_annulus_cyl",	// 58 annulus cylinder source
    "binary_format"     // 59 set the binary output format
};
}

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
                        SourceList & sources)
{
    DetectorArray detector_array;
    std::string filename_detector = "";
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
    bool parseErrorOccurred = false;

    // Vectorial Source parsing
    bool parse_VectorSource = false;
    VectorSource * curVectorSource = NULL;

    
    GammaMaterial* curMaterial = dynamic_cast<GammaMaterial*>(&theScene.GetMaterial(0));

    struct RepeatInfo {
        int no_repeats;
        int no_complete;
        size_t start_idx;
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
                if (!repeat_info_stack.empty()) {
                    print_parse_error(line);
                    cerr << "unpaired begin_repeat found in: \""
                         << filename_stack.top() << "\"" << endl;
                    return(false);
                }
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
        line = line.substr(line.find_first_not_of(" "), line.find_first_of("#"));
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
        } else {
            int cmdNum = GetCommandNumber(command);
            if (cmdNum < 0) {
                print_parse_error(line);
                cerr << "command not found: " << command << endl;
                return(false);
            }
            switch (cmdNum) {
                case 0: {
                    int numVerts;
                    const int maxNumVerts = 256;
                    scanCode = sscanf(args.c_str(), "%d", &numVerts );
                    if (scanCode!=1 || numVerts<3 ) {
                        parseErrorOccurred = true;
                    } else if ( numVerts>maxNumVerts ) {
                        //UnsupportedTooManyVerts( maxNumVerts );
                    } else {
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
                    }
                    break;
                }
                case 1: { // material index
                    int matIndex = theScene.GetMaterialIndex(args);
                    if (matIndex >= 0) {
                        curMaterial = dynamic_cast<GammaMaterial*>(&theScene.GetMaterial(matIndex));
                    } else {
                        parseErrorOccurred = true;
                    }
                    break;
                }
                case 2: { // color of material
                    VectorR3 ambient;
                    VectorR3 diffuse;
                    VectorR3 spec;
                    VectorR3 trans;
                    VectorR3 reflec;
                    double ior;
                    int scanCode = sscanf(args.c_str(), "%lf %lf %lf %lf %lf %lf %lf %lf %lf %lf %lf %lf %lf %lf %lf %lf",
                                           &(ambient.x), &(ambient.y), &(ambient.z),
                                           &(diffuse.x), &(diffuse.y), &(diffuse.z),
                                           &(spec.x), &(spec.y), &(spec.z),
                                           &(trans.x), &(trans.y), &(trans.z),
                                           &(reflec.x), &(reflec.y), &(reflec.z),
                                           &ior);
                    if (scanCode != 16) {
                        parseErrorOccurred = true;
                        break;
                    } else {
                        curMaterial->SetColorAmbient(ambient);
                        curMaterial->SetColorDiffuse(diffuse);
                        curMaterial->SetColorSpecular(spec);
                        curMaterial->SetColorTransmissive(trans);
                        curMaterial->SetColorReflective(reflec);
                        curMaterial->SetIndexOfRefraction(ior);
                    }
                    break;
                }
                case 3: // detector size and position
                    // TODO: add detector orientation -- may not be needed because raxis works
                    // each single detector is considered a block, use repeat to make a block detector
                {
                    VectorR3 baseCenter;
                    VectorR3 baseSize;
                    scanCode = sscanf(args.c_str(), "%lf %lf %lf %lf %lf %lf",
                                       &baseCenter.x, &baseCenter.y, &baseCenter.z,
                                       &baseSize.x, &baseSize.y, &baseSize.z );
                    if (scanCode == 6) {
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
                        parseErrorOccurred = true;
                    }
                    break;
                }
                case 4: { // from
                    scanCode = sscanf(args.c_str(), "%lf %lf %lf", &(viewPos.x), &(viewPos.y), &(viewPos.z) );
                    if (scanCode != 3) {
                        parseErrorOccurred = true;
                    }
                    break;
                }
                case 5: { // lookat
                    scanCode = sscanf(args.c_str(), "%lf %lf %lf", &(lookAtPos.x), &(lookAtPos.y), &(lookAtPos.z) );
                    if (scanCode != 3) {
                        parseErrorOccurred = true;
                    }
                    break;
                }
                case 6: { // up
                    scanCode = sscanf(args.c_str(), "%lf %lf %lf", &(upVector.x), &(upVector.y), &(upVector.z) );
                    if (scanCode != 3) {
                        parseErrorOccurred = true;
                    }
                    break;
                }
                case 7: { // angle
                    scanCode = sscanf(args.c_str(), "%lf", &fovy );
                    if (scanCode != 1) {
                        parseErrorOccurred = true;
                    }
                    fovy *= M_PI / 180.0;
                    break;
                }
                case 8: { // hither
                    scanCode = sscanf(args.c_str(), "%lf", &hither );
                    if (scanCode != 1) {
                        parseErrorOccurred = true;
                    }
                    break;
                }
                case 9: { // resolution
                    scanCode = sscanf(args.c_str(), "%d %d", &screenWidth, &screenHeight );
                    if (scanCode != 2) {
                        parseErrorOccurred = true;
                    }
                    break;
                }
                case 10: { // light
                    VectorR3 lightPos, lightColor;
                    scanCode = sscanf(args.c_str(), "%lf %lf %lf %lf %lf %lf",
                                       &(lightPos.x), &(lightPos.y), &(lightPos.z),
                                       &(lightColor.x), &(lightColor.y), &(lightColor.z) );
                    if ( scanCode==3 || scanCode==6 ) {
                        Light* aLight = new Light();
                        aLight->SetPosition( lightPos );
                        if ( scanCode==6 ) {
                            aLight->SetColor( lightColor );
                        }
                        theScene.AddLight( aLight );
                    } else {
                        parseErrorOccurred = true;
                    }
                    break;
                }
                case 11: { // translate
                    VectorR3 trans;
                    scanCode = sscanf(args.c_str(), "%lf %lf %lf",&(trans.x), &(trans.y), &(trans.z));

                    trans *= polygonScale;

                    if (scanCode == 3) {
                        //MatrixStack.top().Transform3x3(&trans);
                        MatrixStack.top().ApplyTranslationLeft(trans);
                    } else {
                        parseErrorOccurred = true;
                    }
                    break;
                }
                case 12: { // push matrix
                    // Add a copy of the current matrix to the top of the stack
                    MatrixStack.push(MatrixStack.top());
                    break;
                }
                case 13: { // pop matrix
                    MatrixStack.pop();
                    break;
                }
                case 14: { // background color
                    VectorR3 bgColor;
                    scanCode = sscanf(args.c_str(), "%lf %lf %lf", &(bgColor.x), &(bgColor.y), &(bgColor.z) );
                    if ( scanCode!=3 ) {
                        parseErrorOccurred = true;
                        break;
                    }
                    theScene.SetBackGroundColor( bgColor );
                    break;
                }
                case 15: { // raxis
                    VectorR3 axis;
                    double degree;
                    scanCode = sscanf(args.c_str(), "%lf %lf %lf %lf", &(axis.x), &(axis.y), &(axis.z), &degree );
                    if (scanCode != 4) {
                        break;
                    }
                    ApplyRotation(axis, degree * (M_PI/180.0), MatrixStack.top());
                    break;
                }
                case 16: {
                    // Sphere source
                    VectorR3 position;
                    double radius = -1.0;
                    double activity = -1.0;
                    scanCode = sscanf(args.c_str(), "%lf %lf %lf %lf %lf", &(position.x), &(position.y), &(position.z), &radius, &activity );
                    if (scanCode == 5) {
                        if (activity < 0.0) {
                            cout << "Negative Source " << activity << " uCi" << endl;
                        }

                        ViewableSphere * sp = new ViewableSphere(position, radius);
                        TransformWithRigid(sp,MatrixStack.top());

                        MatrixStack.top().Transform(&position);
                        SphereSource * s = new SphereSource(position, radius, actScale*activity);
                        s->SetMaterial(curMaterial);
                        sources.AddSource(s);
                    } else {
                        parseErrorOccurred = true;
                        break;
                    }
                    break;
                }
                case 17: { // rectangular source
                    VectorR3 baseCenter;
                    VectorR3 baseSize;
                    double activity = -1.0;
                    scanCode = sscanf(args.c_str(), "%lf %lf %lf %lf %lf %lf %lf",
                                       &baseCenter.x, &baseCenter.y, &baseCenter.z,
                                       &baseSize.x, &baseSize.y, &baseSize.z, &activity );
                    if (scanCode == 7) {
                        //TODO: FIX RECTANGULAR SOURCE ROTATION!
                        MatrixStack.top().Transform(&baseCenter);
                        RectSource * s = new RectSource(baseCenter, baseSize,actScale*activity);
                        s->SetMaterial(curMaterial);
                        sources.AddSource(s);
                    } else {
                        parseErrorOccurred = true;
                    }
                    break;
                }
                case 18: { // repeat detector in 3d
                    VectorR3 UnitStep; // center to center between repeated cell
                    VectorR3 UnitSize; // detector size
                    int num_x = -1;
                    int num_y = -1;
                    int num_z = -1;
                    scanCode = sscanf(args.c_str(), "%d %d %d %lf %lf %lf %lf %lf %lf", &num_x, &num_y, &num_z, &(UnitStep.x), &(UnitStep.y), &(UnitStep.z),
                                       &(UnitSize.x), &(UnitSize.y), &(UnitSize.z));
                    if (scanCode != 9) {
                        parseErrorOccurred = true;
                        break;
                    } else {
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
                    }
                    break;
                }
                case 19: {
                    VectorR3 center;
                    VectorR3 axis;
                    double radius;
                    double height;

                    int scanCode = sscanf(args.c_str(), "%lf %lf %lf %lf %lf %lf %lf %lf",
                                           &(center.x), &(center.y), &(center.z),
                                           &(axis.x), &(axis.y), &(axis.z),
                                           &radius, &height);
                    if (scanCode != 8) {
                        parseErrorOccurred = true;
                        break;
                    } else {
                        ViewableCylinder *vc = new ViewableCylinder();
                        vc->SetRadius(radius);
                        vc->SetCenterAxis(axis);
                        vc->SetCenter(center);
                        vc->SetHeight(height);
                        vc->SetMaterial(curMaterial);
                        TransformWithRigid(vc,MatrixStack.top());
                        theScene.AddViewable(vc);
                    }
                    break;
                }
                case 20: {
                    VectorR3 center;
                    VectorR3 axis;
                    double radius;
                    double height;
                    double activity;
                    int scanCode = sscanf(args.c_str(), "%lf %lf %lf %lf %lf %lf %lf %lf %lf",
                                           &(center.x), &(center.y), &(center.z),
                                           &(axis.x), &(axis.y), &(axis.z),
                                           &radius, &height, &activity);
                    cout << "Reading Cylinder Source " <<endl;
                    if (scanCode == 9) {

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
                        CylinderSource * cyl = new CylinderSource(center, radius, axis, actScale*activity);
                        cyl->SetMaterial(curMaterial);
                        sources.AddSource(cyl);

                    } else {
                        parseErrorOccurred = true;
                        break;
                    }
                    break;
                }
                case 21: { // # comment
                    continue;
                    break;
                }
                case 22: { // simulation time in seconds
                    double simulationTime = 1.0;
                    int scanCode = sscanf(args.c_str(), "%lf", &simulationTime);
                    if (scanCode != 1) {
                        parseErrorOccurred = true;
                        break;
                    } else {
                        sources.SetSimulationTime(simulationTime);
                    }
                    break;
                }
                case 23: { // read view command
                    viewCmdStatus = true;
                    break;
                }
                case 24: { // polygon scale
                    double t_polygonScale = -1.0;
                    int scanCode = sscanf(args.c_str(), "%lf", &t_polygonScale);
                    if (scanCode ==1) {
                        polygonScale = t_polygonScale;
                    } else {
                        parseErrorOccurred = true;
                        break;
                    }
                    break;
                }
                case 25: { // input long integer random seed
                    unsigned long seed = 0;
                    int scanCode = sscanf(args.c_str(), "%ld", &seed);
                    if (scanCode ==1) {
                        Random::Seed((unsigned long)seed);
                    } else {
                        parseErrorOccurred = true;
                        break;
                    }
                    break;
                }
                case 26: {
                    Output::SetLogPositron(true);
                    break;
                }
                case 27: {
                    // Sphere object
                    VectorR3 position;
                    double radius = -1.0;
                    scanCode = sscanf(args.c_str(), "%lf %lf %lf %lf", &(position.x), &(position.y), &(position.z), &radius );
                    if (scanCode == 4) {
                        ViewableSphere * s = new ViewableSphere(position, radius);
                        s->SetMaterial(curMaterial);
                        TransformWithRigid(s,MatrixStack.top());
                        theScene.AddViewable(s);
                    } else {
                        parseErrorOccurred = true;
                        break;
                    }
                    break;
                }
                case 28: { // beam pos_x pos_y pos_z axis_x axis_y axis_z angle activity
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
                    if (scanCode == 8) {
                        axis.Normalize();
                        MatrixStack.top().Transform(&position);
                        MatrixStack.top().Transform3x3(&axis);
                        BeamPointSource * s = new BeamPointSource(position, axis, angle, actScale*activity);
                        s->SetMaterial(curMaterial);
                        sources.AddSource(s);
                    } else {
                        parseErrorOccurred = true;
                        break;
                    }
                    break;
                }
                case 29: { // acolinearity degrees fwhm
                    double acon = -1.0;
                    scanCode = sscanf(args.c_str(), "%lf", &acon);
                    if (scanCode == 1) {
                        sources.SetAcolinearity(acon);
                    } else {
                        parseErrorOccurred = true;
                        break;
                    }
                    break;
                }
                case 30: { // log_all true/false logs all interactions
                    Output::SetLogAll(true);
                    break;
                }
                case 31: { // Set binary file io
                    Output::SetBinary(true);
                    break;
                }
                case 32: { // Set spectial detector binning TODO: implement binning
                    cout << "Warning: Binning not implemented\n";
                    break;
                }
                case 33: { // Start Vector Source
                    double activity = -1.0;
                    int scanCode = sscanf(args.c_str(), "%lf", &activity);
                    if (scanCode ==1) {
                        cout << "Starting Vector Source\n";
                        curVectorSource = new VectorSource(actScale*activity);
                        curVectorSource->SetMaterial(curMaterial);
                        parse_VectorSource = true;
                    } else {
                        parseErrorOccurred = true;
                        break;
                    }
                    break;
                }
                case 34: { // End Vector Source
                    char string[256];
                    int scanCode = sscanf(args.c_str(), "%s", string);
                    if ((scanCode ==1) && (curVectorSource != NULL)) {
                        sources.AddSource(curVectorSource);
                        cout << "Ending Vector Source:\n";
                        cout << curVectorSource->GetMin();
                        cout << "\n";
                        cout << curVectorSource->GetMax();
                        cout << "\n";
                        parse_VectorSource = false;
                        curVectorSource = NULL;

                    } else {
                        parseErrorOccurred = true;
                        break;
                    }
                    break;
                }
                case 35: { // Log Detector Id
                    cout << "Warning: Log Detector Id is always on" << endl;
                    break;
                }
                case 36: { // Log Detector Coordinates
                    cout << "Warning: Log Detector Coordinates is always off" << endl;
                    break;
                }
                case 37: { // Save detector
                    char filename[256];
                    int scanCode = sscanf(args.c_str(), "%s", filename);
                    if (scanCode ==1) {
                        filename_detector = std::string(filename);
                    } else {
                        parseErrorOccurred = true;
                        break;
                    }
                    break;
                }
                case 38: { // activity scale
                    double t_actScale = -1.0;
                    int scanCode = sscanf(args.c_str(), "%lf", &t_actScale);
                    if (scanCode ==1) {
                        actScale = t_actScale;
                        cout << "scale act:";
                        cout << actScale;
                        cout << "\n";
                    } else {
                        parseErrorOccurred = true;
                        break;
                    }
                    break;
                }
                case 39: { // set coincidence filename
                    cout << "Warning: Coincidence Processor has been removed: " << endl;
                    break;
                }
                case 40: { // set singles filename
                    cout << "Warning: Singles Processor has been removed: " << endl;
                    break;
                }
                case 41: { // set coincidence processor file
                    cout << "Warning: Coincidence Processor has been removed: " << endl;
                    break;
                }
                case 42: { // set time resolution of detectors
                    cout << "Warning: Singles Processor has been removed: " << endl;
                    break;
                }
                case 43: { // set energy resolution of detectors
                    cout << "Warning: Singles Processor has been removed: " << endl;
                    break;
                }
                case 44: { // set time gate of coincidence processor
                    cout << "Warning: Coincidence Processor has been removed: " << endl;
                    break;
                }
                case 45: { // set energy window for coincidence processor
                    cout << "Warning: Coincidence Processor has been removed: " << endl;
                    break;
                }
                case 46: { // set time gate of coincidence processor
                    cout << "Warning: Coincidence Processor has been removed: " << endl;
                    break;
                }
                case 47: { // set time gate of coincidence processor
                    cout << "Warning: Coincidence Processor has been removed: " << endl;
                    break;
                }
                case 48: { // gateCoincidences by ring difference
                    cout << "Warning: Coincidence Processor has been removed: " << endl;
                    break;
                }
                case 49: { // set singles isotope
                    char string[256];
                    int scanCode = sscanf(args.c_str(), "%s", string);
                    if (scanCode != 1) {
                        parseErrorOccurred = true;
                        break;
                    }
                    if (!sources.SetCurIsotope(string)) {
                        parseErrorOccurred = true;
                        break;
                    }
                    cout << "Debug: set isotope: " << string << endl;
                    break;
                }
                case 50: { // load voxel array source
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
                    if (scanCode ==8) {
                        VoxelSource * s = new
                        VoxelSource(position,dims,voxelsize,activity);
                        if (s->Load(string)) {
                            s->SetMaterial(curMaterial);
                            sources.AddSource(s);
                        }
                    } else {
                        parseErrorOccurred = true;
                        break;
                    }
                    break;
                }
                case 51: { // include a dff file into current one
                    char string[256];
                    int scanCode = sscanf(args.c_str(), "%s", string);
                    // Reference all of the include files to the directory of the
                    // top level file.
                    std::string include_filename = file_dir + std::string(string);
                    if (scanCode ==1) {
                        file_stack.emplace(include_filename.c_str());
                        if (file_stack.top()) {
                            cout << "Including File:" << include_filename << endl;
                        } else {
                            cerr << "Include File doesn't exist: "
                                 << include_filename << endl;
                            file_stack.pop();
                        }
                        file_lines_read_stack.push(0);
                        filename_stack.push(include_filename);
                    } else {
                        parseErrorOccurred = true;
                        break;
                    }
                    break;
                }
                case 52: { // increment detector id, increment before a polygon is called
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
                    break;
                }
                case 53: { // ellipse geometry
                    VectorR3 center;
                    VectorR3 axis1;
                    VectorR3 axis2;
                    double radius1;
                    double radius2;
                    double radius3;

                    int scanCode = sscanf(args.c_str(), "%lf %lf %lf %lf %lf %lf %lf %lf %lf %lf %lf %lf",
                                           &(center.x), &(center.y), &(center.z),
                                           &(axis1.x), &(axis1.y), &(axis1.z),
                                           &(axis2.x), &(axis2.y), &(axis2.z),
                                           &radius1, &radius2, &radius3);
                    if (scanCode != 12) {
                        parseErrorOccurred = true;
                        break;
                    } else {
                        ViewableEllipsoid *ve = new ViewableEllipsoid();
                        ve->SetCenter(center);
                        ve->SetAxes(axis1, axis2);
                        ve->SetRadii(radius3, radius2, radius1);
                        ve->SetMaterial(curMaterial);
                        TransformWithRigid(ve,MatrixStack.top());
                        theScene.AddViewable(ve);
                    }
                    break;
                }
                case 54: { // ellipse source
                    VectorR3 center;
                    VectorR3 axis1;
                    VectorR3 axis2;
                    double radius1;
                    double radius2;
                    double radius3;
                    double activity;

                    int scanCode = sscanf(args.c_str(), "%lf %lf %lf %lf %lf %lf %lf %lf %lf %lf %lf %lf %lf",
                                           &(center.x), &(center.y), &(center.z),
                                           &(axis1.x), &(axis1.y), &(axis1.z),
                                           &(axis2.x), &(axis2.y), &(axis2.z),
                                           &radius1, &radius2, &radius3, &activity);
                    if (scanCode != 13) {
                        parseErrorOccurred = true;
                        break;
                    } else {
                        cout << " New Ellipsoid Source " << endl;
                        cout << " New Center :: "  << (double)center.x  << "  " ;
                        cout << (double) center.y << "  " << (double) center.z <<  endl;
                        cout << " New Axis   :: "  << (double)axis1.x  << "  " ;
                        cout << (double) axis1.y << "  " << (double) axis1.z <<  endl;
                        cout << " New Radius   :: "  << (double)radius1  << "  " ;
                        cout << (double) radius2 << "  " << (double)radius3 <<  endl;
                        //CylinderSource * cyl = new CylinderSource(center, radius, axis, actScale*activity);
                        EllipsoidSource *ve = new EllipsoidSource(center, axis1, axis2, radius1, radius2, radius3, actScale*activity);
                        ve->SetMaterial(curMaterial);
                        sources.AddSource(ve);
                    }
                    break;
                }
                case 55: { // Elliptic Cylinder
                    VectorR3 center;
                    VectorR3 axis;
                    double radius1;
                    double radius2;
                    double height;
                    int scanCode = sscanf(args.c_str(), "%lf %lf %lf %lf %lf %lf %lf %lf %lf",
                                           &(center.x), &(center.y), &(center.z),
                                           &(axis.x), &(axis.y), &(axis.z),
                                           &radius1, &radius2, &height);
                    cout << "Reading Cylinder Source " <<endl;
                    if (scanCode == 9) {
                        ViewableCylinder *vc = new ViewableCylinder();
                        vc->SetCenterAxis(axis);
                        vc->SetCenter(center);
                        vc->SetRadii(radius2,radius1);
                        vc->SetHeight(height);
                        vc->SetMaterial(curMaterial);
                        TransformWithRigid(vc,MatrixStack.top());
                        theScene.AddViewable(vc);
                    } else {
                        parseErrorOccurred = true;
                        break;
                    }
                    break;
                }
                case 56: {	// Elliptic Cylinder Source
                    VectorR3 center;
                    VectorR3 axis;
                    double radius1;
                    double radius2;
                    double height;
                    double activity;
                    int scanCode = sscanf(args.c_str(), "%lf %lf %lf %lf %lf %lf %lf %lf %lf %lf",
                                           &(center.x), &(center.y), &(center.z),
                                           &(axis.x), &(axis.y), &(axis.z),
                                           &radius1, &radius2, &height, &activity);
                    cout << "Reading Elliptic Cylinder Source " <<endl;
                    if (scanCode == 10) {
                        MatrixStack.top().Transform(&center);
                        MatrixStack.top().Transform3x3(&axis);
                        axis *= height;
                        cout << " New Center :: "  << (double)center.x  << "  " ;
                        cout << (double) center.y << "  " << (double) center.z <<  endl;
                        cout << " New Axis   :: "  << (double)axis.x  << "  " ;
                        cout << (double) axis.y << "  " << (double) axis.z <<  endl;
                        EllipticCylinderSource * cyl = new EllipticCylinderSource(center, radius1, radius2, axis, actScale*activity);
                        cyl->SetMaterial(curMaterial);
                        sources.AddSource(cyl);
                    } else {
                        parseErrorOccurred = true;
                        break;
                    }
                    break;
                }
                case 57: {	// AnnulusElliptic Cylinder Source
                    VectorR3 center;
                    VectorR3 axis;
                    double radius1;
                    double radius2;
                    double height;
                    double activity;
                    int scanCode = sscanf(args.c_str(), "%lf %lf %lf %lf %lf %lf %lf %lf %lf %lf",
                                           &(center.x), &(center.y), &(center.z),
                                           &(axis.x), &(axis.y), &(axis.z),
                                           &radius1, &radius2, &height, &activity);
                    cout << "Reading Annulus Elliptic Cylinder Source " <<endl;
                    if (scanCode == 10) {
                        MatrixStack.top().Transform(&center);
                        MatrixStack.top().Transform3x3(&axis);
                        axis *= height;
                        cout << " New Center :: "  << (double)center.x  << "  " ;
                        cout << (double) center.y << "  " << (double) center.z <<  endl;
                        cout << " New Axis   :: "  << (double)axis.x  << "  " ;
                        cout << (double) axis.y << "  " << (double) axis.z <<  endl;
                        AnnulusEllipticCylinderSource * cyl = new AnnulusEllipticCylinderSource(center, radius1, radius2, axis, actScale*activity);
                        cyl->SetMaterial(curMaterial);
                        sources.AddSource(cyl);
                    } else {
                        parseErrorOccurred = true;
                        break;
                    }
                    break;
                }
                case 58: { // Annulus Cylinder Source
                    VectorR3 center;
                    VectorR3 axis;
                    double radius;
                    double height;
                    double activity;
                    int scanCode = sscanf(args.c_str(), "%lf %lf %lf %lf %lf %lf %lf %lf %lf",
                                           &(center.x), &(center.y), &(center.z),
                                           &(axis.x), &(axis.y), &(axis.z),
                                           &radius, &height, &activity);
                    cout << "Reading Annulus Cylinder Source " <<endl;
                    if (scanCode == 9) {

                        MatrixStack.top().Transform(&center);
                        MatrixStack.top().Transform3x3(&axis);
                        axis *= height;
                        cout << " New Center :: "  << (double)center.x  << "  " ;
                        cout << (double) center.y << "  " << (double) center.z <<  endl;
                        cout << " New Axis   :: "  << (double)axis.x  << "  " ;
                        cout << (double) axis.y << "  " << (double) axis.z <<  endl;
                        AnnulusCylinderSource * cyl = new AnnulusCylinderSource(center, radius, axis, actScale*activity);
                        cyl->SetMaterial(curMaterial);
                        sources.AddSource(cyl);
                    } else {
                        parseErrorOccurred = true;
                        break;
                    }
                    break;
                }
                case 59: { // Set the output format
                    Output::BinaryOutputFormat format;
                    int scanCode = sscanf(args.c_str(), "%d", &format);
                    if (scanCode == 1) {
                        Output::SetBinaryFormat(format);
                    } else {
                        parseErrorOccurred = true;
                        break;
                    }
                    break;
                }
                default: {
                    break;
                }
            }
            if (parseErrorOccurred) {
                print_parse_error(line);
                return(false);
            }
        }
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
    return(true);
}

int LoadDetector::GetCommandNumber(const std::string & cmd)
{
    for (int i = 0; i < numCommands; i++) {
        if (cmd == dffCommandList[i]) {
            return(i);
        }
    }
    return(-1); // Command not found
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

void LoadDetector::SetCameraViewInfo( CameraView& theView,
                                      const VectorR3& viewPos, const VectorR3& lookAtPos,
                                      const VectorR3& upVector, double fovy,
                                      int screenWidth, int screenHeight, double nearClipping )
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
