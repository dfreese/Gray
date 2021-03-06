/*
 * Gray: A Ray Tracing-based Monte Carlo Simulator for PET
 *
 * Copyright (c) 2018, David Freese, Peter Olcott, Sam Buss, Craig Levin
 *
 * This software is distributed under the terms of the MIT License unless
 * otherwise noted.  See LICENSE for further details.
 *
 */

#include "Gray/KdTree/KdTree.h"
#include <cstdio>
#include <cassert>
#include <exception>
#include <functional>
#include <array>
#include <sstream>
#include "Gray/KdTree/DoubleRecurse.h"

// Destructor
KdTree::~KdTree()
{
}

long KdTree::Traverse(const VectorR3& startPos, const VectorR3& dir,
                      double & stopDistance, CallbackF ObjectCallback) const
{
    // Set sign of dir components and inverse values of non-zero entries.
    VectorR3 dirInv;
    int sign[3];
    double entryDist, exitDist;
    bool intersects = BoundingBox.RayIntersect(startPos, dir, dirInv,
                                               sign[0], sign[1], sign[2],
                                               0, DBL_MAX, entryDist, exitDist);
    if (!intersects) {
        return(-1);
    }
	// Main traversal loop

	long currentNodeIndex = RootIndex(); // The current node in the traversal
    const KdTreeNode* currentNode = &TreeNodes[currentNodeIndex];
    double minDistance = std::max(0.0, entryDist);
    double maxDistance = std::min(stopDistance, exitDist);
	bool hitParallel = false;
	double parallelHitMax = -DBL_MAX;
    long stopping_object = -1;

    // Use array as a stack.  This is a 30% speedup from std::stack.
    // Bounds checking is done during the tree construction since the stack
    // can't outgrow the depth of the tree.  Since the size is fixed to 63,
    // this would be an absolutely massive tree that would probably break other
    // things.
    //
    // Looked at using boost::container::small_vector, but this ended up not
    // gaining much.
    //
    // static thread_local keeps this array from being regenerated each time
    // the function is called, but makes it thread safe.
    static thread_local std::array<Kd_TraverseNodeData, traverse_stack_size> traverse_stack;
    int stack_size = 0;

	while ( true ) {
		if (currentNode->IsLeaf()) {
            // Handle leaf nodes by invoking the callback function
            // Pass the objects back to the user one at a time
            for (auto object: currentNode->Data.Leaf.Objects) {
                if (ObjectCallback(object, startPos, dir, stopDistance)) {
                    stopping_object = object;
                }
            }
		} else {
            // Handle non-leaf nodes
            //		These do not contain primitive objects.
            int thisSign;
            double thisDir;
            double thisDirInv;
            double thisStartPt;
            switch (currentNode->SplitAxis())
            {
                case KdTreeNode::KD_SPLIT_X:
                    thisSign = sign[0];
                    thisDir = dir.x;
                    thisDirInv = dirInv.x;
                    thisStartPt = startPos.x;
                    break;
                case KdTreeNode::KD_SPLIT_Y:
                    thisSign = sign[1];
                    thisDir = dir.y;
                    thisDirInv = dirInv.y;
                    thisStartPt = startPos.y;
                    break;
                case KdTreeNode::KD_SPLIT_Z:
                    thisSign = sign[2];
                    thisDir = dir.z;
                    thisDirInv = dirInv.z;
                    thisStartPt = startPos.z;
                    break;
                default:
                    throw std::runtime_error("Invalid split type");
                    break;
            }
            long nearNodeIdx;
            long farNodeIdx;
            if (thisDir == 0) {
                // Handle hitting exactly parallel to the splitting plane
                double thisSplitVal = currentNode->SplitValue();
                if ( thisSplitVal<thisStartPt ) {
                    currentNodeIndex = currentNode->RightChildIndex();
                }
                else if ( thisSplitVal>thisStartPt ) {
                    currentNodeIndex = currentNode->LeftChildIndex();
                }
                else {
                    // Exactly hit the splitting plane (not so good!)
                    long leftIdx = currentNode->LeftChildIndex();
                    long rightIdx = currentNode->RightChildIndex();
                    if ( leftIdx == -1 ) {
                        currentNodeIndex = rightIdx;
                    }
                    else if ( rightIdx == -1 ) {
                        currentNodeIndex = leftIdx;
                    }
                    else {
                        // Advance the current stack size after updating the
                        // last element.
                        traverse_stack[stack_size++] = {rightIdx,
                            minDistance,
                            maxDistance};
                        currentNodeIndex = leftIdx;
                        hitParallel = true;
                        UpdateMax(maxDistance,parallelHitMax);
                    }
                }
            } else {
                if (thisSign == 0) {
                    nearNodeIdx = currentNode->LeftChildIndex();
                    farNodeIdx = currentNode->RightChildIndex();
                } else {
                    nearNodeIdx = currentNode->RightChildIndex();
                    farNodeIdx = currentNode->LeftChildIndex();
                }
                double splitDistance = (currentNode->SplitValue() - thisStartPt) * thisDirInv;
                if ( splitDistance<minDistance ) {
                    // Far node is the new current node
                    currentNodeIndex = farNodeIdx;
                } else if ( splitDistance>maxDistance ) {
                    // Near node is the new current node
                    currentNodeIndex = nearNodeIdx;
                } else if ( nearNodeIdx == -1 ) {
                    minDistance = splitDistance;
                    currentNodeIndex = farNodeIdx;
                } else {
                    // Push the far node -- if it exists
                    if ( farNodeIdx != -1 ) {
                        // Advance the current stack size after updating the
                        // last element.
                        traverse_stack[stack_size++] = {farNodeIdx,
                            splitDistance,
                            maxDistance};
                    }
                    // Near node is the new current node
                    maxDistance = splitDistance;
                    currentNodeIndex = nearNodeIdx;
                }
            }
            if ( currentNodeIndex != -1 ) {
                currentNode = &TreeNodes[currentNodeIndex];
                continue;
            }
            // If we reach here, we are at an empty leaf and can fall through.
		}

		// Get to this point if done with a leaf node (possibly empty, possibly not).
		if (stack_size == 0) {
			return stopping_object;
		} else {
            Kd_TraverseNodeData& topNode = traverse_stack[--stack_size];
			minDistance = topNode.GetMinDist();
            if ((stopping_object >= 0) && (minDistance > stopDistance)) {
                if ( !hitParallel || minDistance>=parallelHitMax ) {
                    // Exit loop.  Fully done.
                    return stopping_object;
                }
            }
			currentNodeIndex = topNode.GetNodeNumber();
			currentNode = &TreeNodes[currentNodeIndex];
			maxDistance = topNode.GetMaxDist();
		}

	}

}


