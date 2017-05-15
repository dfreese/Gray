#include <iostream>
#include <Graphics/SceneDescription.h>
#include <GraphicsTrees/IntersectionKdTree.h>
#include <Gray/LoadMaterials.h>
#include <Gray/LoadDetector.h>
#include <Gray/Config.h>
#include <Output/DetectorArray.h>
#include <Output/Output.h>
#include <Sources/SourceList.h>
#include <Random/Random.h>
#include <Physics/Physics.h>
#include <Pipeline/InteractionStream.h>
#include <Viewer.h>

using namespace std;

int main(int argc, char ** argv)
{
    Config config;
    if (!config.ProcessCommandLine(argc,argv)) {
        Config::usage();
        return(-1);
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
    if (config.get_seed_set()) {
        Random::Seed(config.get_seed());
    } else {
        Random::Seed();
    }
    cout << "Using Seed: " << Random::GetSeed() << endl;


    // Setup the singles processor and load a default or specified mapping file
    const double max_req_sort_time = (5 * scene.GetMaxDistance() *
                                      Physics::inverse_speed_of_light);
    InteractionStream singles_stream(max_req_sort_time);

    if (config.get_filename_mapping() == "") {
        singles_stream.set_mappings(detector_array.default_mapping());
    } else if (singles_stream.load_mappings(config.get_filename_mapping()) < 0)
    {
        cerr << "Loading mapping file \"" << config.get_filename_mapping()
        << "\" failed" << endl;
        return(2);
    }

    if (config.get_filename_pipeline() == "") {
        singles_stream.set_processes(config.get_pipeline_lines());
    } else if (singles_stream.load_processes(config.get_filename_pipeline()) < 0)
    {
        cerr << "Loading pipeline file \"" << config.get_filename_pipeline()
             << "\" failed" << endl;
        return(3);
    }

    IntersectKdTree intersect_kd_tree(scene);
    sources.SetKdTree(intersect_kd_tree);

    run_viewer(argc, argv, scene);
    return(0);
}
