/*
 * Gray: A Ray Tracing-based Monte Carlo Simulator for PET
 *
 * Copyright (c) 2018, David Freese, Peter Olcott, Sam Buss, Craig Levin
 *
 * This software is distributed under the terms of the MIT License unless
 * otherwise noted.  See LICENSE for further details.
 *
 */

#include "Gray/Graphics/ViewableBezierSet.h"
#include "Gray/Graphics/ViewableSphere.h"
#include "Gray/Graphics/ViewableParallelepiped.h"
#include "Gray/VrMath/LinearR4.h"

#include <stdio.h>
#include <list>

// The next two variables control recusion depth for subdivision
//	during Bezier intersection.

int BezierPatchMgr::MaxRecurseSave = 10;
int BezierPatchMgr::MinIsectRecurse = 18;

// The next variables hold what is essentially a stack of Bezier patches.
// They are sorted (more-or-less) in order of hit distance to their
//		bounding parallelepiped.
// These are meant for local use only, but are global to save on
//		memory management overhead.
static const int BP_MAX_NUM_PATCHES = 192;	// Maximum number of B'ezier patches that can be active
static int BpStackSize;
static double BpPpdDistIn[BP_MAX_NUM_PATCHES];
static double BpPpdDistOut[BP_MAX_NUM_PATCHES];
static BezierPatch* BezPatchStack[BP_MAX_NUM_PATCHES];
void BpPushPatchToStack( BezierPatch*, double HitDistIn, double HitDistOut, int SortRange );


