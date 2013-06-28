#include "happah/triangulators/PlanarGraphTriangulatorSeidel.h"

PlanarGraphTriangulatorSeidel::PlanarGraphTriangulatorSeidel() {}


PlanarGraphTriangulatorSeidel::~PlanarGraphTriangulatorSeidel() {}


//template<class PlanarGraphSegmentEndpointsIterator>
TriangleMesh2D* PlanarGraphTriangulatorSeidel::triangulate(PlanarGraphSegmentEndpointsIterator first, PlanarGraphSegmentEndpointsIterator last) {

	TrapezoidMesh2D* trapezoidMesh = trapezoidulate(first, last);
	if(trapezoidMesh == 0) return 0;
	TriangleMesh2D* triangleMesh = NULL;//trapezoidMesh->toTriangleMesh();
	if (trapezoidMesh) {
		delete trapezoidMesh;
	}
	return triangleMesh;

}

//template<class PlanarGraphSegmentEndpointsIterator>
PlanarGraphTriangulatorSeidel::TrapezoidMesh2D* PlanarGraphTriangulatorSeidel::trapezoidulate(PlanarGraphSegmentEndpointsIterator first, PlanarGraphSegmentEndpointsIterator last) {

	Node* root = new TrapezoidMesh2D();

	for(auto it = first; it != last; it++) {
		SegmentEndpoints2D* segment = new SegmentEndpoints2D(*it);
		hpvec2 upper = (segment->a.y > segment->b.y)
						? segment->a
						: segment->b;
		hpvec2 lower = (segment->a.y <= segment->b.y)
						? segment->a
						: segment->b;
		Trapezoid* upperTrapezoid = root->getTrapezoid(upper);
		printf("splitting %X horizontally... ", upperTrapezoid);
		upperTrapezoid->splitHorizontal(upper);
		printf("done\n");
		
		Trapezoid* lowerTrapezoid = root->getTrapezoid(lower);
		printf("splitting %X horizontally... ", lowerTrapezoid);
		lowerTrapezoid->splitHorizontal(lower);
		printf("done\n");

		Trapezoid* current = root->getTrapezoid(upper);
		Trapezoid* next;
		Trapezoid* last = root->getTrapezoid(lower);
		Trapezoid* lastLeft = current->upperLeft;
		Trapezoid* lastMiddle = current->upperRight;
		Trapezoid* lastRight = NULL;
		while(current != last) {
			next = current->getNextAlongSegment(segment);
			
			if(current == next) {
				printf("current: %X\n\tlowerLeft: %X\n\tlowerRight: %X\n", current, current->lowerLeft, current->lowerRight);
				throw -1; //TODO
			}
			
			Trapezoid* left = new Trapezoid();
			Trapezoid* right = new Trapezoid();

			printf("splitting %X vertically... ", current->sink);
			XNode* xNode = current->sink->splitVertical(segment, left, right);
			printf("done\n");

			left->setSink((Sink*)xNode->getChild1());
			right->setSink((Sink*)xNode->getChild2());

			left->leftSegment = current->leftSegment;
			right->rightSegment = current->rightSegment;
			left->rightSegment = segment;
			right->leftSegment = segment;

			if(lastLeft->rightSegment == segment) {
				//Fall 1
				left->upperLeft = lastLeft;
				left->upperRight = NULL;
				right->upperLeft = lastMiddle;
				right->upperRight = lastRight;

				if(lastLeft->leftSegment == left->leftSegment) {
				//Fall 1.1
					lastLeft->lowerLeft = left;
					lastLeft->lowerRight = right;
					if(lastRight != NULL) {
						//Fall 1.1.1
						lastMiddle->lowerLeft = right;
						lastMiddle->lowerRight = NULL;
						lastRight->lowerLeft = right;
						lastRight->lowerRight = NULL;
						
					} else {
						//Fall 1.1.2
						lastLeft->lowerLeft = left;
						lastLeft->lowerRight = NULL;
						lastMiddle->lowerLeft = right;
					}
				} else {
					//Fall 1.2
					lastLeft->lowerRight = left;
					lastMiddle->lowerLeft = right;
					lastMiddle->lowerRight = NULL;
				}

			} else if(lastMiddle == NULL) {
				//Fall 2
				left->upperLeft = lastLeft;
				left->upperRight = NULL;
				right->upperLeft = lastLeft;
				right->upperRight = NULL;

				lastLeft->lowerLeft = left;
				lastLeft->lowerRight = right;				

			} else if(lastMiddle->rightSegment == segment) {
				//Fall 3
				left->upperLeft = lastLeft;
				left->upperRight = lastMiddle;
				right->upperLeft = lastRight;
				left->upperRight = NULL;

				lastLeft->lowerLeft = left;
				lastLeft->lowerRight = NULL;
				lastMiddle->lowerLeft = left;
				lastMiddle->lowerRight = NULL;
				lastRight->lowerLeft = right;
				lastRight->lowerRight = NULL;
			} else {
				
				fprintf(stderr, "Unexpected Case while triangulating!");
				throw -1;
			}
			
			if(current->lowerRight) {
			
				hpvec2 upperPoint = (current->lowerLeft->rightSegment->a.y > current->lowerLeft->rightSegment->b.y)
									? current->lowerLeft->rightSegment->a
									: current->lowerLeft->rightSegment->b;
				hpdouble segmentX = getSegment2DX(upperPoint.y, segment);
				
				if(segmentX > upperPoint.x) {

					left->lowerLeft = current->lowerLeft;
					left->lowerRight = current->lowerRight;
					right->lowerLeft = current->lowerRight;
					right->lowerRight = NULL;
				} else {

					left->lowerLeft = current->lowerLeft;
					left->lowerRight = NULL;
					right->lowerLeft = current->lowerLeft;
					right->lowerRight = current->lowerRight;
				}
			} else {
	
				left->lowerLeft = current->lowerLeft;
				left->lowerRight = NULL;
				right->lowerLeft = current->lowerLeft;
				right->lowerRight = NULL;
			}

			if(next->upperLeft == current) {
				lastLeft = left;
				lastMiddle = right;
				lastRight = next->upperRight;
			} else {
				lastLeft = next->upperLeft;
				lastMiddle = left;
				lastRight = right;
			}
			current = next;
		}
	}
	return (TrapezoidMesh2D*)root;
}

