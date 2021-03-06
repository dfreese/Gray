#include <iostream>
#include "Gray/Graphics/SceneDescription.h"
#include "Gray/Gray/Config.h"
#include "Gray/Gray/Load.h"
#include "Gray/Gray/LoadMaterials.h"
#include "Gray/Gray/Simulation.h"
#include "Gray/Output/DetectorArray.h"
#include "Gray/Sources/SourceList.h"
#include "Gray/Viewer/Viewer.h"

using namespace std;

int main(int argc, char ** argv)
{
    Config config;
    int config_status = config.ProcessCommandLine(argc, argv, true);
    if (config_status < 0) {
        return(1);
    } else if (config_status > 0) {
        return(0);
    }
    DetectorArray detector_array;
    SceneDescription scene;
    SourceList sources;
    if (!sources.LoadIsotopes(config.get_physics_filename())) {
        cerr << "Unable to load physics file: \""
        << config.get_physics_filename() << "\"\n"
        << "Check GRAY_INCLUDE env variable or specify name with --phys"
        << endl;
        return(1);
    }
    if (!LoadMaterials::LoadPhysicsJson(scene, config.get_physics_filename()))
    {
        cerr << "Unable to load physics file: \""
        << config.get_physics_filename() << "\"\n"
        << "Check GRAY_INCLUDE env variable or specify name with --phys"
        << endl;
        return(1);
    }
    Load load;
    if (!load.File(config.get_filename_scene(), sources, scene,
                detector_array, config))
    {
        cerr << "Loading file \"" << config.get_filename_scene()
        << "\" failed" << endl;
        return(1);
    }

    scene.BuildTree(true, 8.0);

    run_viewer(argc, argv, scene);
    return(0);
}
