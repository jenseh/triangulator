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
		if(upperTrapezoid->upperLeftVertex != upper && upperTrapezoid->upperRightVertex != upper)
		upperTrapezoid->splitHorizontal(upper);
		
		Trapezoid* lowerTrapezoid = root->getTrapezoid(lower);
		if(lowerTrapezoid->upperLeftVertex != lower && upperTrapezoid->upperRightVertex != lower)
		lowerTrapezoid->splitHorizontal(lower);
		
		Trapezoid* current = root->getTrapezoid(upper);
		Trapezoid* next;
		Trapezoid* end = root->getTrapezoid(lower);
		
		Trapezoid* lastLeft = current->upperLeft;
		Trapezoid* lastMiddle = current->upperRight;
		Trapezoid* lastRight = NULL;
		
		if(lastLeft) Trapezoid::unlink(lastLeft, current);
		if(lastMiddle) Trapezoid::unlink(lastMiddle, current);
		
		printf("Splitting from %X to %X\n", current, end);
		Trapezoid* left = NULL;
		Trapezoid* right = NULL;
		while(current != end) {
			next = current->getNextAlongSegment(segment);
			
			left = new Trapezoid();
			right = new Trapezoid();printf("l %X, r %X replace %X\n", left, right, current);

			XNode* xNode = current->getSink()->splitVertical(segment, left, right);
			
			left->setSink((Sink*)xNode->getChild1());
			right->setSink((Sink*)xNode->getChild2());

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
			
			Trapezoid::link(lastLeft, left);
			Trapezoid::link(lastMiddle, left);
			Trapezoid::link(lastLeft, right);
			Trapezoid::link(lastMiddle, right);
			Trapezoid::link(lastRight, right);
			
			if(next->upperLeft == current) {
				lastLeft = left;
				lastMiddle = right;
				lastRight = next->upperRight;
			} else {
				lastLeft = next->upperLeft;
				lastMiddle = left;
				lastRight = right;
			}
			
			Trapezoid::unlink(current, current->lowerRight);
			Trapezoid::unlink(current, current->lowerLeft);
			
			//delete current;
			
			current = next;
		}
		Trapezoid::link(left, end);
		Trapezoid::link(right, end);
	}
	return (TrapezoidMesh2D*)root;
}

PlanarGraphTriangulatorSeidel::Trapezoid::~Trapezoid() {
	if(lowerRight) unlink(this, lowerRight);
	if(lowerLeft) unlink(this, lowerLeft);
	if(upperRight) unlink(upperRight, this);
	if(upperLeft) unlink(upperLeft, this);
}

void PlanarGraphTriangulatorSeidel::Trapezoid::splitHorizontal(hpvec2& point) {
	
	Trapezoid* upper = new Trapezoid();
	Trapezoid* lower = new Trapezoid();printf("u %X, l %X replace %X\n", upper, lower, this);

	YNode* yNode = sink->splitHorizontal(point.y, upper, lower);

	upper->setSink((Sink*)yNode->getChild1());
	lower->setSink((Sink*)yNode->getChild2());

	hpvec2 leftVertex;
	if(leftSegment) leftVertex = hpvec2(getSegment2DX(point.y, leftSegment), point.y);
	else leftVertex = hpvec2(-INFINITY, point.y);
	hpvec2 rightVertex;
	if(rightSegment) rightVertex = hpvec2(getSegment2DX(point.y, rightSegment), point.y);
	else rightVertex = hpvec2(INFINITY, point.y);

	upper->upperLeftVertex = upperLeftVertex;
	upper->upperRightVertex = upperRightVertex;
	upper->lowerLeftVertex = leftVertex;
	upper->lowerRightVertex = rightVertex;
	
	lower->lowerLeftVertex = lowerLeftVertex;
	lower->lowerRightVertex = lowerRightVertex;
	lower->upperLeftVertex = leftVertex;
	lower->upperRightVertex = rightVertex;
	
	upper->leftSegment = leftSegment;
	lower->leftSegment = leftSegment;
	upper->rightSegment = rightSegment;
	lower->rightSegment = rightSegment;
	
	Trapezoid* ur = upperRight;
	Trapezoid* ul = upperLeft;
	Trapezoid* lr = lowerRight;
	Trapezoid* ll = lowerLeft;
	printf("%X, %X, %X, %X connected to %X\n", ur, ul, lr, ll, this);
	if(ur) {
		unlink(ur, this);
		link(ur, upper);
	}
	if(ul) {
		unlink(ul, this);
		link(ul, upper);
	}
	if(lr) {
		unlink(this, lr);
		link(lower, lr);
	}
	if(ll) {
		unlink(this, ll);
		link(lower, ll);
	}
	link(upper, lower);
	
	//delete this;
}

