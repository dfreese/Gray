#include <Gray/GammaRayTrace.h>
#include <Graphics/VisiblePoint.h>
#include <Graphics/ViewableBase.h>
#include <Graphics/ViewableTriangle.h>
#include <GraphicsTrees/IntersectionKdTree.h>
#include <Gray/GammaMaterial.h>
#include <Physics/Interaction.h>
#include <Physics/Positron.h>
#include <Physics/Photon.h>
#include <Output/Output.h>
#include <Sources/Source.h>
#include <Sources/SourceList.h>
#include <stack>


const double GammaRayTrace::Epsilon = 1e-10;

void GammaRayTrace::TracePhoton(
        Photon &photon,
        std::vector<Interaction> & interactions,
        IntersectKdTree & tree,
        GammaMaterial const * const default_material,
        GammaMaterial const * const start_material,
        int MaxTraceDepth)
{
    std::stack<GammaMaterial const * const> MatStack;
    // We don't get the material information from the visible point as we are
    // exiting a material.  This may be problem with the scene setup, but we
    // need to have the default material to enter into once we've exited a
    // material.
    MatStack.push(default_material);
    // Add the material that the photon started in.
    MatStack.push(start_material);
    for (int trace_depth = 0; trace_depth < MaxTraceDepth; trace_depth++) {
        double hitDist;
        VisiblePoint visPoint;
        int intersectNum = tree.SeekIntersection(photon.pos, photon.dir,
                                                 &hitDist, visPoint, -1);
        // There was nothing further in the environment to hit, so return.
        if (intersectNum < 0) {
            return;
        }

        if (MatStack.empty()) {
            // Should always have the default material at the bottom of the
            // stack.  If we somehow pop that out, it means we somehow detected
            // an intersection with a back face that wasn't out of the inital
            // material or preceded by a front face.  This will happen if there
            // some sort of setup error in the KdTree.
            interactions.push_back(Interaction::ErrorEmtpy(photon));
            cout << "ERROR" << endl;
            return;
        }
        GammaMaterial const * const curMaterial = MatStack.top();


        // set detector id in photon
        Interaction interact = Interaction::GammaInteraction(photon, hitDist,
                                                             *curMaterial);
        interactions.push_back(interact);
        switch (interact.type) {
            case Interaction::PHOTOELECTRIC: {
                return;
            }
            case Interaction::XRAY_ESCAPE: {
                return;
            }
            case Interaction::COMPTON: {
                break;
            }
            case Interaction::RAYLEIGH: {
                break;
            }
            case Interaction::NO_INTERACTION: {
                // No need to hold onto non-interactions.
                interactions.pop_back();
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
                break;
            }
            default: {
                cout << "ERROR: Interaction not specified\n";
                return;
            }
        }
    }

    interactions.push_back(Interaction::ErrorTraceDepth(
            photon, *MatStack.top()));
    cout << "ERROR_TRACE_DEPTH" << endl;
    return;
}

void GammaRayTrace::TraceSources(SourceList & sources,
                                 Output & output,
                                 IntersectKdTree & tree,
                                 GammaMaterial const * const default_material)
{
    const int num_chars = 70;

    // calculate the number of positrons to throw
    // TODO: need to fix the number of rays because of negative sources
    // FIXME: time should not increase when Inside() of a negative source
    int num_rays = sources.GetTotalEvents();

    // create a tick mark so that we fill up 40 chars
    int tick_mark = (int)(num_rays / num_chars);
    if (tick_mark == 0) {
        // Make sure we don't have an error later on because of num % 0
        tick_mark = 1;
    }

    // done for PET Benchmark
    //num_rays = 34076000;
    //num_rays = 2938374;

    //TODO: Make a photon stack, then race trace it
    // text graphics preamble
    cout << "[";

    for (int i = 0; i < num_rays; i++) {
        Source * source = sources.Decay();
        Isotope * isotope = source->GetIsotope();
        if (isotope == NULL) {
            cerr << "Empty Decay: ERROR\n";
            continue;
        }

        vector<Interaction> interactions;
        interactions.push_back(Interaction::NuclearDecay(
                *static_cast<Positron*>(isotope)->GetPositronDecay(),
                *source->GetMaterial()));

        // Fun ANSI graphics to do while waiting for simulation
        // this is a simple spinner code
        if ((i % tick_mark) == 0) {
            cout << "=";
            cout.flush();
        }
        while(!isotope->IsEmpty()) {
            Photon photon = isotope->NextPhoton();
            TracePhoton(photon, interactions, tree, default_material,
                        source->GetMaterial(), 100);
        }
        for (const auto & interact: interactions) {
            output.LogInteraction(interact);
        }
    }
    cout << "=] Done.\n";
}
