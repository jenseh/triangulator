#pragma once

#include "happah/triangulators/PlanarGraphTriangulator.h"
#include "happah/HappahConstants.h"
#include "happah/triangulators/SeidelTriangulation/Trapezoid.h"
#include "happah/triangulators/SeidelTriangulation/Node.h"
#include <algorithm>

/**
* Offers triangulation functionalty of given polygons according to Seidel's algorithm
*/
class PlanarGraphTriangulatorSeidel : public PlanarGraphTriangulator {

public:
	

    /**
	 * trapezoidulate
	 *
	 * @access static
	 * @param PlanarGraphSegmentEndpointsIterator first
	 * @param PlanarGraphSegmentEndpointsIterator last
	 * @return TrapezoidMesh2D*
	 *
	 * @brief Trapezoidulates a given amount of SegmentEndpoints into a TrapezoidMesh2D* and
	 *          returns trapezoidal decomposition of the interior of the given polygon according to Seidel's algorithm
	 */
	template<class PlanarGraphSegmentEndpointsIterator>
	static SeidelTriangulation::TrapezoidMesh2D* trapezoidulate(PlanarGraphSegmentEndpointsIterator first, PlanarGraphSegmentEndpointsIterator last) {
	// Template Implementation must be inside the header file
	// See: last section of http://www.cplusplus.com/doc/tutorial/templates/
	
		using namespace SeidelTriangulation;
		
		Trapezoid::currentID = 0;

		TrapezoidMesh2D* root = new TrapezoidMesh2D();

		for(auto it = first; it != last; it++) {
			SegmentEndpoints2D* segment = new SegmentEndpoints2D(*it);
			root->storeSegment(segment);// storage for deletion
			
			hpvec2 upper = (segment->a.y > segment->b.y)
							? segment->a
							: segment->b;
			hpvec2 lower = (segment->a.y <= segment->b.y)
							? segment->a
							: segment->b;
			Trapezoid* current;
			Trapezoid* end;
			Trapezoid* next;
			
			// get Trapezoid containing upper and to be vertically splitted
			Trapezoid* upperTrapezoid = root->getTrapezoid(upper);
			if(HP_EPSILON > glm::length(upperTrapezoid->upperLeftVertex - upper) && 
				lower.x < getSegment2DX(lower.y, upperTrapezoid->leftSegment)) {
	
				upperTrapezoid = upperTrapezoid->upperLeft->lowerLeft;
			}
			if(HP_EPSILON > glm::length(upperTrapezoid->upperRightVertex - upper) && 
				lower.x > getSegment2DX(lower.y, upperTrapezoid->rightSegment)) {
				if(upperTrapezoid->upperRight) {
					upperTrapezoid = upperTrapezoid->upperRight->lowerRight;
				} else {
					upperTrapezoid = upperTrapezoid->upperLeft->lowerRight;
				}
			}
			
			// split at upper if necessary
			if( (upperTrapezoid->upperLeftVertex.y != upper.y || 
					HP_EPSILON < glm::abs(upperTrapezoid->upperLeftVertex.x - upper.x))
				&& (upperTrapezoid->upperRightVertex.y != upper.y || 
					HP_EPSILON < glm::abs(upperTrapezoid->upperRightVertex.x - upper.x))
			
				&& (!upperTrapezoid->upperLeft || upperTrapezoid->upperLeft->lowerRightVertex.y != upper.y || 
					HP_EPSILON < glm::abs(upperTrapezoid->upperLeft->lowerRightVertex.x - upper.x))) { 
	
				current = upperTrapezoid->splitHorizontal(upper);
			} else {
				current = upperTrapezoid;
			}
			
			// get Trapezoid containing lower and split if necessary
			Trapezoid* lowerTrapezoid = root->getTrapezoid(lower);
			if((lowerTrapezoid->upperLeftVertex.y != lower.y || 
					HP_EPSILON < glm::abs(lowerTrapezoid->upperLeftVertex.x - lower.x))
				&& (lowerTrapezoid->upperRightVertex.y != lower.y || 
					HP_EPSILON < glm::abs(lowerTrapezoid->upperRightVertex.x - lower.x))
				&& (lowerTrapezoid->upperLeft->lowerRightVertex.y != lower.y || 
					HP_EPSILON < glm::abs(lowerTrapezoid->upperLeft->lowerRightVertex.x - lower.x))) { 
	
				end = lowerTrapezoid->splitHorizontal(lower);
			} else {
				end = lowerTrapezoid;
			}
			
			// if current has been splitted get Trapezoid above split
			if(current == lowerTrapezoid) {
				current = end->upperLeft;
			}
			
			
			printf("Splitting from %d to %d\n", Trapezoid::getID(current), Trapezoid::getID(end));
			
			// loop invariant: current is unlinked from its upper neighbors, these (temporarily up to three) are stored in last...
			// last... is filled from left to right
			Trapezoid* lastLeft = current->upperLeft;
			Trapezoid* lastMiddle = current->upperRight;
			Trapezoid* lastRight = NULL;
			
			if(lastLeft) Trapezoid::unlink(lastLeft, current);
			if(lastMiddle) Trapezoid::unlink(lastMiddle, current);
			
			while(current->upperLeftVertex.y != lower.y) {
				next = current->getNextAlongSegment(segment);
				
				Trapezoid* left = new Trapezoid();
				Trapezoid* right = new Trapezoid();
				printf("l %d, r %d replace %d\n", Trapezoid::getID(left), Trapezoid::getID(right), Trapezoid::getID(current));
	
				XNode* xNode = current->getSink()->splitVertical(segment, left, right);
				
				double lowerY = current->lowerLeftVertex.y;
				double upperY = current->upperLeftVertex.y;
	
				left->lowerLeftVertex = current->lowerLeftVertex;
				left->upperLeftVertex = current->upperLeftVertex;
				left->lowerRightVertex = hpvec2(getSegment2DX(lowerY, segment), lowerY);
				left->upperRightVertex = hpvec2(getSegment2DX(upperY, segment), upperY);
				
				right->lowerRightVertex = current->lowerRightVertex;
				right->upperRightVertex = current->upperRightVertex;
				right->lowerLeftVertex = hpvec2(getSegment2DX(lowerY, segment), lowerY);
				right->upperLeftVertex = hpvec2(getSegment2DX(upperY, segment), upperY);
				
				left->leftSegment = current->leftSegment;
				left->rightSegment = segment;
				right->leftSegment = segment;
				right->rightSegment = current->rightSegment;
				
				// try to link left and right to the last...
				// possibility checks will be done in link()
				Trapezoid::link(lastLeft, left);
				Trapezoid::link(lastMiddle, left);
				Trapezoid::link(lastLeft, right);
				Trapezoid::link(lastMiddle, right);
				Trapezoid::link(lastRight, right);
				
				Trapezoid* uc = current->upperCornerNeighbor;
				if(uc) {
					Trapezoid::unlink(uc, current);
					Trapezoid::link(uc, left);
					Trapezoid::link(uc, right);
				}
	
				Trapezoid* lc = current->lowerCornerNeighbor;
				if(lc) {
					Trapezoid::unlink(current, lc);
					Trapezoid::link(left, lc);
					Trapezoid::link(right, lc);
				}
	
				// restore loop invariant for next round
				if(next->upperLeft == current) {
					lastLeft = left;
					lastMiddle = right;
					lastRight = next->upperRight;
				} else {
					lastLeft = next->upperLeft;
					lastMiddle = left;
					lastRight = right;				
				}
				
				Trapezoid* ll = current->lowerLeft;
				Trapezoid* lr = current->lowerRight;
				
				Trapezoid::unlink(current, ll);
				Trapezoid::unlink(current, lr);
				
				if(ll == end || lr == end) {
					Trapezoid::link(left, ll);
					Trapezoid::link(right, ll);				
					Trapezoid::link(left, lr);
					Trapezoid::link(right, lr);
				} else if(ll == next) {
					Trapezoid::link(right, lr);
				} else if(lr == next) {
					Trapezoid::link(left, ll);
				}
				
				delete current;
				
				current = next;
			}
		}
	
		root->treeMerge();
		
		printf("merging done\n");
		
		root->getTrapezoid(hpvec2(INFINITY, INFINITY))->flagOutside(true);

		printf("flagging done\n");
	
		return (TrapezoidMesh2D*)root;
	}
    
    /**
	 * triangulate
	 *
	 * @access static
	 * @param PlanarGraphSegmentEndpointsIterator first Iterator with first SegmentEndpoints2D as starting point
	 * @param PlanarGraphSegmentEndpointsIterator last Iterator with last SegmentEndpoints2D as ending point
	 * @return TriangleMesh2D* a triangulized mesh
	 *
	 * @brief Triangulates a given amount of SegmentEndpoints into a TriangleMesh2D*
	 */
	template<class PlanarGraphSegmentEndpointsIterator>
	static TriangleMesh2D* triangulate(PlanarGraphSegmentEndpointsIterator first, PlanarGraphSegmentEndpointsIterator last)  {
	// Template Implementation must be inside the header file
	// See: last section of http://www.cplusplus.com/doc/tutorial/templates/
	
		using namespace SeidelTriangulation;
		
		TrapezoidMesh2D* trapezoidMesh = trapezoidulate(first, last);
		if(!trapezoidMesh) {
			return NULL;
		}
		TriangleMesh2D* triangleMesh = trapezoidMesh->toTriangleMesh();
		if (trapezoidMesh) {
			delete trapezoidMesh;
		}
		return triangleMesh;

	}
};