PlanarGraphTriangulatorSeidel::Trapezoid* PlanarGraphTriangulatorSeidel::Trapezoid::getNextAlongSegment(const SegmentEndpoints2D* segment) {
	
	Trapezoid* result;	
	SegmentEndpoints2D* lowerSegment = lowerLeft->rightSegment;
	if(lowerRight == NULL || lowerSegment == NULL) {
		result = lowerLeft;
	} else {

		hpvec2 upperPoint = lowerLeft->upperRightVertex;
		
		hpdouble x = getSegment2DX(upperPoint.y, segment);
	
		result = (x < upperPoint.x)
				? lowerLeft
				: lowerRight;
	}

	return result;	
}

void PlanarGraphTriangulatorSeidel::Trapezoid::addUpper(Trapezoid* upper) {
	if(upperLeft && upperRight) {
		printf("try to add %X to %X, but already full with %X, %X\n", upper, this, upperLeft, upperRight);
	} else
	if(!upperLeft) {
		upperLeft = upper;
	} else if(upperLeft->leftSegment && upperLeft->leftSegment == upper->rightSegment) {
		upperRight = upperLeft;
		upperLeft = upper;
	} else {
		upperRight = upper;
	}
}

void PlanarGraphTriangulatorSeidel::Trapezoid::addLower(Trapezoid* lower) {
	if(lowerLeft && lowerRight) {
		printf("try to add %X to %X, but already full with %X, %X\n", lower, this, lowerLeft, lowerRight);
	} else
	if(!lowerLeft) {
		lowerLeft = lower;
	} else if(lowerLeft->leftSegment && lowerLeft->leftSegment == lower->rightSegment) {
		lowerRight = lowerLeft;
		lowerLeft = lower;
	} else {
		lowerRight = lower;
	}
}

void PlanarGraphTriangulatorSeidel::Trapezoid::link(Trapezoid* upper, Trapezoid* lower) {
	
	if(upper && lower) {
		float a = glm::sign(upper->lowerLeftVertex.x -lower->upperRightVertex.x);
		float b = glm::sign(upper->lowerRightVertex.x -lower->upperLeftVertex.x);
		
		if(a != 0 && b != 0 && a != b) {
			printf("link %X, %X\n", upper, lower);
			upper->addLower(lower);
			lower->addUpper(upper);
		}
	}
}

void PlanarGraphTriangulatorSeidel::Trapezoid::removeUpper(Trapezoid* removee) {
	
	if(upperLeft == removee) {
		upperLeft = upperRight;
		upperRight = NULL;
	} else if(upperRight == removee) {
		upperRight = NULL;
	} else {
		printf("tried to remove not linked %X from above %X\n", removee, this);
	}
}

void PlanarGraphTriangulatorSeidel::Trapezoid::removeLower(Trapezoid* removee) {
	
	if(lowerLeft == removee) {
		lowerLeft = lowerRight;
		lowerRight = NULL;
	} else if(lowerRight == removee) {
		lowerRight = NULL;
	} else {
		printf("tried to remove not linked %X from below %X\n", removee, this);
	}
}

void PlanarGraphTriangulatorSeidel::Trapezoid::unlink(Trapezoid* upper, Trapezoid* lower) {
	
	if(upper && lower) {
		printf("unlink %X, %X\n", upper, lower);
		upper->removeLower(lower);
		lower->removeUpper(upper);
	}
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





