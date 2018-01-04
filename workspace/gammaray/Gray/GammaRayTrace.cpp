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
#include <stack>

GammaRayTrace::GammaRayTrace(const SceneDescription & scene,
                             const std::vector<VectorR3>& source_positions,
                             bool log_nondepositing_inter,
                             bool log_nuclear_decays_inter,
                             bool log_nonsensitive_inter,
                             bool log_nointeractions_inter,
                             bool log_errors_inter) :
    scene(scene),
    source_positions(source_positions),
    source_mats(BuildStacks(scene, source_positions)),
    log_nondepositing_inter(log_nondepositing_inter),
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
        std::stack<GammaMaterial const *> MatStack) const
{
    for (int trace_depth = 0; trace_depth < max_trace_depth; trace_depth++) {
        if (MatStack.empty()) {
            // Should always have the default material at the bottom of the
            // stack.  If we somehow pop that out, it means we somehow detected
            // an intersection with a back face that wasn't out of the inital
            // material or preceded by a front face.  This will happen if there
            // some sort of setup error in the KdTree.
            if (log_errors){
                interactions.emplace_back(Physics::ErrorEmtpy(photon));
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
                interactions.emplace_back(
                        Physics::NoInteraction(photon, mat_gamma_prop));
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
                    MatStack.emplace(static_cast<GammaMaterial const * const>(
                            &visPoint.GetMaterial()));
                } else if (visPoint.IsBackFacing()) {
                    // Check to make sure we are exiting the material we think
                    // we are currently in.
                    if (&visPoint.GetMaterial() != MatStack.top()) {
                        if (log_errors){
                            interactions.emplace_back(
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
                photon.AddPos(photon.GetDir() * SceneDescription::ray_trace_epsilon);
                break;
            }
            case Physics::PHOTOELECTRIC: {
                if (log_interact) {
                    interactions.emplace_back(
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
                    interactions.emplace_back(
                            Physics::XrayEscape(photon, deposit, mat_gamma_prop));
                }
                stats.xray_escape++;
                if (is_sensitive) {
                    stats.xray_escape_sensitive++;
                }
                break;
            }
            case Physics::COMPTON: {
                if (log_interact) {
                    interactions.emplace_back(
                            Physics::Compton(photon, deposit, mat_gamma_prop));
                }
                stats.compton++;
                if (is_sensitive) {
                    stats.compton_sensitive++;
                }
                break;
            }
            case Physics::RAYLEIGH: {
                if (log_nondepositing_inter & log_interact) {
                    interactions.emplace_back(
                            Physics::Rayleigh(photon, mat_gamma_prop));
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
        interactions.emplace_back(
                Physics::ErrorTraceDepth(photon, *MatStack.top()));
    }
    stats.error++;
    return;
}

void GammaRayTrace::TraceDecay(NuclearDecay & decay,
                               std::vector<Interaction> & interactions) const
{
    stats.decays++;
    int src_id = decay.GetSourceId();
    if (log_nuclear_decays) {
        interactions.emplace_back(
                Physics::NuclearDecay(decay, SourceMaterial(src_id)));
    }
    for (Photon & photon: decay) {
        stats.photons++;
        TracePhoton(photon, interactions, DecayStack(src_id, photon.GetPos()));
    }
}

std::stack<GammaMaterial const *> GammaRayTrace::BuildStack(
        const SceneDescription & scene,
        const VectorR3& src_pos)
{
    const VectorR3 dir(1, 0, 0);

    std::stack<GammaMaterial const *> materials;
    std::stack<bool> front_face;
    VisiblePoint point;
    point.SetPosition(src_pos);

    double hit_dist = DBL_MAX;
    long obj_num = scene.SeekIntersection(
            point.GetPosition() + dir * SceneDescription::ray_trace_epsilon,
            dir, hit_dist, point);

    while (obj_num >= 0) {
        materials.push(static_cast<GammaMaterial const *>(&point.GetMaterial()));
        if (point.IsFrontFacing()) {
            front_face.push(true);
        } else {
            front_face.push(false);
        }
        hit_dist = DBL_MAX;
        obj_num = scene.SeekIntersection(
                point.GetPosition() + dir * SceneDescription::ray_trace_epsilon,
                dir, hit_dist, point);
    }

    std::stack<GammaMaterial const *> true_materials;
    true_materials.push(
            static_cast<GammaMaterial const *>(&scene.GetDefaultMaterial()));
    while (!materials.empty()) {
        bool is_front_face = front_face.top();
        GammaMaterial const * material = materials.top();
        front_face.pop();
        materials.pop();

        if (!is_front_face) {
            true_materials.push(material);
        } else {
            true_materials.pop();
            if (true_materials.size() < 1) {
                throw runtime_error("Error in determining source materials: potential object overlap error");
            }
        }
    }
    return (true_materials);
}

std::vector<std::stack<GammaMaterial const *>> GammaRayTrace::BuildStacks(
        const SceneDescription & scene,
        const std::vector<VectorR3>& positions)
{
    std::vector<std::stack<GammaMaterial const *>> stacks(positions.size());
    std::transform(positions.begin(), positions.end(), stacks.begin(),
                   [&scene](const VectorR3& src_pos) {
                       return BuildStack(scene, src_pos);
                   });
    return (stacks);
}

/*!
 * Checks if a photon's start position changes the geometry it is in relative
 * to the center of the source from which we have already established a
 * reliable materials stack.
 *
 * This calls SeekIntersection limited to the distance between the two points.
 */
std::stack<GammaMaterial const *> GammaRayTrace::UpdateStack(
        const VectorR3 & src_pos, const VectorR3 & pos,
        const SceneDescription & scene,
        const std::stack<GammaMaterial const *>& base)
{
    std::stack<GammaMaterial const *> mat_stack(base);
    // If the points are equal, as they will be for a decay without some sort
    // of blur like positron range, then bail without any ray tracing.
    if (src_pos == pos) {
        return (mat_stack);
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
    while (scene.SeekIntersection(
            point.GetPosition() + SceneDescription::ray_trace_epsilon * dir,
            dir, dist, point) >= 0)
    {
        remaining_dist -= dist + SceneDescription::ray_trace_epsilon;
        dist = remaining_dist;
        if (point.IsFrontFacing()) {
            // Front face means we are entering a material.
            mat_stack.emplace(static_cast<GammaMaterial const *>(
                    &point.GetMaterial()));
        } else if (point.IsBackFacing()) {
            // Back face means we are exiting a material
            if (mat_stack.empty()) {
                // If we somehow have an empty stack, then we somehow missed a
                // front face.
                break;
            }
            if (mat_stack.top() != (&point.GetMaterial())) {
                // If the material we find on the back face isn't the material
                // we think we're in, then there's probably some weird overlap.
                break;
            }
            // If everything looks okay, pull that material off of the stack.
            mat_stack.pop();
        }
    }
    return (mat_stack);
}

std::stack<GammaMaterial const *> GammaRayTrace::DecayStack(
        size_t src_id, const VectorR3 & pos) const
{
    return (UpdateStack(source_positions[src_id], pos, scene,
                        source_mats[src_id]));
}

const GammaMaterial& GammaRayTrace::SourceMaterial(size_t idx) const {
    return (*source_mats[idx].top());
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