// Returns an intersection if found with distance maxDistance
// viewDir must be a unit vector.
// intersectDistance and visPoint are returned values.
bool ViewableBezierSet::FindIntersectionNT (
    const VectorR3& viewPos, const VectorR3& viewDir, double maxDist,
    double *intersectDistance, VisiblePoint& returnedPoint ) const
{
    // Start by computing bounding sphere if necessary
    if ( !BoundingSphereSet ) {
        (const_cast<ViewableBezierSet*>(this))->CalcBoundingSphere();
    }

    // Check against bounding sphere
    double quickSphereDistance;
    if ( !ViewableSphere::QuickIntersectTest(viewPos, viewDir, maxDist,
            &quickSphereDistance, BoundingSphereCenter, BoundingSphereRadiusSq ) ) {
        return false;
    }

    // For each BezierPatch, check ray against bounding parallelepiped
    //	Those that are hit by the ray are stored into an array sorted
    //	in order of hit distance.
    BpStackSize=0;

    double intersectDistanceIn, intersectDistanceOut;

    // Loop over each patch, check their bounding parallelepiped.
    //   For those that are hit, push them into the stack.
    for (size_t i = 0; i < PatchList.size(); i++ ) {
        const BezierPatch* bPatch = &(PatchList[i]);
        if ( ViewableParallelepiped::QuickIntersectTest( viewPos, viewDir, maxDist,
                &intersectDistanceIn, &intersectDistanceOut,
                bPatch->NormalA, bPatch->MinDotA, bPatch->MaxDotA,
                bPatch->NormalB, bPatch->MinDotB, bPatch->MaxDotB,
                bPatch->NormalC, bPatch->MinDotC, bPatch->MaxDotC ) ) {
            if ( BpStackSize>=BP_MAX_NUM_PATCHES ) {
                assert( 0 && "BP Stack Overflow (Bezier Patch)!" );
                return false;
            }
            BpPushPatchToStack( const_cast<BezierPatch*>(bPatch), intersectDistanceIn, intersectDistanceOut, BpStackSize );
        }
    }

    // The stack holds the BezierPatches with which we should try to collide
    //	Items in the stack have already been tested with "QuickIntersectTest".
    // Loop until stack is empty
    // Look at top member of stack.
    //     If the distance to its bounding parallelpiped is too large, discard it.
    //	   If it is a good, high quality intersection, and it closer than the previous best,
    //				save it and pop the stack.
    // When stack is empty, return whether intersection found, and the intersection information.

    VisiblePoint* bestIntersection = 0;
    double bestIntersectDist = DBL_MAX;
    VectorR3 temp;
    VectorR4 tempHg;
    VectorR3 hitPosMaybe;
    VectorR2 uVmaybe;
    while ( BpStackSize>0 ) {
        int i = BpStackSize-1;
        if ( bestIntersection && BpPpdDistIn[i]>=bestIntersectDist ) {
            // This is too far away to still be hit.  Remove from stack and continue
            BezierPatchMgr::ReleaseBezierPatch( BezPatchStack[BpStackSize-1] );
            BpStackSize--;
            continue;
        }
        BezierPatch* bp = BezPatchStack[i];
        if ( bp->MaxDotA-bp->MinDotA + bp->MaxDotB-bp->MinDotB < 1.0e-14 ) {
            // This patch is too small to continue subdividing. Remove from stack and continue
            BezierPatchMgr::ReleaseBezierPatch( BezPatchStack[BpStackSize-1] );
            BpStackSize--;
            continue;
        }

        if ( !bp->MgrNeedToRecurse() ) {
            // Intersect with the midpoint of where enters and exits patch
            double alpha;
            alpha = 0.5*( std::max(BpPpdDistIn[i],0.0) + std::min(BpPpdDistOut[i],maxDist) );
            assert ( alpha>= 0.0 );
            hitPosMaybe = viewDir;
            hitPosMaybe *= alpha;
            hitPosMaybe += viewPos;
            uVmaybe.x = ((hitPosMaybe^bp->NormalA)-bp->MinDotA)/(bp->MaxDotA-bp->MinDotA);
            uVmaybe.y = ((hitPosMaybe^bp->NormalB)-bp->MinDotB)/(bp->MaxDotB-bp->MinDotB);
            //BezierPatch::BiLinearInvert( hitPosMaybe,
            //				bp->CntlPts[0][0], bp->CntlPts[3][0],
            //				bp->CntlPts[3][3], bp->CntlPts[0][3],  bp->NormalC,
            //				&uVmaybe );
            bp->EvalPatch( uVmaybe, &tempHg );
            temp.SetFromHg(tempHg);
            double distSq = temp.DistSq(hitPosMaybe);
            if ( distSq<1.0e-8 && (distSq < 1.0e-13
                                   || (bp->MaxDotC-bp->MinDotC) < 1.0e-8) ) {
                // If we have a good close hit!!
                if ( alpha<bestIntersectDist && alpha<maxDist ) {
                    if ( !bestIntersection ) {
                        bestIntersection = &returnedPoint;
                    }
                    bestIntersectDist = alpha;
                    bestIntersection->SetPosition(hitPosMaybe);
                    bp->EvalPatchNormal( uVmaybe, tempHg, &temp, &(bp->NormalC) );
                    if ( (viewDir^temp) < 0 ) {	// if front face hit
                        bestIntersection->SetFrontFace();
                        bestIntersection->SetMaterial(ViewableBase::GetMaterialFront());
                    } else {
                        bestIntersection->SetBackFace();
                        bestIntersection->SetMaterial(ViewableBase::GetMaterialBack());
                    }
                }
                // Done with this patch.  Remove from stack and continue looping
                BezierPatchMgr::ReleaseBezierPatch( BezPatchStack[BpStackSize-1] );
                BpStackSize--;
                continue;
            }
        }

        // Split the patch into two
        BezierPatch* bpX0;
        BezierPatch* bpX1;
        BezierPatchMgr::GetTwoSubPatchs( *bp, &bpX0, &bpX1 );
        BezierPatchMgr::ReleaseBezierPatch( BezPatchStack[BpStackSize-1] );
        BpStackSize--;						// Pop off old patch
        bpX0->CalcBoundingPpd();
        bpX1->CalcBoundingPpd();
        int sortRange = 0;
        if ( ViewableParallelepiped::QuickIntersectTest( viewPos, viewDir, maxDist,
                &intersectDistanceIn, &intersectDistanceOut,
                bpX0->NormalA, bpX0->MinDotA, bpX0->MaxDotA,
                bpX0->NormalB, bpX0->MinDotB, bpX0->MaxDotB,
                bpX0->NormalC, bpX0->MinDotC, bpX0->MaxDotC ) ) {
            if ( BpStackSize>=BP_MAX_NUM_PATCHES ) {
                assert( 0 && "BP Stack Overflow (Bezier Patch)!" );
                return false;
            }
            BpPushPatchToStack( bpX0, intersectDistanceIn, intersectDistanceOut, 0 );
            sortRange = 1;
        } else {
            BezierPatchMgr::ReleaseBezierPatch( bpX0 );
        }
        if ( ViewableParallelepiped::QuickIntersectTest( viewPos, viewDir, maxDist,
                &intersectDistanceIn, &intersectDistanceOut,
                bpX1->NormalA, bpX1->MinDotA, bpX1->MaxDotA,
                bpX1->NormalB, bpX1->MinDotB, bpX1->MaxDotB,
                bpX1->NormalC, bpX1->MinDotC, bpX1->MaxDotC ) ) {
            if ( BpStackSize>=BP_MAX_NUM_PATCHES ) {
                assert( 0 && "BP Stack Overflow (Bezier Patch)!" );
                return false;
            }
            BpPushPatchToStack( bpX1, intersectDistanceIn, intersectDistanceOut, sortRange );
        } else {
            BezierPatchMgr::ReleaseBezierPatch( bpX1 );
        }
    }

    if ( bestIntersection) {	// If we found a good hit, copy to be returned
        *intersectDistance = bestIntersectDist;
        return true;
    } else {
        return false;
    }
}