/***********************************************************************************************
 * Tree building functions.
 ***********************************************************************************************/
void KdTree::BuildTree(long numObjects,
                       std::function<void(long,AABB&)> ExtentFunc,
                       std::function<bool(long, const AABB&, AABB&)> ExtentInBoxFunc)
{
    if (TreeNodes.size() > 0) {
        return;
    }
    this->ExtentInBoxFunc = ExtentInBoxFunc;
    // If it's a perfect binary tree, in the worst case we need 2x nodes.
    TreeNodes.reserve(numObjects * 2);
	NumObjects = numObjects;

	// Get total cost of all objects
    TotalObjectCosts = ObjectConstantCost * NumObjects;


	// Allocate space for the AABB's for each object
	//	This is used only during the tree construction and is then released.
    ObjectAABBs.resize(numObjects);

	// Calculate all initial extents
	for (long ii = 0; ii < numObjects; ii++ ) {
		ExtentFunc(ii, ObjectAABBs[ii]);
	}

	// Pick the overall BoundingBox to enclose all the individual bounding boxes.
    for (auto aabb: ObjectAABBs) {
		BoundingBox.EnlargeToEnclose(aabb);
	}
	BoundingBoxSurfaceArea = BoundingBox.SurfaceArea();

	// Set up the initial extent lists
	ET_Lists = new ExtentTriple[ (3*2*ExtentTripleStorageMultiplier)*NumObjects ];
	if ( !ET_Lists ) {
		MemoryError();
	}
	ExtentTripleArrayInfo XextentList( ET_Lists, 0, 0 );
	ExtentTripleArrayInfo YextentList( ET_Lists + (2*ExtentTripleStorageMultiplier)*NumObjects, 0, 0 );
	ExtentTripleArrayInfo ZextentList( ET_Lists + (2*2*ExtentTripleStorageMultiplier)*NumObjects, 0, 0 );
	LeftRightStatus = new unsigned char[NumObjects];

	// Loop over all objects, creating the extent triples.
	for (long ii = 0; ii < numObjects; ii++ ) {
        const AABB & aabb = ObjectAABBs[ii];
		XextentList.AddToEnd(aabb.GetMinX(), aabb.GetMaxX(), ii);
		YextentList.AddToEnd(aabb.GetMinY(), aabb.GetMaxY(), ii);
		ZextentList.AddToEnd(aabb.GetMinZ(), aabb.GetMaxZ(), ii);
	}

	// Estimate upper bound on the space available.
	// Need this for memory management of ExtentTriple lists
	long spaceAvailable = 2*(ExtentTripleStorageMultiplier-1)*NumObjects;

	// Sort the triples
	XextentList.Sort();
	YextentList.Sort();
	ZextentList.Sort();

	// Recursively build the entire tree!
    long root_index = NextIndex();
    KdTreeNode& RootNode = TreeNodes.at(root_index);
	RootNode.ParentIdx = -1;				// No parent, it is the root node
	BuildSubTree ( RootIndex(), BoundingBox, TotalObjectCosts, XextentList, YextentList, ZextentList, spaceAvailable );

    // Quite lazily get the depth of the tree
    int tree_depth = 0;
    for (const auto & node: TreeNodes) {
        if (!node.IsLeaf()) {
            continue;
        }
        auto curr_node = &node;
        int depth = 0;
        while (!curr_node->IsRoot()) {
            ++depth;
            curr_node = &TreeNodes[curr_node->ParentIdx];
        }
        tree_depth = std::max(tree_depth, depth);
    }
    if (tree_depth > traverse_stack_size) {
        std::stringstream ss;
        ss << "KdTree is too deep for the fixed stack approach used here.  The"
           << "tree has a maximum depth of " << tree_depth << " but the stack"
           << "size if fixed to " << traverse_stack_size << ".";
        throw std::runtime_error(ss.str());
    }

    for (auto node: TreeNodes) {
        if (node.IsLeaf()) {
        }
    }
    // Could clear ObjectAABBs if memory was wanted.
	delete[] ET_Lists;
	delete[] LeftRightStatus;
    TreeNodes.shrink_to_fit();
}

