#include "happah/triangulators/PlanarGraphTriangulatorSeidel.h"

#include "happah/HappahConstants.h"

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
		Trapezoid* current;
		Trapezoid* end;
		Trapezoid* next;
		
		Trapezoid* upperTrapezoid = root->getTrapezoid(upper);
		if(upperTrapezoid->upperLeftVertex == upper && lower.x < getSegment2DX(lower.y, upperTrapezoid->leftSegment)) {
			upperTrapezoid = upperTrapezoid->upperLeft->lowerLeft;
		}
		if(upperTrapezoid->upperRightVertex == upper && lower.x > getSegment2DX(lower.y, upperTrapezoid->rightSegment)) {
			if(upperTrapezoid->upperRight) {
				upperTrapezoid = upperTrapezoid->upperRight->lowerRight;
			} else {
				upperTrapezoid = upperTrapezoid->upperLeft->lowerRight;
			}
		}
		
		if((upperTrapezoid->upperLeftVertex.y != upper.y || HP_EPSILON < glm::abs(upperTrapezoid->upperLeftVertex.x - upper.x))
			&& (upperTrapezoid->upperRightVertex.y != upper.y || HP_EPSILON < glm::abs(upperTrapezoid->upperRightVertex.x - upper.x))) { 
			printf("upperTrapezoid: (%f, %f), (%f, %f), (%f, %f), (%f, %f)\n", upperTrapezoid->upperLeftVertex.x, upperTrapezoid->upperLeftVertex.y, upperTrapezoid->upperRightVertex.x, upperTrapezoid->upperRightVertex.y, upperTrapezoid->lowerLeftVertex.x, upperTrapezoid->lowerLeftVertex.y, upperTrapezoid->lowerRightVertex.x, upperTrapezoid->lowerRightVertex.y);
			current = upperTrapezoid->splitHorizontal(upper);
		} else {
			current = upperTrapezoid;
		}
		
		Trapezoid* lowerTrapezoid = root->getTrapezoid(lower);
		if((lowerTrapezoid->upperLeftVertex.y != lower.y || HP_EPSILON < glm::abs(lowerTrapezoid->upperLeftVertex.x - lower.x))
			&& (lowerTrapezoid->upperRightVertex.y != lower.y || HP_EPSILON < glm::abs(lowerTrapezoid->upperRightVertex.x - lower.x))) { 
			printf("lowerTrapezoid: (%f, %f), (%f, %f), (%f, %f), (%f, %f)\n", lowerTrapezoid->upperLeftVertex.x, lowerTrapezoid->upperLeftVertex.y, lowerTrapezoid->upperRightVertex.x, lowerTrapezoid->upperRightVertex.y, lowerTrapezoid->lowerLeftVertex.x, lowerTrapezoid->lowerLeftVertex.y, lowerTrapezoid->lowerRightVertex.x, lowerTrapezoid->lowerRightVertex.y);
			end = lowerTrapezoid->splitHorizontal(lower)->upperLeft;
		} else {
			end = lowerTrapezoid;
		}
		
		if(current == lowerTrapezoid) {
		
			current = end->upperLeft;
		}
		
		Trapezoid* lastLeft = current->upperLeft;
		Trapezoid* lastMiddle = current->upperRight;
		Trapezoid* lastRight = NULL;
		
		if(lastLeft) Trapezoid::unlink(lastLeft, current);
		if(lastMiddle) Trapezoid::unlink(lastMiddle, current);
		
		printf("Splitting from %d to %d\n", Trapezoid::getID(current), Trapezoid::getID(end));
		
		Trapezoid* left = NULL;
		Trapezoid* right = NULL;
		while(current->upperLeftVertex.y != lower.y) {
			next = current->getNextAlongSegment(segment);
			
			left = new Trapezoid();
			right = new Trapezoid();printf("l %d, r %d replace %d\n", Trapezoid::getID(left), Trapezoid::getID(right), Trapezoid::getID(current));

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
			
			if(right->upperLeftVertex.x > right->upperRightVertex.x) {
				printf("ul(%f, %f) ur(%f, %f)\n", 
					right->upperLeftVertex.x, right->upperLeftVertex.y, 
					right->upperRightVertex.x, right->upperRightVertex.y);
				printf("seg((%f,%f),(%f,%f))\n",segment->a.x, segment->a.y, segment->b.x, segment->b.y);
			}

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
			
			Trapezoid* ll = current->lowerLeft;
			Trapezoid* lr = current->lowerRight;
			
			Trapezoid::unlink(current, ll);
			Trapezoid::unlink(current, lr);
			
			if(ll == next) {
				Trapezoid::link(right, lr);
			} else if(lr == next) {
				Trapezoid::link(left, ll);
			}
			
			delete current;
			
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

PlanarGraphTriangulatorSeidel::Trapezoid* PlanarGraphTriangulatorSeidel::Trapezoid::splitHorizontal(hpvec2& point) {
	
	Trapezoid* upper = new Trapezoid();
	Trapezoid* lower = new Trapezoid();printf("u %d, l %d replace %d\n", Trapezoid::getID(upper), Trapezoid::getID(lower), Trapezoid::getID(this));
printf("split at (%f, %f)\n", point.x, point.y);
	YNode* yNode = sink->splitHorizontal(point, upper, lower);

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
	
	delete this;
	return lower;
}

PlanarGraphTriangulatorSeidel::Trapezoid* PlanarGraphTriangulatorSeidel::Trapezoid::getNextAlongSegment(const SegmentEndpoints2D* segment) {
	
	Trapezoid* result;	
	hpvec2 upperPoint = lowerLeft->upperRightVertex;
		
	hpdouble x = getSegment2DX(upperPoint.y, segment);
	result = (x < upperPoint.x)
			? lowerLeft
			: lowerRight;

	return result;	
}

void PlanarGraphTriangulatorSeidel::Trapezoid::addUpper(Trapezoid* upper) {
	if(upperLeft && upperRight) {
		printf("try to add %d to %d, but already full with %d, %d\n", Trapezoid::getID(upper), Trapezoid::getID(this), Trapezoid::getID(upperLeft), Trapezoid::getID(upperRight));
	} else if(!upperLeft) {
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
		printf("try to add %d to %d, but already full with %d, %d\n", Trapezoid::getID(lower), Trapezoid::getID(this), Trapezoid::getID(lowerLeft), Trapezoid::getID(lowerRight));
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
	
	if(upper && lower
		&& upper->lowerLeftVertex.x != upper->lowerRightVertex.x
		&& lower->upperLeftVertex.x != lower->upperRightVertex.x) {
		float a = glm::sign(upper->lowerLeftVertex.x -lower->upperRightVertex.x);
		float b = glm::sign(upper->lowerRightVertex.x -lower->upperLeftVertex.x);
		
		if((a != b && a != 0.0 && b != 0.0)) {
			printf("link %d, %d\n", Trapezoid::getID(upper), Trapezoid::getID(lower));
			upper->addLower(lower);
			lower->addUpper(upper);
		} else {
			printf("didnt link %d, %d\n", Trapezoid::getID(upper), Trapezoid::getID(lower));
			printf("[%f,%f] [%f, %f] dont overlap\n", upper->lowerLeftVertex.x, upper->lowerRightVertex.x, lower->upperLeftVertex.x, lower->upperRightVertex.x);
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
		printf("tried to remove not linked %d from below %d\n", Trapezoid::getID(removee), Trapezoid::getID(this));
	}
}

void PlanarGraphTriangulatorSeidel::Trapezoid::unlink(Trapezoid* upper, Trapezoid* lower) {
	
	if(upper && lower) {
		printf("unlink %d, %d\n", Trapezoid::getID(upper), Trapezoid::getID(lower));
		upper->removeLower(lower);
		lower->removeUpper(upper);
	}
}

int PlanarGraphTriangulatorSeidel::Trapezoid::getID(Trapezoid* trapezoid) {
	return (trapezoid) ? trapezoid->ID : 0;
}

PlanarGraphTriangulatorSeidel::Trapezoid* PlanarGraphTriangulatorSeidel::XNode::getTrapezoid(const hpvec2& point) {
	
	hpdouble x = getSegment2DX(point.y, key);
	return (point.x <= x)
			? child1->getTrapezoid(point)
			: child2->getTrapezoid(point);
}

PlanarGraphTriangulatorSeidel::Trapezoid* PlanarGraphTriangulatorSeidel::YNode::getTrapezoid(const hpvec2& point) {

	return (point.y > key.y || (point.y == key.y && point.x > key.x))
			? child1->getTrapezoid(point)
			: child2->getTrapezoid(point);
}

PlanarGraphTriangulatorSeidel::Trapezoid* PlanarGraphTriangulatorSeidel::Sink::getTrapezoid(const hpvec2& point) {

	return trapezoid;
}

PlanarGraphTriangulatorSeidel::YNode* PlanarGraphTriangulatorSeidel::Sink::splitHorizontal(const hpvec2& key, Trapezoid* upper, Trapezoid* lower) {
	
	YNode* yNode = new YNode(key, parent);
	
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