// Maintains an *approximate* priority queue.
//	sortRange indicates how far back to search for insertion point.
void BpPushPatchToStack( BezierPatch* thePatch, double hitDistIn, double hitDistOut, int sortRange )
{
    int i;
    int j = sortRange;
    // Compute the index i where to put thePatch.
    for ( i=BpStackSize; i>0 && j>0 ; i--, j-- ) {
        if ( hitDistIn<=BpPpdDistIn[i-1] ) {
            break;
        }
    }
    for ( j=BpStackSize-1; j>=i; j-- ) {
        BpPpdDistIn[j+1] = BpPpdDistIn[j];
        BpPpdDistOut[j+1] = BpPpdDistOut[j];
        BezPatchStack[j+1] = BezPatchStack[j];
    }
    BpPpdDistIn[i] = hitDistIn;
    BpPpdDistOut[i] = hitDistOut;
    BezPatchStack[i] = thePatch;
    BpStackSize++;
}

void ViewableBezierSet::CalcBoundingSphere()
{
    if ( !BoundingSphereManuallySet ) {
        CalcBoundingSphereCenter();
    }

    double radiusSq=0.0;
    VectorR3 temp;
    for (size_t i=0; i < PatchList.size(); i++ ) {
        const BezierPatch& bPatch = PatchList[i];
        if ( !bPatch.IsSplitIntoTwo() ) {
            for ( int i=0; i<4; i++ ) {
                for ( int j=0; j<4; j++ ) {
                    temp.SetFromHg(bPatch.CntlPts[i][j]);
                    double distSq = temp.DistSq(BoundingSphereCenter);
                    if ( distSq>radiusSq ) {
                        radiusSq = distSq;
                    }
                }
            }
        }
    }
    BoundingSphereRadiusSq = radiusSq;
    BoundingSphereSet = true;
}

// Get a bounding sphere center by averaging all control points.
//		Easy, but poor quality estimate
void ViewableBezierSet::CalcBoundingSphereCenter()
{
    VectorR3 ctrAccum;
    int num = 0;
    VectorR3 temp;
    for (size_t i = 0; i < PatchList.size(); i++ ) {
        const BezierPatch& bPatch = PatchList[i];
        for ( int i=0; i<4; i++ ) {
            for ( int j=0; j<4; j++ ) {
                temp.SetFromHg(bPatch.CntlPts[i][j]);
                ctrAccum += temp;
            }
        }
        num += 12;
    }
    ctrAccum /= (double)num;
    BoundingSphereCenter = ctrAccum;

}

int ViewableBezierSet::AddPatch(int uOrder, int vOrder, const double* controlPoints,
                                int uStride, int vStride)
{
    assert ( (uStride!=0&&vStride!=0) || ( uStride==0&&vStride==0) );
    if ( uStride == 0 ) {
        uStride = 3;
        vStride = 3*uOrder;
    }
    VectorR4 controlPts[4][4];
    const double* t = controlPoints;
    for (int j=0; j<vOrder; j++) {
        const double* s = t;
        for ( int i=0; i<uOrder; i++ ) {
            controlPts[i][j].Set( *s, *(s+1), *(s+2), 1.0 );
            s += uStride;
        }
        t += vStride;
    }

    return AddPatchInner( uOrder, vOrder, controlPts );
}

int ViewableBezierSet::AddRationalPatch(int uOrder, int vOrder, const double* controlPoints,
                                        int uStride, int vStride)
{
    assert ( (uStride!=0&&vStride!=0) || ( uStride==0&&vStride==0) );
    if ( uStride == 0 ) {
        uStride = 4;
        vStride = 4*uOrder;
    }
    VectorR4 controlPts[4][4];
    const double* t = controlPoints;
    for (int j=0; j<vOrder; j++) {
        const double* s = t;
        for ( int i=0; i<uOrder; i++ ) {
            controlPts[i][j].Load(s);
            s += uStride;
        }
        t += vStride;
    }

    return AddPatchInner( uOrder, vOrder, controlPts );
}