// Recursively build a subtree.
// Pick a splitting point on one of the three axes
// Then call the routine recursively twice, once for each child
//		as appropriate
// spaceAvailable gives the amount of room for growth of the ExtentTripleLists.
void KdTree::BuildSubTree( long baseIndex, AABB& aabb, double totalObjectCost,
					ExtentTripleArrayInfo& xExtents, ExtentTripleArrayInfo& yExtents,
					ExtentTripleArrayInfo& zExtents, long spaceAvailable )
{

	VectorR3 deltaAABB = aabb.GetBoxMax();
	deltaAABB -= aabb.GetBoxMin();

	// Step 1.
	// Try all three axes to find the best split decision
    KdTreeNode::KD_SplittingAxis splitAxisID;
	ExtentTripleArrayInfo* splitExtentList;	// Will point to the split axis extext list
	double splitValue;				// Point where the split occurs
	long numTriplesToLeft;			// Number of triples on left side of split
	long numObjectsToLeft;			// Number of objects on left side of split
	long numObjectsToRight;			// Number of objects on right side of split
	double costObjectsToLeft;		// Total cost of objects on the left side of split
	double costObjectsToRight;		// Total cost of objects on the right side of split
	bool split = CalcBestSplit(aabb, deltaAABB, totalObjectCost, xExtents,
                               yExtents, zExtents, &splitAxisID, &splitValue,
                               &numTriplesToLeft, &numObjectsToLeft,
                               &numObjectsToRight, &costObjectsToLeft,
                               &costObjectsToRight);
    if (split) {
        switch (splitAxisID) {
            case KdTreeNode::KD_SPLIT_X: {
                splitExtentList = &xExtents;
            } break;
            case KdTreeNode::KD_SPLIT_Y: {
                splitExtentList = &yExtents;
            } break;
            case KdTreeNode::KD_SPLIT_Z: {
                splitExtentList = &zExtents;
            } break;
        }

    } else {
        // No splitting occurs
        // Copy object triples into an array
        KdTreeNode& baseNode = TreeNodes.at(baseIndex);
        baseNode.is_leaf = true;
        long numInLeaf = xExtents.NumObjects();
        assert ( yExtents.NumObjects() == numInLeaf && zExtents.NumObjects() == numInLeaf );
        baseNode.Data.Leaf.Objects.resize(numInLeaf);
        // Pick any one of the three axes
        ExtentTriple* triple = xExtents.TripleArray;
        for ( long i=0; i<numInLeaf; triple++ ) {
            if ( !( triple->IsMax() ) ) {
                baseNode.Data.Leaf.Objects[i] = triple->ObjectID;
                i++;
            }
        }
        // At a leaf, exit out of subtree build
        return;
    }

	// Step 2. Handle splits where one subtree is empty
	if ( numObjectsToLeft==0 || numObjectsToRight==0 ) {
		assert ( numObjectsToLeft!=0 || numObjectsToRight!=0 );
		// One child is empty
		long childIndex = NextIndex();		// WARNING: NextIndex() can trigger memory movement
		KdTreeNode& baseNode = TreeNodes.at(baseIndex);
		KdTreeNode& childNode = TreeNodes.at(childIndex);
		childNode.ParentIdx = baseIndex;
		baseNode.NodeType = splitAxisID;
		baseNode.Data.Split.SplitValue = splitValue;
		AABB childAabb(aabb);
		if ( numTriplesToLeft==0 ) {
			baseNode.Data.Split.LeftChildIdx = -1;
			baseNode.Data.Split.RightChildIdx = childIndex;
			childAabb.SetNewAxisMin( splitAxisID, splitValue );
		}
		else {
			baseNode.Data.Split.LeftChildIdx = childIndex;
			baseNode.Data.Split.RightChildIdx = -1;
			childAabb.SetNewAxisMax( splitAxisID, splitValue );
		}
		BuildSubTree( childIndex, childAabb, totalObjectCost,
						xExtents, yExtents, zExtents, spaceAvailable );
		return;
	}

	// Step 3.
	// Two subtrees must be formed.
	// Decide which objects go left and right - Store info in LeftRightStatus[]
	ExtentTriple* etPtr = splitExtentList->TripleArray;
	long i;
	for ( i=0; i<numTriplesToLeft; i++, etPtr++ ) {
		// It is on the left, don't know if on right yet, so set as not.
		LeftRightStatus[ etPtr->ObjectID ] = 1;			// Set first bit, reset second bit
	}
	long n = splitExtentList->NumTriples();
	for ( ; i<n; i++, etPtr++ ) {
		if ( etPtr->ExtentType == ExtentTriple::TT_MAX ) {
			// On right side.  Maybe on left side too.
			LeftRightStatus[ etPtr->ObjectID ] |= 2;		// Set second bit
		}
		else {
			// On right side only
			LeftRightStatus[ etPtr->ObjectID ] = 2;			// Set second bit, reset first bit
		}
	}

	// Step 4.  Build the child AABB's
	AABB leftChildAabb = aabb;
	leftChildAabb.SetNewAxisMax( splitAxisID, splitValue );
	AABB rightChildAabb = aabb;
	rightChildAabb.SetNewAxisMin( splitAxisID, splitValue );
	AABB* smallerChildAabb;
	AABB* largerChildAabb;

	// Step 5.
	// Allocate the left and right children
	// Set entries in baseNode for internal node
	// Set all other tree pointers. (Indices)
	long leftChildIndex = NextIndex();		// Warning: NextIndex() can trigger memory movement
	long rightChildIndex = NextIndex();
	KdTreeNode& baseNode = TreeNodes.at(baseIndex);
	KdTreeNode& leftChildNode = TreeNodes.at(leftChildIndex);
	KdTreeNode& rightChildNode = TreeNodes.at(rightChildIndex);
	baseNode.NodeType = splitAxisID;
	baseNode.Data.Split.LeftChildIdx = leftChildIndex;
	baseNode.Data.Split.RightChildIdx = rightChildIndex;
	baseNode.Data.Split.SplitValue = splitValue;
	leftChildNode.ParentIdx = baseIndex;
	rightChildNode.ParentIdx = baseIndex;
	long smallerChildIdx;
	long largerChildIdx;

	// Step 6: Build the extent lists for the two subtrees.
	unsigned char leftRightFlag;
	long newSpaceAvailable;		// Space available for extent lists for smaller subtree building
	double smallerTotalCost;		// total cost of objects in the subtree with fewer objects
	double largerTotalCost;		// total cost of objects in the other subtree
	long smallerNumObjects;
	long largerNumObjects;
	// First build the extent for the smaller of the two subtrees
	if (  numObjectsToLeft<numObjectsToRight ) {
		leftRightFlag = 1;
		newSpaceAvailable = spaceAvailable-2*numObjectsToLeft;
		smallerChildAabb = &leftChildAabb;
		largerChildAabb = &rightChildAabb;
		smallerChildIdx = leftChildIndex;
		largerChildIdx = rightChildIndex;
		smallerTotalCost = costObjectsToLeft;
		largerTotalCost = costObjectsToRight;
		smallerNumObjects = numObjectsToLeft;
		largerNumObjects = numObjectsToRight;
	}
	else {
		leftRightFlag = 2;
		newSpaceAvailable = spaceAvailable-2*numObjectsToRight;
		smallerChildAabb = &rightChildAabb;
		largerChildAabb = &leftChildAabb;
		smallerChildIdx = rightChildIndex;
		largerChildIdx = leftChildIndex;
		smallerTotalCost = costObjectsToRight;
		largerTotalCost = costObjectsToLeft;
		smallerNumObjects = numObjectsToRight;
		largerNumObjects = numObjectsToLeft;
	}
	if ( newSpaceAvailable<0 ) {
		MemoryError2();
	}
	// Step 7.  Allocate space for the smaller subtree
	ExtentTripleArrayInfo newXextents( xExtents.EndOfArray, 0, 0 );
	ExtentTripleArrayInfo newYextents( yExtents.EndOfArray, 0, 0 );
	ExtentTripleArrayInfo newZextents( zExtents.EndOfArray, 0, 0 );
	// Create the AABB's for the smaller subtree
	MakeAabbsForSubtree( leftRightFlag, xExtents, *smallerChildAabb );
	// Copy the extent triples for the smaller subtree
	CopyTriplesForSubtree( leftRightFlag, 0, xExtents, newXextents );
	CopyTriplesForSubtree( leftRightFlag, 1, yExtents, newYextents );
	CopyTriplesForSubtree( leftRightFlag, 2, zExtents, newZextents );
	// Recalculate total cost if necessary, i.e., if some objects go missing
	if ( newXextents.NumObjects()!=smallerNumObjects ) {
		smallerTotalCost = CalcTotalCosts( newXextents );
	}

	// Step 8.
	leftRightFlag = 3-leftRightFlag;
	// Create the AABB's for the larger subtree
	MakeAabbsForSubtree( leftRightFlag, xExtents, *largerChildAabb );
	// Copy the extent triples for the larger subtree
	CopyTriplesForSubtree( leftRightFlag, 0, xExtents, xExtents );
	CopyTriplesForSubtree( leftRightFlag, 1, yExtents, yExtents );
	CopyTriplesForSubtree( leftRightFlag, 2, zExtents, zExtents );
	leftRightFlag = 3-leftRightFlag;		// Reset to smaller subtree again
	// Recalculate total cost if necessary, i.e., if some objects go missing
	if ( xExtents.NumObjects()!=largerNumObjects ) {
		largerTotalCost = CalcTotalCosts( xExtents );
	}

	// Step 9.
	// Invoke BuildSubTree recursively for the two subtrees
	BuildSubTree(smallerChildIdx, *smallerChildAabb, smallerTotalCost,
					newXextents, newYextents, newZextents, newSpaceAvailable);
	BuildSubTree(largerChildIdx, *largerChildAabb, largerTotalCost,
					xExtents, yExtents, zExtents, spaceAvailable );

}

