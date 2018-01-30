/*
 * Gray: a Ray Tracing-based Monte Carlo Simulator for PET
 *
 * Copyright (c) 2018, David Freese, Peter Olcott, Sam Buss, Craig Levin
 *
 * This software is distributed under the terms of the MIT License unless
 * otherwise noted.  See LICENSE for further details.
 *
 */

#ifndef VECTORSOURCE_H_
#define VECTORSOURCE_H_

#include <memory>
#include <Sources/Source.h>
#include <VrMath/LinearR3.h>
#include <VrMath/Aabb.h>

class SceneDescription;

class VectorSource : public Source
{
public:
    VectorSource(const double act, std::unique_ptr<SceneDescription> scene);
    VectorR3 Decay() const override;
    bool Inside(const VectorR3 & pos) const override;

private:
    const VectorR3 size;
    const VectorR3 center;
    std::unique_ptr<SceneDescription> scene;
};

#endif /*VECTORSOURCE_H_*/
