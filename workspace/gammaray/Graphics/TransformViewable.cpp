/*
 * Gray: A Ray Tracing-based Monte Carlo Simulator for PET
 *
 * Copyright (c) 2018, David Freese, Peter Olcott, Sam Buss, Craig Levin
 *
 * This software is distributed under the terms of the MIT License unless
 * otherwise noted.  See LICENSE for further details.
 *
 */

// The routines in TransformViewable.cpp and .h allow you to transform
//	  a viewable object with a rigid transformation.  It does this by
//    replacing the data in the viewable object appropriately.
//
// You create a viewable object and then invoke TransformWithRigid
//	  to update it according to the specified rigid transformation.
//    The rigid transformation is represented by a 3x4 matrix,
//		specifically by a RigidMapR3 object.

#include <Graphics/TransformViewable.h>
#include <VrMath/LinearR4.h>
#include <Graphics/CameraView.h>
#include <Graphics/ViewableBezierSet.h>
#include <Graphics/ViewableCone.h>
#include <Graphics/ViewableCylinder.h>
#include <Graphics/ViewableEllipsoid.h>
#include <Graphics/ViewableParallelepiped.h>
#include <Graphics/ViewableParallelogram.h>
#include <Graphics/ViewableSphere.h>
#include <Graphics/ViewableTorus.h>
#include <Graphics/ViewableTriangle.h>

void TransformWithRigid(  ViewableBezierSet* theObject, const RigidMapR3& theTransform )
{
    BezierArray thePatches = theObject->GetPatchList();
    for (long i=0; i < thePatches.size(); i++) {
        BezierPatch* thisBezierPatch = &(thePatches[i]);
        TransformBezierPatchRecursive( theTransform, thisBezierPatch );
    }

    // Transform bounding sphere appropriately
    if ( theObject->BoundingSphereSet ) {
        theTransform.Transform( &(theObject->BoundingSphereCenter) );
    }
}

void TransformBezierPatchRecursive( const RigidMapR3& theTransform, BezierPatch* theBp )
{
    for ( int i=0; i<4; i++ ) {
        for ( int j=0; j<4; j++ ) {
            theTransform.Transform( &(theBp->CntlPts[i][j]) );
        }
    }
    theBp->CalcBoundingPpd();
    if ( theBp->IsSplitIntoTwo() ) {
        TransformBezierPatchRecursive( theTransform, theBp->SplitPatchA );
        TransformBezierPatchRecursive( theTransform, theBp->SplitPatchB );
    }
}


void TransformWithRigid(  ViewableCone* theObject, const RigidMapR3& theTransform )
{
    VectorR3 newAxA, newAxC, newApex;
    // Transform the center
    theTransform.Transform(theObject->GetApex(), &newApex);
    // Transform the axes
    theTransform.Transform3x3(theObject->GetCenterAxis(), &newAxC);
    theTransform.Transform3x3(theObject->GetAxisA(), &newAxA);
    theObject->SetApex( newApex );
    theObject->SetCenterAxis( newAxC );
    theObject->SetRadialAxis( newAxA );

    if ( theObject->IsRightCone() ) {
        theObject->SetHeight( theObject->GetHeight() );
    } else {
        // Top and bottom bounding planes
        VectorR3 newBaseNormal;
        double newPlaneCoefBase;
        theObject->GetBaseFace( &newBaseNormal, &newPlaneCoefBase );
        theTransform.Transform3x3( &newBaseNormal );
        VectorR3 displace = theTransform.Column4();
        newPlaneCoefBase += (newBaseNormal^displace);

        theObject->SetBaseFace( newBaseNormal, newPlaneCoefBase );
    }
}