bool KdTree::CalcBestSplit(const AABB& aabb, const VectorR3& deltaBox,
                           double totalObjectCost,
                           const ExtentTripleArrayInfo& xExtents,
                           const ExtentTripleArrayInfo& yExtents,
                           const ExtentTripleArrayInfo& zExtents,
                           KdTreeNode::KD_SplittingAxis* splitAxisID,
                           double* splitValue, long* numTriplesToLeft,
                           long* numObjectsToLeft, long* numObjectsToRight,
                           double* costObjectsToLeft,
                           double* costObjectsToRight)
{
	assert( xExtents.NumObjects() == yExtents.NumObjects() );
	assert( yExtents.NumObjects() == zExtents.NumObjects() );

    // Default is no split, unless find a better option.

	double surfaceArea = 2.0*(deltaBox.x*deltaBox.y + deltaBox.x*deltaBox.z +
                              deltaBox.y*deltaBox.z);
	double minImprove = (BoundingBoxSurfaceArea/surfaceArea)*StoppingCostPerRay;
	double costToBeat = totalObjectCost - minImprove;
	if ( costToBeat<=1.0+1.0e-7 ) {
        // There is no way to improve enough to bother.
		return(false);
	}

	// Try each of the three axes in turn.
	bool foundBetter = false;
	double bestCostSoFar = totalObjectCost;
	if ( CalcBestSplit( totalObjectCost, costToBeat, xExtents,
						aabb.GetMinX(), aabb.GetMaxX(), deltaBox.y, deltaBox.z,
						&bestCostSoFar, splitValue,
						numTriplesToLeft, numObjectsToLeft, numObjectsToRight,
						costObjectsToLeft, costObjectsToRight ) )
	{
		foundBetter = true;
		*splitAxisID = KdTreeNode::KD_SPLIT_X;
		costToBeat = bestCostSoFar;
	}
	if ( CalcBestSplit( totalObjectCost, costToBeat, yExtents,
						aabb.GetMinY(), aabb.GetMaxY(), deltaBox.x, deltaBox.z,
						&bestCostSoFar, splitValue,
						numTriplesToLeft, numObjectsToLeft, numObjectsToRight,
						costObjectsToLeft, costObjectsToRight ) )
	{
		foundBetter = true;
		*splitAxisID = KdTreeNode::KD_SPLIT_Y;
		costToBeat = bestCostSoFar;
	}
	if ( CalcBestSplit( totalObjectCost, costToBeat, zExtents,
						aabb.GetMinZ(), aabb.GetMaxZ(), deltaBox.x, deltaBox.y,
						&bestCostSoFar, splitValue,
						numTriplesToLeft, numObjectsToLeft, numObjectsToRight,
						costObjectsToLeft, costObjectsToRight ) )
	{
		foundBetter = true;
		*splitAxisID = KdTreeNode::KD_SPLIT_Z;
	}
    return(foundBetter);
}

