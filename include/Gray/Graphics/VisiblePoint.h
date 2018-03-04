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
    VisiblePoint()
    {
        FrontFace = true;
        MatNeedsFreeing = false;
    };
    ~VisiblePoint();

    void SetPosition( const VectorR3& pos )
    {
        Position = pos;
    }
    void SetNormal( const VectorR3& normal )
    {
        Normal = normal;
    }
    void SetMaterial( const Material& material );
    void SetFrontFace ( bool frontface=true )
    {
        FrontFace = frontface;
    }
    void SetBackFace ()
    {
        FrontFace = false;
    }

    bool IsFrontFacing() const
    {
        return FrontFace;
    }
    bool IsBackFacing() const
    {
        return !FrontFace;
    }

    const VectorR3& GetPosition() const
    {
        return Position;
    }
    const VectorR3& GetNormal() const
    {
        return Normal;
    }
    const Material& GetMaterial() const
    {
        return *Mat;
    }
    Material& GetMaterialMutable()   //assert(MatNeedsFreeing);
    {
        return *Mat;
    }

    void SetUV( double u, double v )
    {
        uvCoords.Set(u,v);
    }
    void SetUV( const VectorR2& uv )
    {
        uvCoords = uv;
    }

    double GetU() const
    {
        return uvCoords.x;
    }
    double GetV() const
    {
        return uvCoords.y;
    }
    const VectorR2& GetUV() const
    {
        return uvCoords;
    }
    VectorR2& GetUV()
    {
        return uvCoords;
    }

    // Face numbers allow different texture maps to be applied to different faces of an object.
    // Typically, the front and back side of a face get the same face number.  However, they
    //  get different texture maps, and also "FrontFace" can be used to distinguish front and back faces.
    // Face numbers are non-negative integers.  Generally: 0 is the "main" face.
    void SetFaceNumber( int faceNumber )
    {
        FaceNumber = faceNumber;
    }
    int GetFaceNumber() const
    {
        return FaceNumber;
    }

    void SetObject( const ViewableBase *object )
    {
        TheObject = object;
    }
    const ViewableBase& GetObject() const
    {
        return *TheObject;
    }

private:
    VectorR3 Position;
    VectorR3 Normal;		// Outward Normal
    Material* Mat;
    VectorR2 uvCoords;		// (u,v) coordinates for texture mapping & etc.
    int FaceNumber;			// Index of face number (non-negative).
    const ViewableBase* TheObject;		// The object from which the visible point came.
    bool FrontFace;			// Is it being viewed from the front side?

    bool MatNeedsFreeing;	// true if we are responsible for freeing the material.

};

inline VisiblePoint::~VisiblePoint()
{
    if ( MatNeedsFreeing ) {
        delete Mat;
        cerr << "Freeing Material\n";
    }
}

inline void VisiblePoint::SetMaterial( const Material& material )
{
    if ( MatNeedsFreeing ) {
        delete Mat;
    }
    Mat = const_cast<Material*>(&material);
    MatNeedsFreeing = false;
}


#endif // VISIBLEPOINT_H
