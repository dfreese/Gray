#include <Gray/GammaRayTrace.h>
#include <Graphics/VisiblePoint.h>
#include <Graphics/ViewableBase.h>
#include <Graphics/ViewableTriangle.h>
#include <Graphics/SceneDescription.h>
#include <Gray/GammaMaterial.h>
#include <Physics/Interaction.h>
#include <Physics/Positron.h>
#include <Physics/Photon.h>
#include <Physics/Physics.h>
#include <Sources/Source.h>
#include <Sources/SourceList.h>
#include <stack>


const double GammaRayTrace::Epsilon = 1e-10;

GammaRayTrace::GammaRayTrace(SourceList & source_list,
                             const SceneDescription & scene,
                             bool log_nuclear_decays_inter,
                             bool log_nonsensitive_inter,
                             bool log_nointeractions_inter,
                             bool log_errors_inter) :
    sources(source_list),
    scene(scene),
    log_nuclear_decays(log_nuclear_decays_inter),
    log_nonsensitive(log_nonsensitive_inter),
    log_nointeractions(log_nointeractions_inter),
    log_errors(log_errors_inter),
    max_trace_depth(500)
{

}

void GammaRayTrace::TracePhoton(
        Photon &photon,
        std::vector<Interaction> & interactions,
        std::stack<GammaMaterial const *> MatStack)
{
    for (int trace_depth = 0; trace_depth < max_trace_depth; trace_depth++) {
        if (MatStack.empty()) {
            // Should always have the default material at the bottom of the
            // stack.  If we somehow pop that out, it means we somehow detected
            // an intersection with a back face that wasn't out of the inital
            // material or preceded by a front face.  This will happen if there
            // some sort of setup error in the KdTree.
            if (log_errors){
                interactions.push_back(Physics::ErrorEmtpy(photon));
            }
            stats.error++;
            return;
        }
        const GammaMaterial & mat_gamma_prop = *MatStack.top();


        double hitDist = DBL_MAX;
        VisiblePoint visPoint;
        long intersectNum = scene.SeekIntersection(photon.GetPos(), photon.GetDir(),
                                                   hitDist, visPoint);
        // There was nothing further in the environment to hit, so return.
        if (intersectNum < 0) {
            stats.no_interaction++;
            if (log_nointeractions) {
                interactions.push_back(Physics::NoInteraction(photon,
                                                              mat_gamma_prop));
            }
            return;
        }

        double deposit = 0;
        Physics::INTER_TYPE type = Physics::InteractionType(
                photon, hitDist, mat_gamma_prop, deposit);
        bool is_sensitive = (photon.GetDetId() >= 0);
        bool log_interact = ((!is_sensitive && log_nonsensitive) ||
                             is_sensitive);
        // test for Photoelectric interaction
        switch (type) {
            case Physics::NO_INTERACTION: {
                // If not interaction, recursively traverse the in the direction
                // the photon was travelling
                if (visPoint.IsFrontFacing()) {
                    // This detector id will be used to determine if we scatter
                    // in a detector or inside a phantom
                    photon.SetDetId(visPoint.GetObject().GetDetectorId());
                    MatStack.push(static_cast<GammaMaterial const * const>(
                            &visPoint.GetMaterial()));
                } else if (visPoint.IsBackFacing()) {
                    // Check to make sure we are exiting the material we think
                    // we are currently in.
                    if (&visPoint.GetMaterial() != MatStack.top()) {
                        if (log_errors){
                            interactions.push_back(
                                    Physics::ErrorTraceDepth(photon, mat_gamma_prop));
                        }
                        stats.error++;
                        return;
                    }
                    photon.SetDetId(-1);
                    MatStack.pop();
                } else {
                    throw(runtime_error("Material has no face"));
                }
                // Make sure not to hit same place in kdtree
                photon.AddPos(photon.GetDir() * Epsilon);
                break;
            }
            case Physics::PHOTOELECTRIC: {
                if (log_interact) {
                    interactions.push_back(
                            Physics::Photoelectric(photon, mat_gamma_prop));
                }
                stats.photoelectric++;
                if (is_sensitive) {
                    stats.photoelectric_sensitive++;
                }
                return;
            }
            case Physics::XRAY_ESCAPE: {
                if (log_interact) {
                    interactions.push_back(Physics::XrayEscape(photon, deposit,
                                                               mat_gamma_prop));
                }
                stats.xray_escape++;
                if (is_sensitive) {
                    stats.xray_escape_sensitive++;
                }
                break;
            }
            case Physics::COMPTON: {
                if (log_interact) {
                    interactions.push_back(Physics::Compton(photon, deposit,
                                                            mat_gamma_prop));
                }
                stats.compton++;
                if (is_sensitive) {
                    stats.compton_sensitive++;
                }
                break;
            }
            case Physics::RAYLEIGH: {
                if (log_interact) {
                    interactions.push_back(Physics::Rayleigh(photon,
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
        interactions.push_back(Physics::ErrorTraceDepth(
                photon, *MatStack.top()));
    }
    stats.error++;
    return;
}

void GammaRayTrace::TraceSources(std::vector<Interaction> & interactions,
                                 size_t soft_max_interactions)
{
    while (sources.GetTime() < sources.GetEndTime()) {
        NuclearDecay decay = sources.Decay();
        stats.decays++;
        int src_id = decay.GetSourceId();
        if (log_nuclear_decays) {
            interactions.push_back(
                    Physics::NuclearDecay(decay, sources.GetSourceMaterial(src_id)));
        }
        while (!decay.IsEmpty()) {
            Photon photon = decay.NextPhoton();
            stats.photons++;
            TracePhoton(photon, interactions,
                        sources.GetUpdatedStack(src_id, photon.GetPos(), scene));
        }

        if (interactions.size() >= soft_max_interactions) {
            return;
        }
    }
}

const GammaRayTrace::TraceStats & GammaRayTrace::statistics() const {
    return (stats);
}

std::ostream & operator<< (std::ostream & os,
                           const GammaRayTrace::TraceStats& s)
{
    os << "decays: " << s.decays << "\n"
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