// Returns true if a new better split is found on the axis.
// The other return values will NOT be changed unless "true" is returned.
bool KdTree::CalcBestSplit( double totalObjectCosts, double costToBeat,
						const ExtentTripleArrayInfo& extents,
						double minOnAxis, double maxOnAxis,
						double secondAxisLen, double thirdAxisLen,
						double* retNewBestCost, double* retSplitValue,
						long* retNumTriplesToLeft, long* retNumObjectsToLeft, long* retNumObjectsToRight,
						double* retCostObjectsToLeft, double* retCostObjectsToRight )
{
	if ( minOnAxis>=maxOnAxis ) {
		return false;		// We do not support splitting a zero length axis.
	}

	InitSplitCostFunction( minOnAxis, maxOnAxis, secondAxisLen, thirdAxisLen,
							costToBeat, totalObjectCosts );

	bool foundBetter = false;
	double bestCost = costToBeat;			// Cost to beat
	long numTriplesLeft = 0;				// number of triples processed so far
	long numTriples = extents.NumTriples(); // total number of triples
	long numObjectsLeft = 0;					// number of ojects on the left during the scan
	long numObjectsRight = extents.NumObjects();	// number of objects on the right during the scan
	double costLeft = 0.0;					// cost of objects on the left during the scan
	double costRight = totalObjectCosts;	// total cost of objects still on the right side during scan
	ExtentTriple* etPtr = extents.TripleArray;
	bool inFirstHalf = true;			// If still scanning first half, measured in distance along axis
	double midPoint = 0.5*(minOnAxis+maxOnAxis);
	while ( numTriplesLeft<numTriples ) {
		// The split can occur either right before or right after the split value.
		double thisSplitValue = etPtr->ExtentValue;
		bool sameSplitValue = true;
		ExtentTriple::TripleType thisType = etPtr->ExtentType;
		if ( inFirstHalf && thisSplitValue>midPoint ) {
			inFirstHalf = false;
		}

		// Skip past all TT_MAX's, and TT_FLAT's in first half, with this split value
		while ( numTriplesLeft<numTriples ) {
			if ( !( ((thisType==ExtentTriple::TT_MAX
						|| (thisType==ExtentTriple::TT_FLAT && inFirstHalf)))
					&& sameSplitValue ) )
			{
				break;
			}
            UpdateLeftRightCosts( *etPtr, &numObjectsLeft, &numObjectsRight, &costLeft, &costRight );
			etPtr++;
			numTriplesLeft++;
			thisType = etPtr->ExtentType;
			sameSplitValue = (etPtr->ExtentValue <= thisSplitValue);
		}

		// Ready to call the cost function
		// If the cost function gives better value, save everything appropriately
		if ( CalcSplitCost( thisSplitValue, costLeft, costRight, &bestCost ) ) {
			foundBetter = true;
			*retNewBestCost = bestCost;
			*retSplitValue = thisSplitValue;
			*retNumTriplesToLeft = numTriplesLeft;
			*retNumObjectsToLeft = numObjectsLeft;
			*retNumObjectsToRight = numObjectsRight;
			*retCostObjectsToLeft = costLeft;
			*retCostObjectsToRight = costRight;
		}

		// Skip past all TT_MIN's, and TT_FLAT's in second half, with this split value
		while ( numTriplesLeft<numTriples ) {
			if ( !( sameSplitValue &&
					(thisType==ExtentTriple::TT_MIN
					|| (thisType==ExtentTriple::TT_FLAT && !inFirstHalf)) ) )
			{
				break;
			}
			// Move rightward a triple
			UpdateLeftRightCosts( *etPtr, &numObjectsLeft, &numObjectsRight, &costLeft, &costRight );
			etPtr++;
			numTriplesLeft++;
			thisType = etPtr->ExtentType;
			sameSplitValue = (etPtr->ExtentValue <= thisSplitValue);
		}

	}

	return foundBetter;
}