void TransformWithRigid(  ViewableCylinder* theObject, const RigidMapR3& theTransform )
{
    VectorR3 newAxA, newAxB, newAxC, newCenter;
    // Transform the center
    theTransform.Transform(theObject->GetCenter(), &newCenter);
    // Transform the axes
    theTransform.Transform3x3(theObject->GetCenterAxis(), &newAxC);
    theTransform.Transform3x3(theObject->GetAxisA(), &newAxA);
    theTransform.Transform3x3(theObject->GetAxisB(), &newAxB);
    theObject->SetCenter( newCenter );
    theObject->SetCenterAxis( newAxC );
    theObject->SetRadialAxes( newAxA, newAxB );

    if ( theObject->IsRightCylinder() ) {
        theObject->SetHeight( theObject->GetHeight() );
    } else {
        // Top and bottom bounding planes
        VectorR3 newTopNormal, newBottomNormal;
        double newPlaneCoefTop, newPlaneCoefBottom;
        theObject->GetBottomFace( &newBottomNormal, &newPlaneCoefBottom );
        theObject->GetTopFace( &newTopNormal, &newPlaneCoefTop );
        theTransform.Transform3x3( &newTopNormal );
        theTransform.Transform3x3( &newBottomNormal );
        VectorR3 displace = theTransform.Column4();
        newPlaneCoefTop += (newTopNormal^displace);
        newPlaneCoefBottom += (newBottomNormal^displace);

        theObject->SetTopFace( newTopNormal, newPlaneCoefTop );
        theObject->SetBottomFace( newBottomNormal, newPlaneCoefBottom );
    }
}

void TransformWithRigid(  ViewableEllipsoid* theObject, const RigidMapR3& theTransform )
{
    VectorR3 newAxA, newAxC, newCenter;
    // Transform the center
    theTransform.Transform(theObject->GetCenter(), &newCenter);
    // Transform the axes
    theTransform.Transform3x3(theObject->GetCentralAxis(), &newAxA);
    theTransform.Transform3x3(theObject->GetAxisC(), &newAxC);

    theObject->SetCenter( newCenter );
    theObject->SetAxes( newAxC, newAxA );
}

void TransformWithRigid(  ViewableParallelepiped* theObject, const RigidMapR3& theTransform )
{
    VectorR3 newA, newB, newC, newD;
    // Transform the vertices
    theTransform.Transform( theObject->GetVertexA(), &newA );
    theTransform.Transform( theObject->GetVertexB(), &newB );
    theTransform.Transform( theObject->GetVertexC(), &newC );
    theTransform.Transform( theObject->GetVertexD(), &newD );

    theObject->SetVertices( newA, newB, newC, newD );
}

void TransformWithRigid(  ViewableParallelogram* theObject, const RigidMapR3& theTransform )
{
    VectorR3 newA, newB, newC, vD;
    theObject->GetVertices( &newA, &newB, &newC, &vD );
    // Transform the vertices
    theTransform.Transform( &newA );
    theTransform.Transform( &newB );
    theTransform.Transform( &newC );

    theObject->Init( newA, newB, newC );
}

void TransformWithRigid(  ViewableSphere* theObject, const RigidMapR3& theTransform )
{
    VectorR3 newAxA, newAxC, newCenter;
    // Transform the center
    theTransform.Transform(theObject->GetCenter(), &newCenter);
    // Transform the axes
    theTransform.Transform3x3(theObject->GetAxisA(), &newAxA);
    theTransform.Transform3x3(theObject->GetAxisC(), &newAxC);

    theObject->SetCenter( newCenter );
    theObject->SetuvAxes( newAxA, newAxC );
}

void TransformWithRigid(  ViewableTorus* theObject, const RigidMapR3& theTransform )
{
    VectorR3 newAxA, newAxC, newCenter;
    // Transform the center
    theTransform.Transform(theObject->GetCenter(), &newCenter);
    // Transform the axes
    theTransform.Transform3x3(theObject->GetAxisA(), &newAxA);
    theTransform.Transform3x3(theObject->GetAxisC(), &newAxC);

    theObject->SetCenter( newCenter );
    theObject->SetAxis( newAxC );
    theObject->SetRadialAxis( newAxA );
}

void TransformWithRigid(  ViewableTriangle* theObject, const RigidMapR3& theTransform )
{
    VectorR3 newA, newB, newC;
    theObject->GetVertices( &newA, &newB, &newC );
    // Transform the vertices
    theTransform.Transform( &newA );
    theTransform.Transform( &newB );
    theTransform.Transform( &newC );

    theObject->Init( newA, newB, newC );
}
