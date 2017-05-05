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
        bool log_nointeractions,
        bool log_errors,
        TraceStats & stats)
{
    std::stack<GammaMaterial const *> MatStack;
    // We don't get the material information from the visible point as we are
    // exiting a material.  This may be problem with the scene setup, but we
    // need to have the default material to enter into once we've exited a
    // material.
    MatStack.push(default_material);
    // Add the material that the photon started in.
    MatStack.push(start_material);
    for (int trace_depth = 0; trace_depth < MaxTraceDepth; trace_depth++) {
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
        const GammaMaterial & mat_gamma_prop = *MatStack.top();


        double hitDist;
        VisiblePoint visPoint;
        long intersectNum = tree.SeekIntersection(photon.pos, photon.dir,
                                                  hitDist, visPoint);
        // There was nothing further in the environment to hit, so return.
        if (intersectNum < 0) {
            stats.no_interaction++;
            if (log_nointeractions) {
                interactions.push_back(Interaction::NoInteraction(photon,
                                                                  mat_gamma_prop));
            }
            return;
        }

        double deposit = 0;
        Interaction::INTER_TYPE type = Interaction::InteractionType(
                photon, hitDist, mat_gamma_prop, deposit);
        bool is_sensitive = (photon.det_id >= 0);
        bool log_interact = ((!is_sensitive && log_nonsensitive) ||
                             is_sensitive);
        // test for Photoelectric interaction
        switch (type) {
            case Interaction::NO_INTERACTION: {
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
                // Make sure not to hit same place in kdtree
                photon.pos = visPoint.GetPosition() + photon.dir * Epsilon;
                break;
            }
            case Interaction::PHOTOELECTRIC: {
                if (log_interact) {
                    interactions.push_back(
                            Interaction::Photoelectric(photon, mat_gamma_prop));
                }
                stats.photoelectric++;
                if (is_sensitive) {
                    stats.photoelectric_sensitive++;
                }
                return;
            }
            case Interaction::XRAY_ESCAPE: {
                if (log_interact) {
                    interactions.push_back(Interaction::XrayEscape(photon, deposit,
                                                                   mat_gamma_prop));
                }
                stats.xray_escape++;
                if (is_sensitive) {
                    stats.xray_escape_sensitive++;
                }
                break;
            }
            case Interaction::COMPTON: {
                if (log_interact) {
                    interactions.push_back(Interaction::Compton(photon, deposit,
                                                                mat_gamma_prop));
                }
                stats.compton++;
                if (is_sensitive) {
                    stats.compton_sensitive++;
                }
                break;
            }
            case Interaction::RAYLEIGH: {
                if (log_interact) {
                    interactions.push_back(Interaction::Rayleigh(photon,
                                                                 mat_gamma_prop));
                }
                stats.rayleigh++;
                if (is_sensitive) {
                    stats.rayleigh_sensitive++;
                }
                break;
            }
            default: {
                throw(runtime_error("Unexpected interaction type in Interaction::GammaInteraction"));
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
                                 bool log_nointeractions,
                                 bool log_errors,
                                 TraceStats & stats)
{
    while (sources.GetTime() < sources.GetEndTime()) {
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
                            source->GetMaterial(), 500, log_nonsensitive,
                            log_nointeractions, log_errors, stats);
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