void PlanarGraphTriangulatorSeidel::Trapezoid::splitHorizontal(hpvec2& point) {
	
	Trapezoid* upper = new Trapezoid();
	Trapezoid* lower = new Trapezoid();

	YNode* yNode = sink->splitHorizontal(point.y, upper, lower);

	upper->setSink((Sink*)yNode->getChild1());
	lower->setSink((Sink*)yNode->getChild2());

	upper->leftSegment = leftSegment;
	lower->leftSegment = leftSegment;
	upper->rightSegment = rightSegment;
	lower->rightSegment = rightSegment;
	
	upper->upperLeft = upperLeft;
	upper->upperRight = upperRight;
	lower->lowerLeft = lowerLeft;
	lower->lowerRight = lowerRight;

	upper->lowerLeft = lower;
	lower->upperLeft = upper;

	if(upperLeft && upperLeft->lowerLeft == this)
		upperLeft->lowerLeft = upper;
	if(upperRight && upperRight->lowerLeft == this)
		upperRight->lowerLeft = upper;
	if(upperLeft && upperLeft->lowerRight == this)
		upperLeft->lowerRight = upper;
	if(upperRight && upperRight->lowerRight == this)
		upperRight->lowerRight = upper;

	if(lowerLeft && lowerLeft->upperLeft == this)
		lowerLeft->upperLeft = lower;
	if(lowerRight && lowerRight->upperLeft == this)
		lowerRight->upperLeft = lower;
	if(lowerLeft && lowerLeft->upperRight == this)
		lowerLeft->upperRight = lower;
	if(lowerRight && lowerRight->upperRight == this)
		lowerRight->upperRight = lower;
	
	delete this;
}

PlanarGraphTriangulatorSeidel::Trapezoid* PlanarGraphTriangulatorSeidel::Trapezoid::getNextAlongSegment(const SegmentEndpoints2D* segment) {
	
	Trapezoid* result;	
	SegmentEndpoints2D* lowerSegment = lowerLeft->rightSegment;
	if(lowerRight == NULL || lowerSegment == NULL) {
		result = lowerLeft;
	} else {

		hpvec2 upperPoint = (lowerSegment->a.y > lowerSegment->b.y)
							? lowerSegment->a
							: lowerSegment->b;
		
		hpdouble x = getSegment2DX(upperPoint.y, segment);
	
		result = (x > upperPoint.x)
				? lowerRight
				: lowerLeft;
	}

	return result;	
}

PlanarGraphTriangulatorSeidel::Trapezoid* PlanarGraphTriangulatorSeidel::XNode::getTrapezoid(const hpvec2& point) {
	
	hpdouble x = getSegment2DX(point.y, key);
	return (point.x > x)
			? child1->getTrapezoid(point)
			: child2->getTrapezoid(point);
}

PlanarGraphTriangulatorSeidel::Trapezoid* PlanarGraphTriangulatorSeidel::YNode::getTrapezoid(const hpvec2& point) {

	return (point.y > key)
			? child1->getTrapezoid(point)
			: child2->getTrapezoid(point);
}

PlanarGraphTriangulatorSeidel::Trapezoid* PlanarGraphTriangulatorSeidel::Sink::getTrapezoid(const hpvec2& point) {

	return trapezoid;
}

PlanarGraphTriangulatorSeidel::YNode* PlanarGraphTriangulatorSeidel::Sink::splitHorizontal(const hpdouble& yValue, Trapezoid* upper, Trapezoid* lower) {
	
	YNode* yNode = new YNode(yValue, parent);
	
	yNode->setChild1(new Sink(upper, yNode));
	yNode->setChild2(new Sink(lower, yNode));

	if(parent->child1 == this) {
		parent->child1 = yNode;
	} else {
		parent->child2 = yNode;
	}

	delete this;

	return yNode;
}

PlanarGraphTriangulatorSeidel::XNode* PlanarGraphTriangulatorSeidel::Sink::splitVertical(SegmentEndpoints2D* segment, Trapezoid* left, Trapezoid* right) {
	
	XNode* xNode = new XNode(segment, parent);
	
	xNode->setChild1(new Sink(left, xNode));
	xNode->setChild2(new Sink(right, xNode));

	if(parent->child1 == this) {
		parent->child1 = xNode;
	} else {
		parent->child2 = xNode;
	}

	delete this;

	return xNode;
}