void KdTree::UpdateLeftRightCosts( const ExtentTriple& et, long* numObjectsLeft, long* numObjectsRight,
							   double *costLeft, double *costRight )
{
	double cost = ObjectConstantCost;
	switch ( et.ExtentType ) {
	case ExtentTriple::TT_MAX:
		(*numObjectsRight) --;
		(*costRight) -= cost;
		break;
	case ExtentTriple::TT_FLAT:
		(*numObjectsRight)--;
		(*costRight) -= cost;
		(*numObjectsLeft)++;
		(*costLeft) += cost;
		break;
	case ExtentTriple::TT_MIN:
		(*numObjectsLeft)++;
		(*costLeft) += cost;
		break;
	}
}


// Create the Aabb's for one of the subtrees
void KdTree::MakeAabbsForSubtree(unsigned char leftRightFlag,
                                 const ExtentTripleArrayInfo& theExtents,
                                 const AABB& theAabb)
{
	ExtentTriple* etPtr = theExtents.TripleArray;
	long i;
	long n = theExtents.NumTriples();
	for ( i=0; i<n; i++, etPtr++ ) {
		long objectID = etPtr->ObjectID;
		if ( (LeftRightStatus[ objectID ] & leftRightFlag) != 0 ) {
			// Don't bother if a Max on the left, or a Min on the right.
			//		In these cases, the extent will be computed anyway
			if ( !((etPtr->ExtentType==(ExtentTriple::TT_MIN) && leftRightFlag==2)
				|| (etPtr->ExtentType==(ExtentTriple::TT_MAX) && leftRightFlag==1)) )
			{
				assert ( 0<=objectID && objectID<NumObjects );
				bool stillIn = ExtentInBoxFunc(objectID, theAabb, ObjectAABBs[objectID]);
				bool flatX = ObjectAABBs[objectID].IsFlatX();
				bool flatY = ObjectAABBs[objectID].IsFlatY();
				bool flatZ = ObjectAABBs[objectID].IsFlatZ();
				if ( !stillIn ||(flatX&&flatY) || (flatY&&flatZ) || (flatX&&flatZ) ) {
					// Remove from being in this subtree (bitwise OR with complement of leftRightFlag)
					LeftRightStatus[objectID] &= ~leftRightFlag;
				}
			}
		}
	}
}


// Selectively copy and recalculate extent triples for a sub tree.
//	leftRightFlag is 1 or 2 depending on whether this is for the left
//		or right subtree.
//	The Aabb's have already been set correctly for the new subtree.
//  The new triples are created and copied in an order that promises to
//		be as sorted as possible, but they are not fully sorted yet.
//		After that, they are ShellSorted.
void KdTree::CopyTriplesForSubtree( unsigned char leftRightFlag, int axisNumber,
											ExtentTripleArrayInfo& fromExtents,
											ExtentTripleArrayInfo& toExtents )
{
	ExtentTriple* fromET = fromExtents.TripleArray;
	ExtentTriple* toET = toExtents.TripleArray;
	long n = fromExtents.NumTriples();			// Number of "from" items left
	long iF = 0;								// Number of "to" flats created
	long iM = 0;								// Number of "to" max/mins created
	for ( ; n>0; n--, fromET++ ) {
		long objectID = fromET->ObjectID;
		assert ( 0<=objectID && objectID<NumObjects );
		if ( LeftRightStatus[objectID] & leftRightFlag ) {
			toET->ObjectID = objectID;
			toET->ExtentType = fromET->ExtentType;
			switch ( fromET->ExtentType )
			{
			case ExtentTriple::TT_MIN:
				{
					const AABB& theAABB = ObjectAABBs[objectID];
					double newMinExtent = theAABB.GetBoxMin()[axisNumber];
					double newMaxExtent = theAABB.GetBoxMax()[axisNumber];
					toET->ExtentValue = newMinExtent;
					assert ( newMinExtent<=newMaxExtent );
					if ( newMinExtent < newMaxExtent ) {
						iM++;
					}
					else {
						toET->ExtentType = ExtentTriple::TT_FLAT;
						iF++;
					}
				}
				toET++;
				break;
			case ExtentTriple::TT_FLAT:
				toET->ExtentValue = fromET->ExtentValue;
				iF++;
				toET++;
				break;
			case ExtentTriple::TT_MAX:
				{
					const AABB& theAABB = ObjectAABBs[objectID];
					double newMinExtent = theAABB.GetBoxMin()[axisNumber];
					double newMaxExtent = theAABB.GetBoxMax()[axisNumber];
					toET->ExtentValue = newMaxExtent;
					assert ( newMinExtent<=newMaxExtent );
					if ( newMinExtent < newMaxExtent ) {
						iM++;
						toET++;
					}
					// If newMinExtent>=newMaxExtent, then this was changed to a flat already.
				}
				break;
			default:
				assert(0);
			}
		}
	}
	assert ( (iM&0x01) == 0 );
	toExtents.SetNumbers( iM>>1, iF );

	// Now sort the new array of triples
	toExtents.Sort();
}

