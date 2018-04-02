/*
 * Gray: A Ray Tracing-based Monte Carlo Simulator for PET
 *
 * Copyright (c) 2018, David Freese, Peter Olcott, Sam Buss, Craig Levin
 *
 * This software is distributed under the terms of the MIT License unless
 * otherwise noted.  See LICENSE for further details.
 *
 */

#ifndef VISIBLEPOINT_H
#define VISIBLEPOINT_H

// ****************************************************************************
// The class   VisiblePoint   is defined in this file.
// ****************************************************************************

#include "Gray/VrMath/LinearR2.h"
#include "Gray/VrMath/LinearR3.h"
#include "Gray/Graphics/Material.h"
class ViewableBase;

//  VisiblePoint is a class storing information about a visible point.

class VisiblePoint
{
    //friend ViewableBase;

public:
    VisiblePoint() = default;

    void SetPosition(const VectorR3& pos) {
        Position = pos;
    }
    void SetMaterial(const Material* material) {
        Mat = material;
    }
    void SetFrontFace() {
        FrontFace = true;
    }
    void SetBackFace() {
        FrontFace = false;
    }
    bool IsFrontFacing() const {
        return FrontFace;
    }
    bool IsBackFacing() const {
        return !FrontFace;
    }
    const VectorR3& GetPosition() const {
        return Position;
    }
    const Material* GetMaterial() const {
        return Mat;
    }
    void SetObject(const ViewableBase *object) {
        TheObject = object;
    }
    const ViewableBase& GetObject() const {
        return *TheObject;
    }

private:
    VectorR3 Position;
    Material const* Mat = nullptr;
    // The object from which the visible point came
    ViewableBase const* TheObject = nullptr;
    // Is it being viewed from the front side?
    bool FrontFace = true;

};

#endif // VISIBLEPOINT_H
