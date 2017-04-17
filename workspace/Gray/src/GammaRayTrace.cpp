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
        int MaxTraceDepth,
        bool log_nonsensitive,
        bool log_errors,
        TraceStats & stats)
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
            stats.no_interaction++;
            return;
        }

        if (MatStack.empty()) {
            // Should always have the default material at the bottom of the
            // stack.  If we somehow pop that out, it means we somehow detected
            // an intersection with a back face that wasn't out of the inital
            // material or preceded by a front face.  This will happen if there
            // some sort of setup error in the KdTree.
            if (log_errors){
                interactions.push_back(Interaction::ErrorEmtpy(photon));
            }
            cout << "ERROR: empty materials stack" << endl;
            stats.error++;
            return;
        }
        GammaMaterial const * const curMaterial = MatStack.top();


        // set detector id in photon
        Interaction interact = Interaction::GammaInteraction(photon, hitDist,
                                                             *curMaterial);

        if (interact.type == Interaction::NO_INTERACTION) {
            // If not interaction, recursively traverse the in the direction
            // the photon was travelling
            if (visPoint.IsFrontFacing()) {
                // This detector id will be used to determine if we scatter
                // in a detector or inside a phantom
                photon.det_id = visPoint.GetObject().GetDetectorId();
                MatStack.push(dynamic_cast<GammaMaterial const * const>(
                        &visPoint.GetMaterial()));
            } else if (visPoint.IsBackFacing()) {
                photon.det_id = -1;
                MatStack.pop();
            } else {
                throw(runtime_error("Material has no face"));
            }
            // calculate the time taken to travel distance of the non-interaction
            photon.time += (hitDist * Interaction::inverse_speed_of_light);

            // Make sure not to hit same place in kdtree
            photon.pos = visPoint.GetPosition() + photon.dir * Epsilon;
        } else {
            bool is_sensitive = (photon.det_id >= 0);
            bool log_interact = ((!is_sensitive && log_nonsensitive) ||
                                 is_sensitive);
            if (log_interact) {
                interactions.push_back(interact);
            }
            // Keep track of what happened
            if (interact.type == Interaction::PHOTOELECTRIC) {
                stats.photoelectric++;
                if (is_sensitive) {
                    stats.photoelectric_sensitive++;
                }
                return;
            } else if (interact.type == Interaction::XRAY_ESCAPE) {
                stats.xray_escape++;
                if (is_sensitive) {
                    stats.xray_escape_sensitive++;
                }
                return;
            } else if (interact.type == Interaction::COMPTON) {
                stats.compton++;
                if (is_sensitive) {
                    stats.compton_sensitive++;
                }
            } else if (interact.type == Interaction::RAYLEIGH) {
                stats.rayleigh++;
                if (is_sensitive) {
                    stats.rayleigh_sensitive++;
                }
            }
        }
    }

    if (log_errors){
        interactions.push_back(Interaction::ErrorTraceDepth(
                photon, *MatStack.top()));
    }
    cout << "ERROR_TRACE_DEPTH" << endl;
    stats.error++;
    return;
}

void GammaRayTrace::TraceSources(SourceList & sources,
                                 IntersectKdTree & tree,
                                 std::vector<Interaction> & interactions,
                                 size_t soft_max_interactions,
                                 GammaMaterial const * const default_material,
                                 bool log_nuclear_decays,
                                 bool log_nonsensitive,
                                 bool log_errors,
                                 TraceStats & stats)
{
    while (sources.GetTime() < sources.GetSimulationTime()) {
        Source * source = sources.Decay();
        stats.events++;
        if (!source) {
            continue;
        }
        Isotope * isotope = source->GetIsotope();
        if (isotope == NULL) {
            cerr << "Empty Decay: ERROR\n";
            stats.error++;
            continue;
        }

        while(!isotope->IsEmpty()) {
            NuclearDecay * decay = isotope->NextNuclearDecay();
            stats.decays++;
            if (log_nuclear_decays) {
                interactions.push_back(
                        Interaction::NuclearDecay(*decay,
                                                  *source->GetMaterial()));
            }
            while (!decay->IsEmpty()) {
                Photon & photon = *decay->NextPhoton();
                stats.photons++;
                TracePhoton(photon, interactions, tree, default_material,
                            source->GetMaterial(), 100, log_nonsensitive,
                            log_errors, stats);
            }
        }

        if (interactions.size() >= soft_max_interactions) {
            return;
        }
    }
}

std::ostream & operator<< (std::ostream & os,
                           const GammaRayTrace::TraceStats& s)
{
    os << "events: " << s.events << "\n"
       << "decays: " << s.decays << "\n"
       << "photons: " << s.photons << "\n"
       << "no_interaction: " << s.no_interaction << "\n"
       << "photoelectric: " << s.photoelectric << "\n"
       << "xray_escape: " << s.xray_escape << "\n"
       << "compton: " << s.compton << "\n"
       << "rayleigh: " << s.rayleigh << "\n"
       << "photoelectric_sensitive: " << s.photoelectric_sensitive << "\n"
       << "xray_escape_sensitive: " << s.xray_escape_sensitive << "\n"
       << "compton_sensitive: " << s.compton_sensitive << "\n"
       << "rayleigh_sensitive: " << s.rayleigh_sensitive << "\n"
       << "error: " << s.error << "\n";
    return os;
}
