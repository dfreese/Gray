#include <Gray/GammaRayTrace.h>
#include <Graphics/VisiblePoint.h>
#include <Graphics/ViewableBase.h>
#include <Graphics/ViewableTriangle.h>
#include <GraphicsTrees/IntersectionKdTree.h>
#include <Gray/GammaMaterial.h>
#include <Physics/Interaction.h>
#include <Physics/Positron.h>
#include <Physics/Photon.h>
#include <Sources/Source.h>

GammaRayTrace::GammaRayTrace()
{
    defaultMat = NULL;

    // Default simulation time is 1 second
    simulationTime = 1.0;

}

GammaRayTrace::~GammaRayTrace()
{
}

void GammaRayTrace::SetFileNameOutput(const std::string & name)
{
    output.SetLogfile(name);
}

void GammaRayTrace::SetDefaultMaterial(GammaMaterial * mat)
{
    defaultMat = mat;
}

void GammaRayTrace::SetSimulationTime(double time)
{
    simulationTime = time;
};

void GammaRayTrace::SetKdTree(IntersectKdTree & tree) {
    kd_tree = &tree;
    sources.SetKdTree(tree);
}

Interaction::INTER_TYPE GammaRayTrace::GRayTrace(
        VisiblePoint &visPoint, int TraceDepth, Photon &photon,
        std::stack<GammaMaterial const * const> & MatStack, long avoidK=-1)
{
    if (MatStack.empty()) {
        output.LogError(photon, Output::ERROR_EMPTY,  0);
        cout << "ERROR" << endl;
        return Interaction::ERROR;
    }
    GammaMaterial const * const curMaterial = MatStack.top();
    if (TraceDepth <= 0) {
        output.LogError(photon, Output::ERROR_TRACE_DEPTH, curMaterial->GetMaterial());
        cout << "ERROR_TRACE_DEPTH" << endl;
        return Interaction::NO_INTERACTION;
    }

    double hitDist;
    int intersectNum = kd_tree->SeekIntersection(photon.pos, photon.dir,
                                                 &hitDist, visPoint, avoidK);

    // There was nothing further in the environment to hit, so return.
    if (intersectNum < 0) {
        return Interaction::NO_INTERACTION;
    }

    

    // set detector id in photon
    double prev_energy = photon.energy;
    switch(Interaction::GammaInteraction(photon, hitDist, *curMaterial)) {
        case Interaction::PHOTOELECTRIC: {
            output.LogPhotoElectric(photon, (*curMaterial));
            return Interaction::PHOTOELECTRIC;
        }
        case Interaction::XRAY_ESCAPE: {
            return Interaction::XRAY_ESCAPE;
        }
        case Interaction::COMPTON: {
            // log interaction to file
            double deposit = prev_energy - photon.energy;
            output.LogCompton(photon, deposit, *curMaterial);
            return GRayTrace(visPoint, TraceDepth - 1, photon,MatStack, avoidK);
        }
        case Interaction::RAYLEIGH: {
            // log interaction to file
            output.LogRayleigh(photon, *curMaterial);
            return GRayTrace(visPoint, TraceDepth - 1, photon, MatStack, avoidK);
        }
        case Interaction::NO_INTERACTION: {
            // If not interaction, recursively traverse the in the direction the
            // photon was travelling
            if (visPoint.IsFrontFacing()) {
                // This detector id will be used to determine if we scatter in
                // a detector or inside a phantom
                photon.det_id = visPoint.GetObject().GetDetectorId();
                MatStack.push(dynamic_cast<GammaMaterial const * const>(
                        &visPoint.GetMaterial()));
            } else if (visPoint.IsBackFacing()) {
                photon.det_id = -1;
                MatStack.pop();
            } else {
                cout << "ERROR: material has no face\n";
                exit(1);
            }
            // calculate the time taken to travel distance of the non-interaction
            photon.time += (hitDist * Interaction::si1_SOL);

            // Make sure not to hit same place in kdtree
            photon.pos = visPoint.GetPosition() + photon.dir * Epsilon;
            return(GRayTrace(visPoint, TraceDepth - 1, photon, MatStack, avoidK));
        }
        default: {
            cout << "ERROR: Interaction not specified\n";
            return Interaction::NO_INTERACTION;
        }
    }
}

void GammaRayTrace::GRayTraceSources(void)
{
    const int num_chars = 70;

    // calculate the number of positrons to throw
    // TODO: need to fix the number of rays because of negative sources
    // FIXME: time should not increase when Inside() of a negative source
    int num_rays = sources.GetTotalEvents(simulationTime);

    // create a tick mark so that we fill up 40 chars
    int tick_mark = (int)(num_rays / num_chars);
    if (tick_mark == 0) {
        // Make sure we don't have an error later on because of num % 0
        tick_mark = 1;
    }

    // done for PET Benchmark
    //num_rays = 34076000;
    //num_rays = 2938374;

    std::stack<GammaMaterial const * const> MatStack;
    MatStack.push(defaultMat);

    VisiblePoint visPoint;
    int TraceDepth;

    //TODO: Make a photon stack, then race trace it
    // text graphics preamble
    cout << "[";

    for (int i = 1; i < num_rays; i++) {
        Source * source = sources.Decay();
        Isotope * isotope = source->GetIsotope();
        if (isotope == NULL) {
            cerr << "Empty Decay: ERROR\n";
            continue;
        }

        output.LogNuclearDecay(((Positron*)isotope)->GetPositron());

        // Fun ANSI graphics to do while waiting for simulation
        // this is a simple spinner code
        if ((i % tick_mark) == 0) {
            cout << "=";
            cout.flush();
        }
        while(!isotope->IsEmpty()) {
            Photon photon = isotope->NextPhoton();

            TraceDepth = 100;
            MatStack.push(source->GetMaterial());
            GRayTrace(visPoint, TraceDepth, photon, MatStack);
            // Reset the material stack to be empty with the default material.
            while (!MatStack.empty()) {
                MatStack.pop();
            }
            MatStack.push(defaultMat);
        }
    }
    cout << "=] Done.\n";
}

void GammaRayTrace::AddSource(Source * s)
{
    sources.AddSource(s);
}
