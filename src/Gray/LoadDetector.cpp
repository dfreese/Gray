#include "Gray/Gray/LoadDetector.h"
#include <cstdio>
#include <cstring>
#include <iostream>
#include <memory>
#include <stack>
#include <sstream>
#include <unordered_set>
#include "Gray/Gray/Config.h"
#include "Gray/Gray/GammaMaterial.h"
#include "Gray/Graphics/SceneDescription.h"
#include "Gray/Graphics/TransformViewable.h"
#include "Gray/Graphics/VisiblePoint.h"
#include "Gray/Graphics/ViewableBase.h"
#include "Gray/Graphics/ViewableParallelepiped.h"
#include "Gray/Graphics/ViewableSphere.h"
#include "Gray/Graphics/ViewableEllipsoid.h"
#include "Gray/Graphics/ViewableTriangle.h"
#include "Gray/Graphics/ViewableCylinder.h"
#include "Gray/Output/DetectorArray.h"
#include "Gray/Sources/AnnulusCylinderSource.h"
#include "Gray/Sources/AnnulusEllipticCylinderSource.h"
#include "Gray/Sources/CylinderSource.h"
#include "Gray/Sources/EllipsoidSource.h"
#include "Gray/Sources/EllipticCylinderSource.h"
#include "Gray/Sources/PointSource.h"
#include "Gray/Sources/RectSource.h"
#include "Gray/Sources/SphereSource.h"
#include "Gray/Sources/VectorSource.h"
#include "Gray/Sources/VoxelSource.h"
#include "Gray/Sources/SourceList.h"
#include "Gray/VrMath/Aabb.h"
#include "Gray/VrMath/LinearR3.h"

bool LoadDetector::LoadConfig(const std::string & filename, Config & config) {
    ifstream input(filename);
    if (!input) {
        cerr << "Unable to open file: " << filename << endl;
        return(false);
    }

    std::string file_dir = "";
    size_t dir_pos = filename.find_last_of('/');
    if (dir_pos != std::string::npos) {
        // Include everything, including slash
        file_dir = filename.substr(0, dir_pos + 1);
    }

    string line;
    int line_no = -1;
    while(getline(input, line)) {
        line_no++;
        // Ignore blank lines, including just all whitespace
        if (line.find_first_not_of(" ") == string::npos) {
            continue;
        }
        line = line.substr(line.find_first_not_of(" "));
        // Remove leading spaces, and anything after a comment
        line = line.substr(0, line.find_first_of("#"));
        // Ignore blank lines again after removing comments
        if (line.empty()) {
            continue;
        }

        stringstream line_ss(line);
        string command;
        if ((line_ss >> command).fail()) {
            cerr << "Unable to parse command \"" << line
                 << "\" on line " << line_no << " of " << filename << endl;
            return(false);
        }

        string args = ScanForSecondField(line);
        if (!HandleConfigCommand(command, args, file_dir, config)) {
            cerr << "Unable to parse command \"" << line
            << "\" on line " << line_no << " of " << filename << endl;
            return(false);
        }
    }
    return(true);
}


bool LoadDetector::HandleConfigCommand(const std::string & command,
                                       const std::string & args,
                                       const std::string & file_dir,
                                       Config & config)
{
    stringstream line_ss(args);
    if (command == "hits_format") {
        if (!config.set_format_hits(args)) {
            cerr << "Invalid format identifier: " << args << endl;
            return(false);
        }
    } else if (command == "singles_format") {
        if (!config.set_format_singles(args)) {
            cerr << "Invalid format identifier: " << args << endl;
            return(false);
        }
    } else if (command == "coinc_format") {
        if (!config.set_format_coinc(args)) {
            cerr << "Invalid format identifier: " << args
            << endl;
            return(false);
        }
    } else if (command == "hits_output") {
        config.set_filename_hits(args);
    } else if (command == "singles_output") {
        config.set_filename_singles(args);
    } else if (command == "coinc_output") {
        config.add_filename_coinc(args);
    } else if (command == "process_file") {
        std::string filename = file_dir + args;
        config.set_filename_process(filename);
    }  else if (command == "process") {
        // Any line prefaced with pipeilne_config will be processed as a
        // pipeline file.
        config.add_process_line(args);
    } else if (command == "mapping_file") {
        std::string filename = file_dir + args;
        config.set_filename_mapping(filename);
    } else if (command == "hits_var_mask") {
        if (!config.set_hits_var_output_write_flags(args)) {
            cerr << "Unable to parse the variable mask given" << endl;
            return(false);
        }
    } else if (command == "singles_var_mask") {
        if (!config.set_singles_var_output_write_flags(args)) {
            cerr << "Unable to parse the variable mask given" << endl;
            return(false);
        }
    } else if (command == "coinc_var_mask") {
        if (!config.set_coinc_var_output_write_flags(args)) {
            cerr << "Unable to parse the variable mask given" << endl;
            return(false);
        }
    } else if (command == "time") {
        // simulation time in seconds
        double sim_time;
        if ((line_ss >> sim_time).fail()) {
            cerr << "Invalid start time" << args << endl;
            return(false);
        }
        config.set_time(sim_time);
    } else if (command == "start_time") {
        // simulation time in seconds
        double start_time;
        if ((line_ss >> start_time).fail()) {
            cerr << "Invalid start time" << args << endl;
            return(false);
        }
        config.set_start_time(start_time);
    } else if (command == "seed") {
        unsigned long seed;
        if ((line_ss >> seed).fail()) {
            cerr << "Invalid start time" << args << endl;
            return(false);
        }
        config.set_seed(seed);
    } else if (command == "log_positron") {
        config.set_log_nuclear_decays(true);
    } else if (command == "log_nondepositing") {
        config.set_log_nondepositing_inter(true);
    } else if (command == "log_nonsensitive") {
        config.set_log_nonsensitive(true);
    } else if (command == "log_errors") {
        config.set_log_errors(true);
    } else if (command == "log_all") {
        config.set_log_all(true);
    }
    return(true);
}

