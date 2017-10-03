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
        long intersectNum = scene.SeekIntersection(photon.pos, photon.dir,
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
        bool is_sensitive = (photon.det_id >= 0);
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
                    photon.det_id = visPoint.GetObject().GetDetectorId();
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
                    photon.det_id = -1;
                    MatStack.pop();
                } else {
                    throw(runtime_error("Material has no face"));
                }
                // Make sure not to hit same place in kdtree
                photon.pos = visPoint.GetPosition() + photon.dir * Epsilon;
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

/*!
 * Checks if a photon's start position changes the geometry it is in relative
 * to the center of the source from which we have already established a
 * reliable materials stack.
 *
 * This calls SeekIntersection limited to the distance between the two points.
 */
bool GammaRayTrace::UpdateStack(const VectorR3 & src_pos,
                                const VectorR3 & pos,
                                std::stack<GammaMaterial const *> & mat_stack) const
{
    // If the points are equal, as they will be for a decay without some sort
    // of blur like positron range, then bail without any ray tracing.
    if (src_pos == pos) {
        return (true);
    }
    // The direction vector from the center of the source to the photon's
    // starting position. Leave this unnormalized for now so we can calculate
    // the distance.
    auto dir = pos - src_pos;
    // This holds the maximum trace distance / returned hit distance
    double dist = dir.Norm();
    // The remaining distance we must trace to the photon's starting point
    double remaining_dist = dist;
    dir.Normalize();
    VisiblePoint point;
    point.SetPosition(src_pos);
    while (scene.SeekIntersection(point.GetPosition() + Epsilon * dir, dir, dist, point) >= 0) {
        remaining_dist -= dist + Epsilon;
        dist = remaining_dist;
        if (point.IsFrontFacing()) {
            // Front face means we are entering a material.
            mat_stack.push(static_cast<GammaMaterial const * const>(&point.GetMaterial()));
        } else if (point.IsBackFacing()) {
            // Back face means we are exiting a material
            if (mat_stack.empty()) {
                // If we somehow have an empty stack, then we somehow missed a
                // front face.
                return (false);
            }
            if (mat_stack.top() != (&point.GetMaterial())) {
                // If the material we find on the back face isn't the material
                // we think we're in, then there's probably some weird overlap.
                return (false);
            }
            // If everything looks okay, pull that material off of the stack.
            mat_stack.pop();
        }
    }
    return (true);
}

void GammaRayTrace::TraceSources(std::vector<Interaction> & interactions,
                                 size_t soft_max_interactions)
{
    while (sources.GetTime() < sources.GetEndTime()) {
        Source * source = sources.Decay();
        stats.events++;
        if (!source) {
            continue;
        }
        Isotope * isotope = source->GetIsotope();
        while(!isotope->IsEmpty()) {
            NuclearDecay * decay = isotope->NextNuclearDecay();
            stats.decays++;
            if (log_nuclear_decays) {
                interactions.push_back(
                        Physics::NuclearDecay(*decay, *source->GetMaterialStack().top()));
            }
            while (!decay->IsEmpty()) {
                Photon & photon = *decay->NextPhoton();
                stats.photons++;
                std::stack<GammaMaterial const *> mat_stack(source->GetMaterialStack());
                if (!UpdateStack(source->GetPosition(), photon.pos, mat_stack)) {
                    ++stats.error;
                    continue;
                }

                TracePhoton(photon, interactions, std::move(mat_stack));
            }
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