int ViewableBezierSet::AddPatch(int uOrder, int vOrder, const float* controlPoints,
                                int uStride, int vStride)
{
    assert ( (uStride!=0&&vStride!=0) || ( uStride==0&&vStride==0) );
    if ( uStride == 0 ) {
        uStride = 3;
        vStride = 3*uOrder;
    }
    VectorR4 controlPts[4][4];
    const float* t = controlPoints;
    for (int j=0; j<vOrder; j++) {
        const float* s = t;
        for ( int i=0; i<uOrder; i++ ) {
            controlPts[i][j].Set( *s, *(s+1), *(s+2), 1.0 );
            s += uStride;
        }
        t += vStride;
    }

    return AddPatchInner( uOrder, vOrder, controlPts );
}

int ViewableBezierSet::AddRationalPatch(int uOrder, int vOrder, const float* controlPoints,
                                        int uStride, int vStride)
{
    assert ( (uStride!=0&&vStride!=0) || ( uStride==0&&vStride==0) );
    if ( uStride == 0 ) {
        uStride = 4;
        vStride = 4*uOrder;
    }
    VectorR4 controlPts[4][4];
    const float* t = controlPoints;
    for (int j=0; j<vOrder; j++) {
        const float* s = t;
        for ( int i=0; i<uOrder; i++ ) {
            controlPts[i][j].Load(s);
            s += uStride;
        }
        t += vStride;
    }

    return AddPatchInner( uOrder, vOrder, controlPts );
}

int ViewableBezierSet::AddPatch(int uOrder, int vOrder, const VectorR3* controlPoints,
                                int uStride, int vStride)
{
    assert ( (uStride!=0&&vStride!=0) || ( uStride==0&&vStride==0) );
    if ( uStride == 0 ) {
        uStride = 1;
        vStride = uOrder;
    }
    VectorR4 controlPts[4][4];
    const VectorR3* t = controlPoints;
    for (int j=0; j<vOrder; j++) {
        const VectorR3* s = t;
        for ( int i=0; i<uOrder; i++ ) {
            controlPts[i][j].Set(s->x, s->y, s->z, 1.0);
            s += uStride;
        }
        t += vStride;
    }

    return AddPatchInner( uOrder, vOrder, controlPts );
}

int ViewableBezierSet::AddPatch(int uOrder, int vOrder, const VectorR4* controlPoints,
                                int uStride, int vStride)
{
    assert ( (uStride!=0&&vStride!=0) || ( uStride==0&&vStride==0) );
    if ( uStride == 0 ) {
        uStride = 1;
        vStride = uOrder;
    }
    VectorR4 controlPts[4][4];
    const VectorR4* t = controlPoints;
    for (int j=0; j<vOrder; j++) {
        const VectorR4* s = t;
        for ( int i=0; i<uOrder; i++ ) {
            controlPts[i][j] = *s;
            s += uStride;
        }
        t += vStride;
    }

    return AddPatchInner( uOrder, vOrder, controlPts );
}