double KdTree::CalcTotalCosts( const ExtentTripleArrayInfo& extents ) const
{
	return (ObjectConstantCost * extents.NumObjects());
}


// ****************************************************************************
// Code for cost functions
// ****************************************************************************
inline void KdTree::InitSplitCostFunction( double minOnAxis, double maxOnAxis,
											double secondAxisLen, double thirdAxisLen,
											double costToBeat, double totalObjectCosts )
{
	switch ( SplitAlgorithm ) {
	case MacDonaldBooth:
	case MacDonaldBoothModifiedCoefs:
		// MacDonald-Booth method
		InitMacdonaldBooth( minOnAxis, maxOnAxis, secondAxisLen, thirdAxisLen,
							costToBeat,totalObjectCosts);
		break;
	case DoubleRecurseGS:
	case DoubleRecurseModifiedCoefs:
		InitDoubleRecurse( minOnAxis, maxOnAxis, secondAxisLen, thirdAxisLen,
							 costToBeat, totalObjectCosts );
		break;
	}
}

inline void KdTree::InitMacdonaldBooth( double minOnAxis, double maxOnAxis,
										double secondAxisLen, double thirdAxisLen,
										double costToBeat, double totalObjectCosts )
{
	CF_MinOnAxis = minOnAxis;
	CF_MaxOnAxis = maxOnAxis;
	CF_FirstAxisLenInv = 1.0/(maxOnAxis-minOnAxis);
	CF_OldCost = costToBeat;
	CF_TotalNodeObjectCosts = totalObjectCosts;
	CF_EndArea = secondAxisLen*thirdAxisLen;
	CF_Wrap = 2.0*(maxOnAxis-minOnAxis)*(secondAxisLen+thirdAxisLen);
	CF_Area = 2.0*CF_EndArea + CF_Wrap;
}

inline void KdTree::InitDoubleRecurse( double minOnAxis, double maxOnAxis,
										double secondAxisLen, double thirdAxisLen,
										double costToBeat, double totalObjectCosts )
{
	CF_MinOnAxis = minOnAxis;
	CF_MaxOnAxis = maxOnAxis;
	CF_FirstAxisLenInv = 1.0/(maxOnAxis-minOnAxis);
	CF_OldCost = costToBeat;
	CF_TotalNodeObjectCosts = totalObjectCosts;
	CF_LogTNOCinv = 1.0/log(CF_TotalNodeObjectCosts);
	CF_EndArea = secondAxisLen*thirdAxisLen;
	CF_Wrap = 2.0*(maxOnAxis-minOnAxis)*(secondAxisLen+thirdAxisLen);
	CF_Area = 2.0*CF_EndArea + CF_Wrap;

	// Calculate double recurse cost exponent to beat
	if ( CF_EndArea > 1.0e-14*CF_Area ) {
		CF_D = -CF_Area/(2.0*CF_EndArea);
		CF_C = 1.0 - CF_D;
        CF_ExponentToBeat = log((costToBeat-CF_D)/CF_C) * CF_LogTNOCinv;
        // Set the lower bound to allow zero, as sometimes the scale between
        // costToBeat and CF_D are drastically different.
        assert((0 <= CF_ExponentToBeat) && (CF_ExponentToBeat < 1.0));
	}
	else {
		CF_EndArea = 0.0;		// End area is small enough to treat as being exactly zero
	}
}

bool KdTree::CalcSplitCost( double splitValue, double costLeft, double costRight, double* retCost )
{
	switch ( SplitAlgorithm ) {
	case MacDonaldBooth:
		// MacDonald-Booth method
		return CalcMacdonaldBooth( splitValue, costLeft, costRight, retCost );
	case MacDonaldBoothModifiedCoefs:
		// MacDonald-Booth method with modified coefs
		return CalcMacdonaldBoothModifiedCoefs( splitValue, costLeft, costRight, retCost );
	case DoubleRecurseGS:
	case DoubleRecurseModifiedCoefs:
		// Buss double-recurse method
		return CalcDoubleRecurseGS( splitValue, costLeft, costRight, retCost );
		break;
	default:
		assert(0);
		return false;
	}
}

