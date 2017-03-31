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
        long intersectNum = tree.SeekIntersection(photon.pos, photon.dir,
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

long GammaRayTrace::TraceSources(SourceList & sources,
                                 IntersectKdTree & tree,
                                 long num_decays,
                                 std::vector<Interaction> & interactions,
                                 size_t soft_max_interactions,
                                 GammaMaterial const * const default_material,
                                 bool log_nuclear_decays)
{
    for (long i = 0; i < num_decays; i++) {
        Source * source = sources.Decay();
        Isotope * isotope = source->GetIsotope();
        if (isotope == NULL) {
            cerr << "Empty Decay: ERROR\n";
            continue;
        }

        while(!isotope->IsEmpty()) {
            NuclearDecay * decay = isotope->NextNuclearDecay();
            interactions.push_back(Interaction::NuclearDecay(*decay,
                                                             *source->GetMaterial()));
            while (!decay->IsEmpty()) {
                Photon & photon = *decay->NextPhoton();
                TracePhoton(photon, interactions, tree, default_material,
                            source->GetMaterial(), 100);
            }
        }

        // Perhaps keep a histogram of the number of interactions and keep our
        // probability of overrunning the softmax at a certain threshold.
        size_t next_decay_projected_interactions = interactions.size() +
                static_cast<int>(static_cast<float>(interactions.size()) /
                                 (i + 1));
        if ((interactions.size() >= soft_max_interactions) ||
            (next_decay_projected_interactions >= soft_max_interactions))
        {
            return(i + 1);
        }
    }
    return(num_decays);
}