int ViewableBezierSet::AddPatchInner(int uOrder, int vOrder, VectorR4 controlPts[4][4])
{
    // Convert to order 4 by order 4
    assert ( 3<=uOrder && uOrder<=4 && 3<=vOrder && vOrder<=4 );
    if ( !( 3<=uOrder && uOrder<=4 && 3<=vOrder && vOrder<=4 ) ) {
        return -1;				// Just ignore this one!
    }
    if ( uOrder==3 ) {
        VectorR4 temp;
        for (int j=0; j<vOrder; j++ ) {
            controlPts[3][j] = controlPts[2][j];
            temp = controlPts[1][j];
            temp *= 0.666666666666666667;
            controlPts[2][j] *= 0.333333333333333333;
            controlPts[2][j] += temp;
            controlPts[1][j] = controlPts[0][j];
            controlPts[1][j] *= 0.333333333333333333;
            controlPts[1][j] += temp;
        }
    }
    if ( vOrder==3 ) {
        VectorR4 temp;
        for (int i=0; i<4; i++ ) {
            controlPts[i][3] = controlPts[i][2];
            temp = controlPts[i][1];
            temp *= 0.666666666666666667;
            controlPts[i][2] *= 0.333333333333333333;
            controlPts[i][2] += temp;
            controlPts[i][1] = controlPts[i][0];
            controlPts[i][1] *= 0.333333333333333333;
            controlPts[i][1] += temp;
        }
    }
    // Save the original input patch
    OriginalPatches.push_back(BezierPatch());
    BezierPatch& origBP = OriginalPatches.back();
    origBP.SetControlPoints(controlPts);
    origBP.FaceNum = PatchCounter;

    BezierPatch* bp = new BezierPatch(controlPts);
    bp->SetUvRange( NextUvMin, NextUvMax );
    bp->FaceNum = PatchCounter;
    // Set Defaults
    bp->FrontMaterial = ViewableBase::GetMaterialFront();
    bp->BackMaterial = ViewableBase::GetMaterialBack();

    std::list<BezierPatch *> bp_list;
    bp_list.push_back(bp);

    // Subdivide until decently flat and has no points at infinity
    //   When split, the current patch in the linked list is
    //	 replaced by four new patches.
    bool someNewPatch = true;
    int splitCounter = 0;
    while ( someNewPatch ) {
        someNewPatch = false;
        std::list<BezierPatch *>::iterator bIter = bp_list.begin();
        while (bIter == bp_list.end()) {
            bool needSplit;
            bp = *bIter;
            if (bp->HasPointAtInfinity()) {
                needSplit = true;
            } else {
                bp->CalcBoundingPpd();
                needSplit = !(bp->BoundingPpdNice());
            }
            if ( needSplit ) {
                someNewPatch = true;
                BezierPatch* bpU0 = new BezierPatch();
                BezierPatch* bpU1 = new BezierPatch();
                BezierPatch* bpU0V0 = new BezierPatch();
                BezierPatch* bpU0V1 = new BezierPatch();
                BezierPatch* bpU1V0 = bpU0;				// Dangerous re-use of bp
                BezierPatch* bpU1V1 = bp;				// Dangerous re-use of bpU0
                bp->MakeSplitU ( bpU0, bpU1 );
                bpU0->MakeSplitV ( bpU0V0, bpU0V1 );
                bpU0V0->CalcBoundingPpd();
                bpU0V1->CalcBoundingPpd();
                bp_list.insert(bIter, bpU0V0);
                bp_list.insert(bIter, bpU0V1);
                bpU1->MakeSplitV ( bpU1V0, bpU1V1 );
                bpU1V0->CalcBoundingPpd();
                bpU1V1->CalcBoundingPpd();
                bp_list.insert(bIter, bpU1V0);

                //bPtr->AddAfter( bpU1V1 );
                //bPtr->Remove();				// Remove from the linked list
                delete bpU1;					// Only patch that was not added to linked list
            }
            bIter++;
        }
        splitCounter++;
        assert( splitCounter<9 && "Patches may be badly formed.");
    }
    // Copy into the master linked list
    std::list<BezierPatch *>::iterator bIter = bp_list.begin();
    while (bIter == bp_list.end()) {
        BezierPatch* bp = *bIter;
        PatchList.push_back(*bp);
        bIter++;
    }

    PatchCounter++;
    return PatchCounter-1;
}

void BezierPatch::CalcBoundingPpd()		// Compute a bounding parallelepiped
{
    VectorR3 temp;
    VectorR3 CornerA, CornerB, CornerC, CornerD;
    CornerA.SetFromHg(CntlPts[0][0]);
    CornerB.SetFromHg(CntlPts[3][0]);
    CornerC.SetFromHg(CntlPts[3][3]);
    CornerD.SetFromHg(CntlPts[0][3]);

    NormalC = CornerC;
    NormalC -= CornerA;
    NormalB = NormalC;
    NormalA = NormalC;
    temp = CornerD;
    temp -= CornerB;
    NormalC *= temp;					// Cross-product;
    //NormalA -= temp;			// OLD: Wrong calculation - gives SideA
    //NormalB += temp;			// OLD: Wrong calculation - gives SideB
    NormalA += temp;					// Points along SideB
    NormalA *= NormalC;					// Point inward perpindicular to SideA
    NormalB -= temp;					// Points along SideA
    NormalB.CrossProductLeft(NormalC);	// Points inward perpindicular to SideB

    BoundingPpdBad = false;
    double normSq;
    normSq = NormalC.NormSq();
    if ( normSq > 0.0 ) {
        NormalC /= sqrt(normSq);
    } else {
        BoundingPpdBad = true;
    }
    normSq = NormalA.NormSq();
    if ( normSq > 0.0 ) {
        NormalA /= sqrt(normSq);
    } else {
        BoundingPpdBad = true;
    }
    normSq = NormalB.NormSq();
    if ( normSq > 0.0 ) {
        NormalB /= sqrt(normSq);
    } else {
        BoundingPpdBad = true;
    }
    if ( BoundingPpdBad ) {
        NormalA.SetUnitZ();
        NormalB.SetUnitX();
        NormalC.SetUnitY();
    }

    GetMinMaxDot( NormalC, &MinDotC, &MaxDotC );
    GetMinMaxDot( NormalA, &MinDotA, &MaxDotA );
    GetMinMaxDot( NormalB, &MinDotB, &MaxDotB );

}

bool BezierPatch::BoundingPpdNice()  		// Is the bounding parallelepiped nice?
{
    if ( BoundingPpdBad ) {
        return false;
    }
    double deltaC = MaxDotC-MinDotC;
    return ( deltaC<0.5*(MaxDotB-MinDotB) && deltaC<0.5*(MaxDotA-MinDotA) );
}