bool KdTree::CalcMacdonaldBooth( double splitValue, double costLeft, double costRight, double* retCost )
{
	double gamma = (splitValue-CF_MinOnAxis)*CF_FirstAxisLenInv;
	double surfaceAreaLeft = 2.0*CF_EndArea + gamma*CF_Wrap;
	double surfaceAreaRight = 2.0*CF_EndArea + (1.0-gamma)*CF_Wrap;
	double newCost = 1.0 + (surfaceAreaLeft*costLeft + surfaceAreaRight*costRight)/CF_Area;
	if ( newCost<CF_OldCost ) {
		*retCost = newCost;
		CF_OldCost = newCost;
		return true;
	}
	else {
		return false;
	}
}

bool KdTree::CalcMacdonaldBoothModifiedCoefs( double splitValue, double costLeft, double costRight, double* retCost )
{
	double gamma = (splitValue-CF_MinOnAxis)*CF_FirstAxisLenInv;
	double surfaceAreaLeft = 2.0*CF_EndArea + gamma*CF_Wrap;
	double surfaceAreaRight = 2.0*CF_EndArea + (1.0-gamma)*CF_Wrap;
	double modFade = CF_TotalNodeObjectCosts/TotalObjectCosts;
	double fracLeft = costLeft/(costLeft+costRight);
	double fracRight = 1.0-fracLeft;
	double newCost = 1.0;
	newCost += (1.0-modFade)*(surfaceAreaLeft*costLeft + surfaceAreaRight*costRight)/CF_Area;
	newCost += modFade*((fracLeft+fracRight*CF_EndArea/surfaceAreaRight)*costLeft
						+(fracRight+fracLeft*CF_EndArea/surfaceAreaLeft)*costRight);

	if ( newCost<CF_OldCost ) {
		*retCost = newCost;
		CF_OldCost = newCost;
		return true;
	}
	else {
		return false;
	}
}

bool KdTree::CalcDoubleRecurseGS( double splitValue, double costLeft, double costRight, double* retCost )
{
	double gamma = (splitValue-CF_MinOnAxis)*CF_FirstAxisLenInv;
	double surfaceAreaLeft = 2.0*CF_EndArea + gamma*CF_Wrap;
	double surfaceAreaRight = 2.0*CF_EndArea + (1.0-gamma)*CF_Wrap;
	double A = surfaceAreaLeft/CF_Area;
	double B = surfaceAreaRight/CF_Area;
	double alpha = costLeft/CF_TotalNodeObjectCosts;
	double beta = costRight/CF_TotalNodeObjectCosts;
	if ( SplitAlgorithm==DoubleRecurseModifiedCoefs ) {
		double modFade = CF_TotalNodeObjectCosts/TotalObjectCosts;
		double fracLeft = costLeft/(costLeft+costRight);
		double fracRight = 1.0-fracLeft;
		A = Lerp(A, fracLeft+fracRight*CF_EndArea/surfaceAreaRight, modFade);
		B = Lerp(B, fracRight+fracLeft*CF_EndArea/surfaceAreaLeft, modFade);
	}

	if ( costLeft==0.0 || costRight==0.0 ) {
		double newCost;
		if ( costLeft==0.0 && B<1.0 ) {
			newCost = 1.0/(1.0-B);
		}
		else if ( costRight==0.0 && A<1.0 ) {
			newCost = 1.0/(1.0-A);
		}
		else {
			return false;
		}
		if ( newCost<CF_OldCost ) {
			*retCost = newCost;
            CF_OldCost = newCost;
            if ( CF_EndArea!=0.0 ) {
                CF_ExponentToBeat = log( (newCost-CF_D)/CF_C ) * CF_LogTNOCinv;
                // Set the lower bound to allow zero, as sometimes the scale
                // between costToBeat and CF_D are drastically different.
                assert((0 <= CF_ExponentToBeat) && (CF_ExponentToBeat < 1.0));
            }
            return true;
		}
		return false;
	}

	if ( CF_EndArea==0.0 ) {
		if ( alpha!=0.0 && beta!=0.0 ) {
			double newCost = 1.0 - log(CF_TotalNodeObjectCosts)/(A*log(alpha)+B*log(beta));
			if ( newCost<CF_OldCost ) {
				*retCost = newCost;
				CF_OldCost = newCost;
				return true;
			}
		}
		return false;
	}

	double C, D;
	double newExponent;
	bool betterCost = FindDoubleRecurseSoln( A, B, alpha, beta, &C, &newExponent, &D, CF_ExponentToBeat );
	if ( betterCost ) {
		CF_ExponentToBeat = newExponent;
		assert ( 0<CF_ExponentToBeat && CF_ExponentToBeat<1.0 );
		CF_OldCost = C * pow(CF_TotalNodeObjectCosts,newExponent) + D;
		*retCost = CF_OldCost;
		return true;
	}

	return false;
}

void KdTree::MemoryError()
{
	assert(0);
	fprintf(stderr,"KdTree construction: Failed to allocate memory.\n");
	exit(0);
}

void KdTree::MemoryError2()
{
	assert(0);
	fprintf(stderr,"KdTree: Memory overflow. Need to increase storage multiplier.\n");
	exit(0);
}
