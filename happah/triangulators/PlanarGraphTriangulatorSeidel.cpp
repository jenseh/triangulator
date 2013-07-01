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
		Trapezoid* next;
		Trapezoid* end;
				
		Trapezoid* upperTrapezoid = root->getTrapezoid(upper);
		if(upperTrapezoid->upperLeftVertex.y != upper.y) {
			current = upperTrapezoid->splitHorizontal(upper);
		} else {
			current = upperTrapezoid;
		}
		
		Trapezoid* lowerTrapezoid = root->getTrapezoid(lower);
		if(lowerTrapezoid->upperLeftVertex.y != lower.y) {
			end = lowerTrapezoid->splitHorizontal(lower);
		} else {
			end = lowerTrapezoid;
		}
		
		if(current == lowerTrapezoid) {
		
			current = end->upperLeft;
		}		

		if(current->leftSegment && lower.x < getSegment2DX(lower.y, current->leftSegment)) {
			printf("starting from %X instead of %X\n", current->upperLeft->lowerLeft, current);
			current = current->upperLeft->lowerLeft;
		}

		Trapezoid* lastLeft = current->upperLeft;
		Trapezoid* lastMiddleLeft = current->upperMiddle;
		Trapezoid* lastMiddleRight = current->upperRight;
		Trapezoid* lastRight = NULL;
		
		if(lastLeft) Trapezoid::unlink(lastLeft, current);
		if(lastMiddleLeft) Trapezoid::unlink(lastMiddleLeft, current);
		if(lastMiddleRight) Trapezoid::unlink(lastMiddleRight, current);
		
		printf("Splitting from %d to %d\n", Trapezoid::getID(current), Trapezoid::getID(end));
		Trapezoid* left = NULL;
		Trapezoid* right = NULL;
		while(current != end) {
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

			left->leftSegment = current->leftSegment;
			left->rightSegment = segment;
			right->leftSegment = segment;
			right->rightSegment = current->rightSegment;
			
			Trapezoid::link(lastLeft, left);
			Trapezoid::link(lastMiddleLeft, left);
			Trapezoid::link(lastMiddleRight, left);
			Trapezoid::link(lastLeft, right);
			Trapezoid::link(lastMiddleLeft, right);
			Trapezoid::link(lastMiddleRight, right);
			Trapezoid::link(lastRight, right);
			
			if(next->upperLeft == current) {
				lastLeft = left;
				lastMiddleLeft = right;
				lastMiddleRight = next->upperMiddle;
				lastRight = next->upperRight;
			} else if(next->upperMiddle == current) {
				lastLeft = next->upperLeft;
				lastMiddleLeft = left;
				lastMiddleRight = right;
				lastRight = next->upperRight;
			} else {
				lastLeft = next->upperLeft;
				lastMiddleLeft = next->upperMiddle;
				lastMiddleRight = left;
				lastRight = right;
			}
			
			Trapezoid* ll = current->lowerLeft;
			Trapezoid* lm = current->lowerMiddle;
			Trapezoid* lr = current->lowerRight;
			
			Trapezoid::unlink(current, current->lowerRight);
			Trapezoid::unlink(current, current->lowerMiddle);
			Trapezoid::unlink(current, current->lowerLeft);
			
			if(ll == next) {
				Trapezoid::link(right, lm);
				Trapezoid::link(right, lr);
			} else if(lm == next) {
				Trapezoid::link(left, ll);
				Trapezoid::link(right, lr);
			} else {
				Trapezoid::link(left, ll);
				Trapezoid::link(left, lm);
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
	if(lowerMiddle) unlink(this, lowerMiddle);
	if(lowerLeft) unlink(this, lowerLeft);
	if(upperRight) unlink(upperRight, this);
	if(upperMiddle) unlink(upperMiddle, this);
	if(upperLeft) unlink(upperLeft, this);
}

PlanarGraphTriangulatorSeidel::Trapezoid* PlanarGraphTriangulatorSeidel::Trapezoid::splitHorizontal(hpvec2& point) {
	
	Trapezoid* upper = new Trapezoid();
	Trapezoid* lower = new Trapezoid();printf("u %d, l %d replace %d\n", Trapezoid::getID(upper), Trapezoid::getID(lower), Trapezoid::getID(this));

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
	Trapezoid* um = upperMiddle;
	Trapezoid* ul = upperLeft;
	Trapezoid* lr = lowerRight;
	Trapezoid* lm = lowerMiddle;
	Trapezoid* ll = lowerLeft;
	if(ur) {
		unlink(ur, this);
		link(ur, upper);
	}
	if(um) {
		unlink(um, this);
		link(um, upper);
	}
	if(ul) {
		unlink(ul, this);
		link(ul, upper);
	}
	if(lr) {
		unlink(this, lr);
		link(lower, lr);
	}
	if(lm) {
		unlink(this, lm);
		link(lower, lm);
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
	if(x < upperPoint.x) {
		result = lowerLeft;
	} else {
		upperPoint = lowerMiddle->upperRightVertex;
		if(x < upperPoint.x) {
			result = lowerMiddle;
		} else {
			result = lowerRight;
		}
	}

	return result;	
}

void PlanarGraphTriangulatorSeidel::Trapezoid::addUpper(Trapezoid* upper) {
	if(upperLeft && upperMiddle && upperRight) {
		printf("try to add %d to %d, but already full with %d, %d, %d\n", Trapezoid::getID(upper), Trapezoid::getID(this), Trapezoid::getID(upperLeft), Trapezoid::getID(upperMiddle), Trapezoid::getID(upperRight));
	} else
	if(!upperLeft || (upperLeft->leftSegment && upperLeft->leftSegment == upper->rightSegment)) {
		upperRight = upperMiddle;
		upperMiddle = upperLeft;
		upperLeft = upper;
	} else if(!upperMiddle || upperMiddle->leftSegment == upper->rightSegment) {
		upperRight = upperMiddle;
		upperMiddle = upper;
	} else {
		upperRight = upper;
	}
}

void PlanarGraphTriangulatorSeidel::Trapezoid::addLower(Trapezoid* lower) {
	if(lowerLeft && lowerMiddle && lowerRight) {
		printf("try to add %d to %d, but already full with %d, %d, %d\n", Trapezoid::getID(lower), Trapezoid::getID(this), Trapezoid::getID(lowerLeft), Trapezoid::getID(lowerMiddle), Trapezoid::getID(lowerRight));
	} else
	if(!lowerLeft || (lowerLeft->leftSegment && lowerLeft->leftSegment == lower->rightSegment)) {
		lowerRight = lowerMiddle;
		lowerMiddle = lowerLeft;
		lowerLeft = lower;
	} else if(!lowerMiddle || lowerMiddle->leftSegment == lower->rightSegment) {
		lowerRight = lowerMiddle;
		lowerMiddle = lower;
	} else {
		lowerRight = lower;
	}
}

void PlanarGraphTriangulatorSeidel::Trapezoid::link(Trapezoid* upper, Trapezoid* lower) {
	
	if(upper && lower) {
		float a = upper->lowerLeftVertex.x -lower->upperRightVertex.x;
		float b = upper->lowerRightVertex.x -lower->upperLeftVertex.x;
		
		if((glm::sign(a) != glm::sign(b) || glm::abs(a) < HP_EPSILON || glm::abs(b) < HP_EPSILON)
			&& (upper->leftSegment == NULL || upper->leftSegment != lower->rightSegment)
			&& (upper->rightSegment == NULL || upper->rightSegment != lower->leftSegment)) {
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
		upperLeft = upperMiddle;
		upperMiddle = upperRight;
		upperRight = NULL;
	} else if(upperMiddle == removee) {
		upperMiddle = upperRight;
		upperRight = NULL;
	} else if(upperRight == removee) {
		upperRight = NULL;
	} else {
		printf("tried to remove not linked %X from above %X\n", removee, this);
	}
}

void PlanarGraphTriangulatorSeidel::Trapezoid::removeLower(Trapezoid* removee) {
	
	if(lowerLeft == removee) {
		lowerLeft = lowerMiddle;
		lowerMiddle = lowerRight;
		lowerRight = NULL;
	} else if(lowerMiddle == removee) {
		lowerMiddle = lowerRight;
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
	return (point.x < x)
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