void BezierPatch::GetMinMaxDot(const VectorR3& n, double* minDot, double* maxDot) const
{
    *minDot = +DBL_MAX;
    *maxDot = -DBL_MAX;
    for ( int i=0; i<4; i++ ) {
        for ( int j=0; j<4; j++ ) {
            const VectorR4& cp = CntlPts[i][j];
            double dot = (n.x*cp.x + n.y*cp.y + n.z*cp.z)/cp.w;
            if ( dot < *minDot ) {
                *minDot = dot;
            }
            if ( dot > *maxDot ) {
                *maxDot = dot;
            }
        }
    }
}

void BezierPatch::GetMinMaxDotRecursive(const VectorR3& n, double* minDot, double* maxDot) const
{
    if ( IsSplitIntoTwo() ) {		// If split into two, recursively use the two subpatches.
        double minA, maxA;
        SplitPatchA->GetMinMaxDotRecursive( n, &minA, &maxA );
        double minB, maxB;
        SplitPatchB->GetMinMaxDotRecursive( n, &minB, &maxB );
        *minDot = std::min(minA, minB);
        *maxDot = std::max(maxA, maxB);
    } else {
        GetMinMaxDot( n, minDot, maxDot );
    }
}

BezierPatch::BezierPatch()
{
    UvMin.Set(0.0,0.0);
    UvMax.Set(1.0,1.0);
    BackMaterial = &Material::Default;
    FrontMaterial = &Material::Default;
    FaceNum = 0;
    MgrRecurseLevel = 0;
    SplitPatchA = 0;
    SplitPatchB = 0;
}

BezierPatch::BezierPatch( VectorR4 controlPts[4][4] )
{
    SetControlPoints( controlPts );
    UvMin.Set(0.0,0.0);
    UvMax.Set(1.0,1.0);
    BackMaterial = &Material::Default;
    FrontMaterial = &Material::Default;
    FaceNum = 0;
    MgrRecurseLevel = 0;
    SplitPatchA = 0;
    SplitPatchB = 0;
}

void BezierPatch::SetControlPoints( VectorR4 controlPts[4][4] )
{
    for (int i=0; i<4; i++ ) {
        for ( int j=0; j<4; j++ ) {
            CntlPts[i][j] = controlPts[i][j];
        }
    }
}


bool BezierPatch::HasPointAtInfinity()
{
    for (int i=0; i<4; i++ ) {
        for ( int j=0; j<4; j++ ) {
            if ( CntlPts[i][j].w==0.0 ) {
                return true;
            }
        }
    }
    return false;
}

void BezierPatch::DeCasteljauSplit( const VectorR4 in[4], VectorR4 out[7] )
{
    out[0] = in[0];
    out[6] = in[3];
    averageOf(in[0], in[1], out[1]);
    averageOf(in[2], in[3], out[5]);
    averageOf(in[1], in[2], out[3]);
    averageOf(out[1],out[3], out[2]);
    averageOf(out[3],out[5], out[4]);
    averageOf(out[2],out[4], out[3]);
}

void BezierPatch::DeCasteljauSplit( double alpha, const VectorR4 in[4], VectorR4 out[7] )
{
    out[0] = in[0];
    out[6] = in[3];
    Lerp(in[0], in[1], alpha, out[1]);
    Lerp(in[2], in[3], alpha, out[5]);
    Lerp(in[1], in[2], alpha, out[3]);
    Lerp(out[1],out[3], alpha, out[2]);
    Lerp(out[3],out[5], alpha, out[4]);
    Lerp(out[2],out[4], alpha, out[3]);
}

// Split patch into two in u direction
//		Does not calculate the bounding parallelepiped.
void BezierPatch::MakeSplitU ( BezierPatch* u0, BezierPatch* u1 )
{
    // control points
    VectorR4 inPts[4];
    VectorR4 outPts[7];
    for ( int j=0; j<4; j++ ) {
        int i;
        for ( i=0; i<4; i++ ) {
            inPts[i] = CntlPts[i][j];
        }
        DeCasteljauSplit( &inPts[0], &outPts[0] );
        for ( i=0; i<4; i++) {
            u0->CntlPts[i][j] = outPts[i];
            u1->CntlPts[i][j] = outPts[i+3];
        }
    }

    // uv coordinates & face num info & Material
    u0->FaceNum = FaceNum;
    u1->FaceNum = FaceNum;
    u0->UvMin = UvMin;
    double uHalf = (UvMin.x+UvMax.x)*0.5;
    u0->UvMax.Set(uHalf,UvMax.y);
    u1->UvMin.Set(uHalf,UvMin.y);
    u1->UvMax = UvMax;
    u0->BackMaterial = u1->BackMaterial = BackMaterial;
    u0->FrontMaterial = u1->FrontMaterial = FrontMaterial;
}

