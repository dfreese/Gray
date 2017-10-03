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
    if (argc == 1) {
        Config::usage();
        return(0);
    }
    Config config;
    if (!config.ProcessCommandLine(argc, argv, true)) {
        Config::usage();
        return(1);
    }
    DetectorArray detector_array;
    SceneDescription scene;
    SourceList sources;
    if (!sources.LoadIsotopes(config.get_isotopes_filename())) {
        cerr << "Unable to load isotopes file: \""
        << config.get_isotopes_filename() << "\"\n"
        << "Check GRAY_INCLUDE env variable or specify name with --iso"
        << endl;
        return(1);
    }
    if (!LoadMaterials::LoadPhysicsFiles(scene,
                                         config.get_materials_filename()))
    {
        cerr << "Unable to load materials file: \""
        << config.get_materials_filename() << "\"\n"
        << "Check GRAY_INCLUDE env variable or specify name with --mat"
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
