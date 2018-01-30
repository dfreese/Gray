/*
 * Gray: A Ray Tracing-based Monte Carlo Simulator for PET
 *
 * Copyright (c) 2018, David Freese, Peter Olcott, Sam Buss, Craig Levin
 *
 * This software is distributed under the terms of the MIT License unless
 * otherwise noted.  See LICENSE for further details.
 *
 */

#include <Sources/VectorSource.h>
#include <exception>
#include <memory>
#include <Graphics/ViewableTriangle.h>
#include <Random/Random.h>
#include <Graphics/SceneDescription.h>

using namespace std;


VectorSource::VectorSource(const double act,
                           std::unique_ptr<SceneDescription> scene) :
    Source({0, 0, 0}, act),
    size(scene->GetExtents().GetBoxMax() - scene->GetExtents().GetBoxMin()),
    center((scene->GetExtents().GetBoxMax() + scene->GetExtents().GetBoxMin()) / 2.0),
    scene(std::move(scene))
{
    // Build the tree for the source specific scene.
    this->scene->BuildTree(true, 8.0);
}

VectorR3 VectorSource::Decay() const {
    VectorR3 pos;
    do {
        pos = center + Random::UniformRectangle(size);
    } while (!Inside(pos));
    return (pos);
}

bool VectorSource::Inside(const VectorR3 & pos) const
{
    if (!scene->GetExtents().Inside(pos)) {
        return (false);
    }
    VectorR3 dir = Random::UniformSphere();

    double hitDist = std::numeric_limits<double>::max();
    VisiblePoint visPoint;
    long intersectNum = scene->SeekIntersection(pos, dir, hitDist, visPoint);

    if (intersectNum < 0) {
        return false;
    }
    if (visPoint.IsFrontFacing()) {
        return false;
    }
    if (visPoint.IsBackFacing()) {
        return true;
    }
    throw(runtime_error("Material has no face"));
}