// Split patch into two in v direction
//		Does not calculate the bounding parallelepiped
void BezierPatch::MakeSplitV ( BezierPatch* u0, BezierPatch* u1 )
{
    // control points
    VectorR4 outPts[7];
    for ( int i=0; i<4; i++ ) {
        DeCasteljauSplit( &CntlPts[i][0], &outPts[0] );
        for ( int j=0; j<4; j++) {
            u0->CntlPts[i][j] = outPts[j];
            u1->CntlPts[i][j] = outPts[j+3];
        }
    }

    // uv coordinates & face num info & Material
    u0->FaceNum = FaceNum;
    u1->FaceNum = FaceNum;
    u0->UvMin = UvMin;
    double vHalf = (UvMin.y+UvMax.y)*0.5;
    u0->UvMax.Set(UvMax.x,vHalf);
    u1->UvMin.Set(UvMin.x,vHalf);
    u1->UvMax = UvMax;
    u0->BackMaterial = u1->BackMaterial = BackMaterial;
    u0->FrontMaterial = u1->FrontMaterial = FrontMaterial;
}

void BezierPatch::EvalPatch( const VectorR2& Uv, VectorR3* val) const
{
    VectorR4 valRational;
    EvalPatch ( Uv, &valRational );
    val->SetFromHg( valRational );
}

void BezierPatch::EvalPatch( const VectorR2& Uv, VectorR4* val) const
{
    VectorR4 outPts[7];
    VectorR4 midCntls[4];
    for ( int i=0; i<4; i++ ) {
        DeCasteljauSplit( Uv.y, &CntlPts[i][0], outPts );
        midCntls[i] = outPts[3];
    }
    DeCasteljauSplit(Uv.x, &midCntls[0], outPts);
    *val = outPts[3];
}

void BezierPatch::EvalPatchNormal( const VectorR2& Uv, const VectorR4& patchVal,
                                   VectorR3* val, const VectorR3* backupNormal) const
{
    // Get partial w.r.t. u
    VectorR4 outPts[7];
    VectorR4 midCntls[4];
    int i;
    for ( i=0; i<4; i++ ) {
        DeCasteljauSplit( Uv.y, &CntlPts[i][0], outPts );
        midCntls[i] = outPts[3];
    }
    VectorR3 partialU;
    EvalDeriv3( Uv.x, patchVal, &midCntls[0], &partialU );

    // Get partial w.r.t. v
    VectorR4 inPts[4];
    for ( int j=0; j<4; j++ ) {
        for ( i=0; i<4; i++ ) {
            inPts[i] = CntlPts[i][j];
        }
        DeCasteljauSplit( Uv.x, &inPts[0], &outPts[0] );
        midCntls[j] = outPts[3];
    }
    VectorR3 partialV;
    EvalDeriv3( Uv.y, patchVal, &midCntls[0], &partialV );

    *val = partialU;
    *val *= partialV;		// Cross product

    if ( val->IsZero() ) {
        if ( backupNormal != 0 ) {
            *val = *backupNormal;
        } else {
            assert(0);				// Shouldn't ever happen (?)
            val->SetUnitY();
        }
    } else {
        val->Normalize();					// Make into unit vector
    }

}

void BezierPatch::EvalDeriv3( double u, const VectorR4& evalVal,
                              const VectorR4 cntlPts[4], VectorR3* retVal )
{
    VectorR4 deriv4;
    EvalDeriv4( u, cntlPts, &deriv4 );
    retVal->Set( deriv4.x*evalVal.w - evalVal.x*deriv4.w,
                 deriv4.y*evalVal.w - evalVal.y*deriv4.w,
                 deriv4.z*evalVal.w - evalVal.z*deriv4.w );
    *retVal /= Square(evalVal.w);
}

bool BezierPatch::EvalDeriv4( double u, const VectorR4 cntlPts[4], VectorR4* retVal )
{
    VectorR4 cp3[3];
    VectorR4 temp, temp2;
    VectorR4 ret;
    int i;
    for ( i=0; i<3; i++ ) {
        cp3[i] = cntlPts[i+1];
        cp3[i] -= cntlPts[i];
    }
    Lerp( cp3[0], cp3[1], u, temp );
    Lerp( cp3[1], cp3[2], u, temp2 );
    Lerp( temp, temp2, u, *retVal );
    if ( !(retVal->NearZero(1.0e-25)) ) {
        ret *= 3.0;
        return true;
    }
    // First derivative was zero.  Try second derivative (L'Hopital's rule)
    VectorR4 cp2[2];
    for ( i=0; i<2; i++ ) {
        cp2[i] = cp3[i+1];
        cp2[i] -= cp3[i];
    }
    Lerp ( cp2[0], cp2[1], u, ret );
    if ( !(retVal->NearZero(1.0e-25)) ) {
        ret *= 6.0;
        return true;
    }
    // Second derivative zero too, try third derivative as a last resort
    ret = cp2[1];
    ret -= cp2[0];
    ret *= 6.0;
    if ( !(retVal->NearZero(1.0e-25)) ) {
        return true;
    } else {
        return false;
    }
}

