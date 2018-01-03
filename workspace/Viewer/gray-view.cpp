#include <iostream>
#include <Graphics/SceneDescription.h>
#include <Gray/Config.h>
#include <Gray/LoadMaterials.h>
#include <Gray/LoadDetector.h>
#include <Gray/Simulation.h>
#include <Output/DetectorArray.h>
#include <Sources/SourceList.h>
#include <Viewer.h>

using namespace std;

int main(int argc, char ** argv)
{
    Config config;
    int config_status = config.ProcessCommandLine(argc, argv, true);
    if (config_status < 0) {
        Config::usage();
        return(1);
    } else if (config_status > 0) {
        Config::usage();
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
    if (!LoadDetector::Load(config.get_filename_scene(), scene, sources,
                            config, detector_array))
    {
        cerr << "Loading file \"" << config.get_filename_scene()
        << "\" failed" << endl;
        return(1);
    }

    scene.BuildTree(true, 8.0);

    run_viewer(argc, argv, scene);
    return(0);
}