bool LoadDetector::Load(const std::string & filename,
                        SceneDescription & theScene,
                        SourceList & sources, Config & config,
                        DetectorArray & detector_array)
{
    double polygonScale = 1.0;
    double actScale = 1.0;
    int block_id = 0;

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

    // Add state variable for polygons, so that it's appropriately handled
    // during repeats.
    bool looking_for_polygon_lines = false;
    int no_polygon_lines_needed = 0;
    vector<string> polygon_lines;

    bool view_pos_set = false;
    VectorR3 viewPos;
    bool lookat_pos_set = false;
    VectorR3 lookAtPos(0, 0, 0);
    VectorR3 upVector(0, 1, 0);
    double fovy = 35 * M_PI / 180.0; // Field of view angle (in radians)
    int screenWidth = 0;
    int screenHeight = 0;
    double hither = -100;

    theScene.SetBackGroundColor(1.0, 1.0, 1.0);

    // Vectorial Source parsing
    bool parse_VectorSource = false;
    double vector_source_activity = -1;
    unique_ptr<SceneDescription> vector_source_scene;

    GammaMaterial * curMaterial = static_cast<GammaMaterial *>(
            &theScene.GetDefaultMaterial());

    struct RepeatInfo {
        enum RepeatType {
            basic,
            rotate,
            grid
        };
        RepeatType type;
        int no_repeats;
        int no_complete;
        size_t start_idx;
        string starting_file;
        size_t starting_line;
        VectorR3 grid_no_complete;
        VectorR3 grid_repeats;
        VectorR3 grid_step;
        double rotate_angle;
        VectorR3 rotate_axis;
    };
    stack<RepeatInfo> repeat_info_stack;
    vector<string> repeat_buffer;
    size_t current_idx = 0;
    // Stores the detector id number for sets of polygons.  This will be used if
    // the material is sensitive and if "increment" is called.
    int polygon_det_id = -1;


    unordered_set<string> config_commands = {"hits_format", "singles_format",
        "coinc_format", "hits_output", "singles_output", "coinc_output",
        "process_file", "process", "mapping_file", "hits_var_mask",
        "singles_var_mask", "coinc_var_mask", "time", "start_time", "seed",
        "log_positron", "log_nonsensitive", "log_errors",
        "log_all", "log_nondepositing"};

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
        line = line.substr(line.find_first_not_of(" "));
        // Remove leading spaces, and anything after a comment
        line = line.substr(0, line.find_first_of("#"));
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

        // Privileged commands that won't be skipped if we're still waiting for
        // the end of a repeat.
        if (command == "end_repeat") {
            if (repeat_info_stack.empty()) {
                print_parse_error(line);
                cerr << "Found unpaired end_repeat" << endl;
                return(false);
            }
            RepeatInfo & info = repeat_info_stack.top();
            info.no_complete++;
            if (info.no_complete <= 0) {
                // Initialize
                info.no_complete = 0;
                current_idx = info.start_idx;
                // push the current matrix before repeating as some repeat
                // commands will modify the matrix.
                MatrixStack.push(MatrixStack.top());
                switch (info.type) {
                    case RepeatInfo::rotate: {
                        break;
                    }
                    case RepeatInfo::grid: {
                        VectorR3 init_step = info.grid_step * -0.5;
                        init_step.x *= info.grid_repeats.x - 1;
                        init_step.y *= info.grid_repeats.y - 1;
                        init_step.z *= info.grid_repeats.z - 1;
                        MatrixStack.top().Transform3x3(&init_step);
                        MatrixStack.top().ApplyTranslationLeft(init_step);
                        break;
                    }
                    case RepeatInfo::basic: {
                        break;
                    }
                }
                // push the current matrix again before repeating to protect
                // against translations inside of the repeat.
                MatrixStack.push(MatrixStack.top());
            } else if (info.no_complete < info.no_repeats) {
                // Process
                current_idx = info.start_idx;
                // pop off the internal protection matrix
                if (MatrixStack.empty() || (MatrixStack.size() == 1)) {
                    print_parse_error(line);
                    cerr << "Probably an unpaired push command in repeat\n"
                    << "Repeats imply a push command" << endl;
                    return(false);
                }
                MatrixStack.pop();

                switch (info.type) {
                    case RepeatInfo::rotate: {
                        MatrixStack.top() *= VrRotate(info.rotate_angle * (M_PI/180.0),
                                                      info.rotate_axis);
                        break;
                    }
                    case RepeatInfo::grid: {
                        VectorR3 x_step(info.grid_step.x, 0, 0);
                        VectorR3 y_step(0, info.grid_step.y, 0);
                        VectorR3 z_step(0, 0, info.grid_step.z);
                        MatrixStack.top().Transform3x3(&x_step);
                        MatrixStack.top().Transform3x3(&y_step);
                        MatrixStack.top().Transform3x3(&z_step);
                        MatrixStack.top().ApplyTranslationLeft(x_step);
                        info.grid_no_complete.x++;
                        if (info.grid_no_complete.x >= info.grid_repeats.x) {
                            // Reset x, step y
                            MatrixStack.top().ApplyTranslationLeft(x_step * -1 * info.grid_no_complete.x);
                            info.grid_no_complete.x = 0;
                            MatrixStack.top().ApplyTranslationLeft(y_step);
                            info.grid_no_complete.y++;
                        }
                        if (info.grid_no_complete.y >= info.grid_repeats.y) {
                            // Reset y, step z
                            MatrixStack.top().ApplyTranslationLeft(y_step * -1 * info.grid_no_complete.y);
                            info.grid_no_complete.y = 0;
                            MatrixStack.top().ApplyTranslationLeft(z_step);
                            info.grid_no_complete.z++;
                        }
                        break;
                    }
                    case RepeatInfo::basic: {
                        break;
                    }
                }

                // push the current matrix again before repeating to protect
                // against translations inside of the repeat.
                MatrixStack.push(MatrixStack.top());
            } else {
                // Cleanup
                // We should have at least
                if (MatrixStack.size() < 3) {
                    print_parse_error(line);
                    cerr << "Probably an unpaired push command in repeat\n"
                    << "Repeats imply a push command" << endl;
                    return(false);
                }
                MatrixStack.pop();
                MatrixStack.pop();
                repeat_info_stack.pop();
                if (repeat_info_stack.empty()) {
                    repeat_buffer.clear();
                    current_idx = 0;
                }
            }
            continue;
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
            continue;
        }

        // skip the remainder of the commands
        if (!repeat_info_stack.empty()) {
            if (repeat_info_stack.top().no_complete < 0) {
                continue;
            }
        }

        // Unprivileged commands
        if (command == "begin_repeat") {
            string repeat_type;
            if ((line_ss >> repeat_type).fail()) {
                print_parse_error(line);
                cerr << "Unable to parse repeat type" << endl;
                return(false);
            }
            if (repeat_type == "rotate") {
                bool fail = false;
                int no_repeats;
                double angle;
                VectorR3 axis;
                fail |= (line_ss >> no_repeats).fail();
                fail |= (line_ss >> angle).fail();
                fail |= (line_ss >> axis.x).fail();
                fail |= (line_ss >> axis.y).fail();
                fail |= (line_ss >> axis.z).fail();
                if (fail) {
                    print_parse_error(line);
                    cerr << "Unable to parse grid repeats\n"
                    << "Format: [steps] [size deg] [axis x y z]" << endl;
                    return(false);
                }

                repeat_info_stack.push(RepeatInfo());
                repeat_info_stack.top().type = RepeatInfo::rotate;
                repeat_info_stack.top().no_repeats = no_repeats;
                repeat_info_stack.top().rotate_axis = axis;
                repeat_info_stack.top().rotate_angle = angle;
            } else if (repeat_type == "grid") {
                bool fail = false;
                VectorR3 repeats_xyz;
                VectorR3 step_xyz;
                fail |= (line_ss >> repeats_xyz.x).fail();
                fail |= (line_ss >> repeats_xyz.y).fail();
                fail |= (line_ss >> repeats_xyz.z).fail();
                fail |= (line_ss >> step_xyz.x).fail();
                fail |= (line_ss >> step_xyz.y).fail();
                fail |= (line_ss >> step_xyz.z).fail();
                if (fail) {
                    print_parse_error(line);
                    cerr << "Unable to parse grid repeats\n"
                         << "Format: [steps x y z] [stride x y z]" << endl;
                    return(false);
                }

                if ((repeats_xyz.x <= 0) || (repeats_xyz.y <= 0) ||
                    (repeats_xyz.z <= 0))
                {
                    print_parse_error(line);
                    cerr << "Number of steps must be 1 or greater\n" << endl;
                    return(false);
                }

                repeat_info_stack.push(RepeatInfo());
                repeat_info_stack.top().type = RepeatInfo::grid;
                repeat_info_stack.top().no_repeats = (repeats_xyz.x *
                                                      repeats_xyz.y *
                                                      repeats_xyz.z);

                repeat_info_stack.top().type = RepeatInfo::grid;
                repeat_info_stack.top().grid_repeats = repeats_xyz;
                repeat_info_stack.top().grid_step = step_xyz;
                repeat_info_stack.top().grid_no_complete = VectorR3(0, 0, 0);
            } else {
                stringstream type_ss(repeat_type);
                int no_repeats;
                if ((type_ss >> no_repeats).fail()) {
                    print_parse_error(line);
                    cerr << "Unable to parse number of repeats" << endl;
                    return(false);
                }
                repeat_info_stack.push(RepeatInfo());
                repeat_info_stack.top().type = RepeatInfo::basic;
                repeat_info_stack.top().no_repeats = no_repeats;
            }
            // Need to read all lines first
            repeat_info_stack.top().no_complete = -1;
            repeat_info_stack.top().starting_file = filename_stack.top();
            repeat_info_stack.top().starting_line = file_lines_read_stack.top();
            // The first line will be the next line put into the buffer
            repeat_info_stack.top().start_idx = current_idx;
        } else if (command == "echo") {
            cout << "echo: " << args << endl;
        } else if (command == "isotope") {
            if (!sources.SetCurIsotope(args, MatrixStack.top())) {
                print_parse_error(line);
                cerr << "invalid isotope: " << args << endl;
                return(false);
            }
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
            std::unique_ptr<VoxelSource> s(new VoxelSource(position, dims,
                                                           voxelsize,
                                                           actScale * activity));
            if (s->Load(string)) {
                sources.AddSource(std::move(s));
            } else {
                print_parse_error(line);
                cerr << "Unable to load voxelized source: " << string << endl;
                return(false);
            }
        } else if (command == "increment") {
            polygon_det_id = IncrementDetector(MatrixStack.top(), detector_array);
        } else if (command == "ellipsoid") {
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
            std::unique_ptr<ViewableEllipsoid> ve(new ViewableEllipsoid());
            ve->SetCenter(center);
            ve->SetAxes(axis1, axis2);
            ve->SetRadii(radius3, radius2, radius1);
            ve->SetMaterial(curMaterial);
            TransformWithRigid(ve.get(), MatrixStack.top());
            theScene.AddViewable(std::move(ve));
        } else if (command == "ellipsoid_src") {
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
            std::unique_ptr<EllipsoidSource> ve(new EllipsoidSource(
                    center, axis1, axis2, radius1, radius2, radius3,
                    actScale*activity));
            sources.AddSource(std::move(ve));
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
            if (scanCode != 9) {
                print_parse_error(line);
                return(false);
            }
            std::unique_ptr<ViewableCylinder> vc(new ViewableCylinder());
            vc->SetCenterAxis(axis);
            vc->SetCenter(center);
            vc->SetRadii(radius2,radius1);
            vc->SetHeight(height);
            vc->SetMaterial(curMaterial);
            TransformWithRigid(vc.get(),MatrixStack.top());
            theScene.AddViewable(std::move(vc));
        } else if (command == "elliptic_cyl_src") {
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
            if (scanCode != 10) {
                print_parse_error(line);
                return(false);
            }
            MatrixStack.top().Transform(&center);
            MatrixStack.top().Transform3x3(&axis);
            axis *= height;
            std::unique_ptr<EllipticCylinderSource> cyl(new EllipticCylinderSource(
                    center, radius1, radius2, axis, actScale*activity));
            sources.AddSource(std::move(cyl));
        } else if (command == "annulus_ell_src") {
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
            if (scanCode != 10) {
                print_parse_error(line);
                return(false);
            }
            MatrixStack.top().Transform(&center);
            MatrixStack.top().Transform3x3(&axis);
            axis *= height;
            std::unique_ptr<AnnulusEllipticCylinderSource> cyl(
                    new AnnulusEllipticCylinderSource(center, radius1, radius2,
                                                      axis, actScale*activity));
            sources.AddSource(std::move(cyl));
        } else if (command == "annulus_cyl_src") {
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
            if (scanCode != 9) {
                print_parse_error(line);
                return(false);
            }

            MatrixStack.top().Transform(&center);
            MatrixStack.top().Transform3x3(&axis);
            std::unique_ptr<AnnulusCylinderSource> cyl(new AnnulusCylinderSource(
                    center, radius, height, axis, actScale*activity));
            sources.AddSource(std::move(cyl));
        } else if (command == "scale_act") {
            double t_actScale = -1.0;
            int scanCode = sscanf(args.c_str(), "%lf", &t_actScale);
            if (scanCode != 1) {
                print_parse_error(line);
                return(false);
            }
            actScale = t_actScale;
            cout << "scale act:" << actScale << "\n";
        } else if (command == "disable_half_life") {
            sources.SetSimulateIsotopeHalfLife(false);
        } else if (command == "sphere") {
            // Sphere object
            VectorR3 position;
            double radius = -1.0;
            int scanCode = sscanf(args.c_str(), "%lf %lf %lf %lf",
                                  &(position.x), &(position.y), &(position.z),
                                  &radius);
            if (scanCode != 4) {
                print_parse_error(line);
                return(false);
            }
            std::unique_ptr<ViewableSphere> s(new ViewableSphere(position, radius));
            s->SetMaterial(curMaterial);
            TransformWithRigid(s.get(),MatrixStack.top());
            theScene.AddViewable(std::move(s));
        } else if (command == "pt_src") {
            // pt_src pos_x pos_y pos_z activity
            VectorR3 position;
            double activity;
            std::stringstream ss(args);
            ss >> position.x;
            ss >> position.y;
            ss >> position.z;
            ss >> activity;
            if (ss.fail()) {
                print_parse_error(line);
                return(false);
            }
            MatrixStack.top().Transform(&position);
            std::unique_ptr<PointSource> s(new PointSource(
                    position, actScale*activity));
            sources.AddSource(std::move(s));
        } else if (command == "start_vecsrc") {
            double activity = -1.0;
            int scanCode = sscanf(args.c_str(), "%lf", &activity);
            if (scanCode != 1) {
                print_parse_error(line);
                return(false);
            }
            cout << "Starting Vector Source\n";
            parse_VectorSource = true;
            vector_source_activity = activity;
            vector_source_scene = unique_ptr<SceneDescription>(new SceneDescription());
        } else if (command == "end_vecsrc") {
            sources.AddSource(std::unique_ptr<Source>(new VectorSource(actScale * vector_source_activity, std::move(vector_source_scene))));
            parse_VectorSource = false;
        } else if (command == "v") {
            // Deprecated, and generic defaults added
        } else if (command == "scale") {
            double t_polygonScale = -1.0;
            int scanCode = sscanf(args.c_str(), "%lf", &t_polygonScale);
            if (scanCode != 1) {
                print_parse_error(line);
                return(false);
            }
            polygonScale = t_polygonScale;
        } else if (command == "sp_src") {
            // Sphere source
            VectorR3 position;
            double radius = -1.0;
            double activity = -1.0;
            int scanCode = sscanf(args.c_str(), "%lf %lf %lf %lf %lf",
                                  &(position.x), &(position.y), &(position.z),
                                  &radius, &activity);
            if (scanCode != 5) {
                print_parse_error(line);
                return(false);
            }

            MatrixStack.top().Transform(&position);
            std::unique_ptr<SphereSource> s(new SphereSource(position, radius,
                                                             actScale*activity));
            sources.AddSource(std::move(s));
        } else if (command == "rect_src") {
            // rectangular source
            VectorR3 baseCenter;
            VectorR3 baseSize;
            // Always assume the rect_src is axis aligned for now.
            VectorR3 orientation(0, 0, 1);
            double activity = -1.0;
            int scanCode = sscanf(args.c_str(), "%lf %lf %lf %lf %lf %lf %lf",
                              &baseCenter.x, &baseCenter.y, &baseCenter.z,
                              &baseSize.x, &baseSize.y, &baseSize.z, &activity);
            if (scanCode != 7) {
                print_parse_error(line);
                return(false);
            }
            MatrixStack.top().Transform(&baseCenter);
            MatrixStack.top().Transform(&orientation);
            std::unique_ptr<RectSource> s(new RectSource(baseCenter, baseSize,
                                                         orientation,
                                                         actScale*activity));
            sources.AddSource(std::move(s));
        } else if (command == "array") {
            // repeat detector in 3d
            VectorR3 StartPos;
            VectorR3 UnitStep; // center to center between repeated cell
            VectorR3 UnitSize; // detector size
            int num_x = -1;
            int num_y = -1;
            int num_z = -1;
            int scanCode = sscanf(args.c_str(),
                                  "%lf %lf %lf %d %d %d %lf %lf %lf %lf %lf %lf",
                                  &(StartPos.x), &(StartPos.y), &(StartPos.z),
                                  &num_x, &num_y, &num_z,
                                  &(UnitStep.x), &(UnitStep.y), &(UnitStep.z),
                                  &(UnitSize.x), &(UnitSize.y), &(UnitSize.z));
            if (scanCode != 12) {
                print_parse_error(line);
                cerr << "unable to process array."
                     << "  Format:  [center xyz] [repeats xyz] [pitch xyz]"
                     << " [size xyz]" << endl;
                return(false);
            }
            VectorR3 CurrentPos;

            UnitStep *= polygonScale;
            UnitSize *= polygonScale;

            StartPos.x -= (double)(num_x-1) * UnitStep.x / 2.0;
            StartPos.y -= (double)(num_y-1) * UnitStep.y / 2.0;
            StartPos.z -= (double)(num_z-1) * UnitStep.z / 2.0;
            for (int k = 0; k < num_z; k++) {
                for (int j = 0; j < num_y; j++) {
                    for (int i = 0; i < num_x; i++) {
                        CurrentPos= StartPos;
                        CurrentPos.x += (double)i * UnitStep.x;
                        CurrentPos.y += (double)j * UnitStep.y;
                        CurrentPos.z += (double)k * UnitStep.z;
                        if (curMaterial->IsSensitive()) {
                            int det_id = detector_array.AddDetector(
                                    CurrentPos, UnitSize, MatrixStack.top(),
                                    i, j, k, block_id);
                            ProcessDetector(CurrentPos, UnitSize,
                                            curMaterial, det_id,
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
            if (curMaterial->IsSensitive()) {
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
            std::unique_ptr<ViewableCylinder> vc(new ViewableCylinder());
            vc->SetRadius(radius);
            vc->SetCenterAxis(axis);
            vc->SetCenter(center);
            vc->SetHeight(height);
            vc->SetMaterial(curMaterial);
            TransformWithRigid(vc.get(), MatrixStack.top());
            theScene.AddViewable(std::move(vc));
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
            if (scanCode != 9) {
                print_parse_error(line);
                return(false);
            }

            MatrixStack.top().Transform(&center);
            MatrixStack.top().Transform3x3(&axis);
            std::unique_ptr<Source> cyl(new CylinderSource(
                        center, radius, height, axis, actScale*activity));
            sources.AddSource(std::move(cyl));
        } else if (command == "ann_cyl") {
            VectorR3 center;
            VectorR3 axis;
            double inner_radius;
            double outer_radius;
            double height;
            int scanCode = sscanf(args.c_str(),
                                  "%lf %lf %lf %lf %lf %lf %lf %lf %lf",
                                  &(center.x), &(center.y), &(center.z),
                                  &(axis.x), &(axis.y), &(axis.z),
                                  &inner_radius, &outer_radius, &height);
            if (scanCode != 9) {
                print_parse_error(line);
                return(false);
            }
            int det_id = -1;
            if (curMaterial->IsSensitive()) {
                det_id = IncrementDetector(MatrixStack.top(), detector_array);
            }
            auto pieces = MakeAnnulusCylinder(center, axis, inner_radius,
                                              outer_radius, height, det_id,
                                              curMaterial);
            for (const auto & triangle: pieces) {
                std::unique_ptr<ViewableTriangle> vc(new ViewableTriangle(triangle));
                TransformWithRigid(vc.get(), MatrixStack.top());
                theScene.AddViewable(std::move(vc));
            }
        } else if (command == "l") {
            // light
            VectorR3 lightPos, lightColor;
            int scanCode = sscanf(args.c_str(), "%lf %lf %lf %lf %lf %lf",
                                  &(lightPos.x), &(lightPos.y), &(lightPos.z),
                                  &(lightColor.x), &(lightColor.y),
                                  &(lightColor.z));
            if ((scanCode != 3) && (scanCode != 6)) {
                print_parse_error(line);
                return(false);
            }
            std::unique_ptr<Light> aLight(new Light());
            aLight->SetPosition(lightPos);
            if (scanCode == 6) {
                aLight->SetColor(lightColor);
            }
            theScene.AddLight(std::move(aLight));
        } else if (command == "t") {
            // translate
            VectorR3 trans;
            int scanCode = sscanf(args.c_str(), "%lf %lf %lf",
                                  &(trans.x), &(trans.y), &(trans.z));
            if (scanCode != 3) {
                print_parse_error(line);
                return(false);
            }

            trans *= polygonScale;
            MatrixStack.top().Transform3x3(&trans);
            MatrixStack.top().ApplyTranslationLeft(trans);
        } else if (command == "push") {
            // push matrix
            // Add a copy of the current matrix to the top of the stack
            MatrixStack.push(MatrixStack.top());
        } else if (command == "pop") {
            // The default matrix is placed at the bottom of the stack.
            // We should never pop that off.
            if (MatrixStack.empty() || (MatrixStack.size() == 1)) {
                print_parse_error(line);
                cerr << "Unpaired pop command found" << endl;
                return(false);
            }
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
            const double theta = degree * (M_PI / 180.0);
            MatrixStack.top() *= VrRotate(theta, axis);
        } else if (command == "from") {
            int scanCode = sscanf(args.c_str(), "%lf %lf %lf",
                                  &(viewPos.x), &(viewPos.y), &(viewPos.z));
            if (scanCode != 3) {
                print_parse_error(line);
                return(false);
            }
            view_pos_set = true;
        } else if (command == "at") {
            int scanCode = sscanf(args.c_str(), "%lf %lf %lf", &(lookAtPos.x),
                                  &(lookAtPos.y), &(lookAtPos.z));
            if (scanCode != 3) {
                print_parse_error(line);
                return(false);
            }
            lookat_pos_set = true;
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
            looking_for_polygon_lines = true;
            no_polygon_lines_needed = numVerts;
            polygon_lines.clear();
        } else if (command == "m") {
            if (!theScene.HasMaterial(args)) {
                print_parse_error(line);
                cerr << "Invalid material: " << args << endl;
                return(false);
            }
            curMaterial = static_cast<GammaMaterial*>(&theScene.GetMaterial(args));
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
            if (curMaterial->IsSensitive()) {
                int det_id = detector_array.AddDetector(baseCenter,
                                                       baseSize,
                                                       MatrixStack.top(),
                                                       0, 0, 0,
                                                       block_id);
                block_id++;
                ProcessDetector(baseCenter, baseSize, curMaterial,
                                det_id, theScene, MatrixStack.top());
            } else {
                ProcessDetector(baseCenter, baseSize, curMaterial, -1,
                                theScene, MatrixStack.top());
            }
        } else if (command == "disable_rayleigh") {
            DisableRayleigh(theScene);
        } else if (config_commands.count(command)) {
            // Handle all of the commands that deal with the config class in
            // the same function so that it can be called independently.
            if (!HandleConfigCommand(command, args, file_dir, config)) {
                print_parse_error(line);
            }
            // FIXME: warning doesn't work if include is in repeat.
            if (file_stack.size() > 1) {
                cout << "Warning: configuration commands, like \"" << command
                     << "\" should be in the top level file.\n"
                     << "They will be ignored by gray-daq otherwise" << endl;
            }
        } else if (looking_for_polygon_lines) {
            polygon_lines.push_back(line);
            if (no_polygon_lines_needed == polygon_lines.size()) {
                looking_for_polygon_lines = false;
                // arbitrary triangles must use increment to advance detector ids
                // detector only is used when material is sensitive
                int polygon_det_id_if_sensitive = -1;
                if (curMaterial->IsSensitive()) {
                    polygon_det_id_if_sensitive = polygon_det_id;
                }
                // Choose if we're adding this to the vector source's scene or
                // the geometric scene.
                SceneDescription & local_scene = (parse_VectorSource ?
                        theScene:(*vector_source_scene.get()));
                ProcessFaceDFF(no_polygon_lines_needed, curMaterial,
                               polygon_lines, parse_VectorSource,
                               polygon_det_id_if_sensitive, local_scene,
                               polygonScale, MatrixStack.top());
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

    if (parse_VectorSource) {
        cerr << "unpaired start_vecsrc command" << endl;
        return(false);
    }
    if (!lookat_pos_set) {
        AABB extents = theScene.GetExtents();
        lookAtPos.x = (extents.GetMaxX() + extents.GetMinX()) / 2.0;
        lookAtPos.y = (extents.GetMaxY() + extents.GetMinY()) / 2.0;
        lookAtPos.z = (extents.GetMaxZ() + extents.GetMinZ()) / 2.0;
    }
    if (!view_pos_set) {
        AABB extents = theScene.GetExtents();
        double maxdim = max(extents.GetMaxX() - extents.GetMinX(),
                            extents.GetMaxY() - extents.GetMinY());
        viewPos = lookAtPos;
        viewPos.z += lookAtPos.z + 1.2 * (0.5 *  maxdim / atan(fovy * 0.5 ));
    }
    if (theScene.NumLights() == 0) {
        theScene.SetGlobalAmbientLight(3.5, 3.5, 3.5);
    }
    SetCameraViewInfo(theScene.GetCameraView(),
                      viewPos, lookAtPos, upVector, fovy,
                      screenWidth, screenHeight, hither);
    return(true);
}

void LoadDetector::ProcessDetector(const VectorR3 & detCenter,
                                   const VectorR3 & detSize,
                                   const Material* curMaterial,
                                   int id,
                                   SceneDescription & scene,
                                   const RigidMapR3 & current_matrix)
{
    std::unique_ptr<ViewableParallelepiped> vp(new ViewableParallelepiped());
    vp->SetMaterialFront(curMaterial);
    vp->SetMaterialBack(curMaterial);
    vp->SetDetectorId(id);

    VectorR3 va, vb, vc, vd;
    va = vb = vc = vd = detCenter;
    VectorR3 half_sz = detSize / 2.0;

    va.x -= half_sz.x;
    va.y -= half_sz.y;
    va.z -= half_sz.z;

    vb.x -= half_sz.x;
    vb.y += half_sz.y;
    vb.z -= half_sz.z;

    vc.x -= half_sz.x;
    vc.y -= half_sz.y;
    vc.z += half_sz.z;

    vd.x += half_sz.x;
    vd.y -= half_sz.y;
    vd.z -= half_sz.z;

    vp->SetVertices(va, vb, vc, vd);
    TransformWithRigid(vp.get(), current_matrix);
    scene.AddViewable(std::move(vp));
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
                                  const std::vector<std::string> & lines,
                                  bool parse_VectorSource,
                                  int det_id,
                                  SceneDescription & scene,
                                  double polygonScale,
                                  const RigidMapR3 & current_matrix)
{
    VectorR3 firstVert, prevVert, thisVert;
    if ( !ReadVertexR3(firstVert, lines[0]) ) {
        return false;
    }
    if ( !ReadVertexR3(prevVert, lines[1]) ) {
        return false;
    }
    firstVert *= polygonScale;
    prevVert *= polygonScale;
    for (int i = 2; i < numVerts; i++) {
        if ( !ReadVertexR3(thisVert, lines[i]) ) {
            return false;
        }

        thisVert *= polygonScale;

        std::unique_ptr<ViewableTriangle> vt(new ViewableTriangle());
        vt->SetDetectorId(det_id);
        vt->Init( firstVert, prevVert, thisVert );
        vt->SetMaterialFront( curMaterial );
        vt->SetMaterialBack( curMaterial );
        TransformWithRigid(vt.get(), current_matrix);

        if (parse_VectorSource) {
            vt->SetSrcId(1);
        } else {
            vt->SetSrcId(0);
        }
        scene.AddViewable(std::move(vt));
        prevVert = thisVert;
    }
    return true;
}

bool LoadDetector::ReadVertexR3(VectorR3& vert, const std::string & line) {
    stringstream ss(line);
    ss >> vert.x;
    ss >> vert.y;
    ss >> vert.z;
    return (!ss.fail());
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

/*!
 * Creates an annulus cylinder at a given point and orientation by constructing
 * an annulus at the origin, oriented along the z axis, using
 * MakeAnnulusCylinder and rotating and transforming it to using
 * RefAxisPlusTransToMap.
 */
std::vector<ViewableTriangle> LoadDetector::MakeAnnulusCylinder(
        const VectorR3 & center, const VectorR3 & axis,
        double radius_inner, double radius_outer, double width,
        int det_id, Material * material)
{
    RigidMapR3 transform = RefAxisPlusTransToMap(axis, center);
    auto pieces = MakeAnnulusCylinder(radius_inner, radius_outer, width);
    for (auto & triangle: pieces) {
        TransformWithRigid(&triangle, transform);
        triangle.SetDetectorId(det_id);
        triangle.SetMaterial(material);
    }
    return (pieces);
}

/*!
 * Construct a set of triangles that mimics an annulus cylinder by rotating a
 * set of four points with the appropriate width, translated in y, and rotating
 * them a given angle around the z axis.  Enforces that points to a double
 * precision are used exactly for each rotated set of points.
 *
 * Triangles are then constructed between the rotated points and the previous
 * set of four points to make four faces of points that enclose one section of
 * the cylindrical annulus.  Current two triangles are constructed per face,
 * with four faces per section, and 100 sections used for the full annulus,
 * regardless of size or shape
 *
 * \param radius_inner
 * \param radius_outer
 * \param width
 * \returns vector of 800 ViewableTriangles that create the annulus.
 */
std::vector<ViewableTriangle> LoadDetector::MakeAnnulusCylinder(
        double radius_inner, double radius_outer, double width)
{
    constexpr int no_sections = 100;
    constexpr double theta_step = M_PI * (2.0 / no_sections);
    std::vector<ViewableTriangle> pieces;
    const double pos_z = width / 2;
    const double neg_z = width / -2;

    for (int idx = 0; idx < no_sections; ++idx) {
        // rotate around the z axis to the {idx}th step of {no_sections}
        const double theta_curr = idx * theta_step;
        const double theta_next = ((idx + 1) % no_sections) * theta_step;
        const double x_curr = std::cos(theta_curr);
        const double y_curr = std::sin(theta_curr);
        const double x_next = std::cos(theta_next);
        const double y_next = std::sin(theta_next);

        const VectorR3 pos_out(radius_outer * x_curr, radius_outer * y_curr, pos_z);
        const VectorR3 neg_out(radius_outer * x_curr, radius_outer * y_curr, neg_z);
        const VectorR3 pos_in(radius_inner * x_curr, radius_inner * y_curr, pos_z);
        const VectorR3 neg_in(radius_inner * x_curr, radius_inner * y_curr, neg_z);

        const VectorR3 next_pos_out(radius_outer * x_next, radius_outer * y_next, pos_z);
        const VectorR3 next_neg_out(radius_outer * x_next, radius_outer * y_next, neg_z);
        const VectorR3 next_pos_in(radius_inner * x_next, radius_inner * y_next, pos_z);
        const VectorR3 next_neg_in(radius_inner * x_next, radius_inner * y_next, neg_z);


        // Create a all four faces, with two triangles each.
        // Top Outside
        pieces.push_back({neg_out, next_neg_out, next_pos_out});
        pieces.push_back({next_pos_out, pos_out, neg_out});
        // Positive Side
        pieces.push_back({pos_out, next_pos_out, next_pos_in});
        pieces.push_back({next_pos_in, pos_in, pos_out});
        // Bottom Inside
        pieces.push_back({pos_in, next_pos_in, next_neg_in});
        pieces.push_back({next_neg_in, neg_in, pos_in});
        // Negative Side
        pieces.push_back({neg_in, next_neg_in, next_neg_out});
        pieces.push_back({next_neg_out, neg_out, neg_in});
    }
    return (pieces);
}

int LoadDetector::IncrementDetector(const RigidMapR3 & current_matrix,
                                     DetectorArray & detector_array)
{
    VectorR3 StartPos;
    VectorR3 UnitSize;
    StartPos.x = 0.0;
    StartPos.y = 0.0;
    StartPos.z = 0.0;
    UnitSize.x = 1.0;
    UnitSize.y = 1.0;
    UnitSize.z = 1.0;

    return (detector_array.AddDetector(StartPos, UnitSize, current_matrix,
                                       0, 0, 0, 0));
}

void LoadDetector::DisableRayleigh(SceneDescription& scene) {
    for (size_t idx = 0; idx < scene.NumMaterials(); ++idx) {
        GammaMaterial& stats = static_cast<GammaMaterial&>(scene.GetMaterial(idx));
        stats.DisableRayleigh();
    }
}