void BezierPatch::BiLinearInvert( const VectorR3& pointU,
                                  const VectorR4& cornerX, const VectorR4& cornerY,
                                  const VectorR4& cornerZ, const VectorR4& cornerW,
                                  const VectorR3& Normal, VectorR2* UvValue )
{
    VectorR3 HgX, HgY, HgZ, HgW;
    HgX.SetFromHg(cornerX);
    HgY.SetFromHg(cornerY);
    HgZ.SetFromHg(cornerZ);
    HgW.SetFromHg(cornerW);

    VectorR3 S1;			// equals W-X
    VectorR3 S2;			// equals Z-Y
    VectorR3 UminX;			// equals U minus X
    VectorR3 UminY;			// equals U minus Y
    S1 = HgW;
    S1 -= HgX;
    S2 = HgZ;
    S2 -= HgY;
    UminX = pointU;
    UminX -= HgX;
    UminY = pointU;
    UminY -= HgY;

    double A, B, C;
    VectorR3 temp = S1;
    temp *= S2;				// Cross product
    A = temp^Normal;
    temp = S2;
    temp *= UminX;
    B = temp^Normal;
    temp = S1;
    temp *= UminY;
    B -= temp^Normal;
    temp = UminX;
    temp *= UminY;
    C = temp^Normal;

    double alpha, beta;
    double descriminant = sqrt( B*B - 4.0*A*C );
    if ( B>0 ) {
        beta = (-B-descriminant)/(2.0*A);
    } else {
        beta = 2.0*C/(-B+descriminant);
    }

    VectorR3& Sbeta1 = S1;			// Reuse these variables (dangerous!)
    VectorR3& Sbeta2 = S2;
    Lerp( HgX, HgW, beta, Sbeta1 );
    Lerp( HgY, HgZ, beta, Sbeta2 );
    temp = pointU;
    temp -= Sbeta1;
    Sbeta2 -= Sbeta1;
    alpha = (temp^Sbeta2)/(Sbeta2^Sbeta2);

    UvValue->Set(alpha,beta);
}

void ViewableBezierSet::CalcBoundingPlanes( const VectorR3& u, double *minDot, double *maxDot ) const
{
    double min = DBL_MAX;
    double max = -DBL_MAX;
    // Loop over each patch, find their bounding parallelepiped.
    for (size_t i = 0; i < PatchList.size(); i++ ) {
        const BezierPatch& bPatch = PatchList[i];
        double newMin, newMax;
        bPatch.GetMinMaxDot(u, &newMin, &newMax );
        if ( newMin<min ) {
            min = newMin;
        }
        if ( newMax>max ) {
            max = newMax;
        }
    }
    *minDot = min;
    *maxDot = max;
}

void BezierPatch::EvalFirstDeriv3( double u, const VectorR4& evalVal, const VectorR4 cntlPts[4],
                                   VectorR3* retVal )
{
    // evalVal is the value of the curve at u.

    // Get homogeneous derivative at u
    VectorR4 deriv4;
    EvalFirstDeriv4( u, cntlPts, &deriv4 );

    // Convert to non-homogenous derivatite
    retVal->Set( deriv4.x*evalVal.w - evalVal.x*deriv4.w,
                 deriv4.y*evalVal.w - evalVal.y*deriv4.w,
                 deriv4.z*evalVal.w - evalVal.z*deriv4.w );
    *retVal /= Square(evalVal.w);
}


// De Casteljau algorithm for evaluating first derivative.

void BezierPatch::EvalFirstDeriv4( double u, const VectorR4 cntlPts[4], VectorR4* retVal )
{
    VectorR4 cp3[3];
    VectorR4 temp, temp2;
    for ( int i=0; i<3; i++ ) {
        cp3[i] = cntlPts[i+1];
        cp3[i] -= cntlPts[i];
    }
    Lerp( cp3[0], cp3[1], u, temp );
    Lerp( cp3[1], cp3[2], u, temp2 );
    Lerp( temp, temp2, u, *retVal );
    return;
}
