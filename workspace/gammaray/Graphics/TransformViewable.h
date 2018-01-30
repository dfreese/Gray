/*
 * Gray: A Ray Tracing-based Monte Carlo Simulator for PET
 *
 * Copyright (c) 2018, David Freese, Peter Olcott, Sam Buss, Craig Levin
 *
 * This software is distributed under the terms of the MIT License unless
 * otherwise noted.  See LICENSE for further details.
 *
 */

#ifndef TRANSFORM_VIEWABLE
#define TRANSFORM_VIEWABLE

// The routines in TransformViewable.cpp and .h allow you to transform
//	  a viewable object with a rigid transformation.  It does this by
//    replacing the data in the viewable object appropriately.
//
// You create a viewable object and then invoke TransformWithRigid
//	  to update it according to the specified rigid transformation.


class RigidMapR3;
class ViewableBezierSet;
class ViewableCone;
class ViewableCylinder;
class ViewableEllipsoid;
class ViewableParallelepiped;
class ViewableParallelogram;
class ViewableSphere;
class ViewableTorus;
class ViewableTriangle;
class BezierPatch;

void TransformWithRigid(  ViewableBezierSet* theObject, const RigidMapR3& theTransform );
void TransformWithRigid(  ViewableCone* theObject, const RigidMapR3& theTransform );
void TransformWithRigid(  ViewableCylinder* theObject, const RigidMapR3& theTransform );
void TransformWithRigid(  ViewableEllipsoid* theObject, const RigidMapR3& theTransform );
void TransformWithRigid(  ViewableParallelepiped* theObject, const RigidMapR3& theTransform );
void TransformWithRigid(  ViewableParallelogram* theObject, const RigidMapR3& theTransform );
void TransformWithRigid(  ViewableSphere* theObject, const RigidMapR3& theTransform );
void TransformWithRigid(  ViewableTorus* theObject, const RigidMapR3& theTransform );
void TransformWithRigid(  ViewableTriangle* theObject, const RigidMapR3& theTransform );
void TransformBezierPatchRecursive( const RigidMapR3& theTransform, BezierPatch* theBp );

#endif    // TRANSFORM_VIEWABLE
