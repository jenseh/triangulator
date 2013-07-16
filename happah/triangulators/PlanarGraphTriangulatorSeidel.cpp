#include "happah/triangulators/PlanarGraphTriangulatorSeidel.h"

#include "happah/HappahConstants.h"

PlanarGraphTriangulatorSeidel::PlanarGraphTriangulatorSeidel() {}


PlanarGraphTriangulatorSeidel::~PlanarGraphTriangulatorSeidel() {}


//template<class PlanarGraphSegmentEndpointsIterator>
TriangleMesh2D* PlanarGraphTriangulatorSeidel::triangulate(PlanarGraphSegmentEndpointsIterator first, PlanarGraphSegmentEndpointsIterator last) {

	TrapezoidMesh2D* trapezoidMesh = trapezoidulate(first, last);
	if(trapezoidMesh == 0) return 0;
	TriangleMesh2D* triangleMesh = trapezoidMesh->toTriangleMesh();
	if (trapezoidMesh) {
		delete trapezoidMesh;
	}
	return triangleMesh;

}

//template<class PlanarGraphSegmentEndpointsIterator>
PlanarGraphTriangulatorSeidel::TrapezoidMesh2D* PlanarGraphTriangulatorSeidel::trapezoidulate(PlanarGraphSegmentEndpointsIterator first, PlanarGraphSegmentEndpointsIterator last) {

	IDGenerator::getInstance().reset();

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
		
		if( (upperTrapezoid->upperLeftVertex.y != upper.y || 
				HP_EPSILON < glm::abs(upperTrapezoid->upperLeftVertex.x - upper.x))
			&& (upperTrapezoid->upperRightVertex.y != upper.y || 
				HP_EPSILON < glm::abs(upperTrapezoid->upperRightVertex.x - upper.x))
			
			&& (!upperTrapezoid->upperLeft || upperTrapezoid->upperLeft->lowerRightVertex.y != upper.y || 
				HP_EPSILON < glm::abs(upperTrapezoid->upperLeft->lowerRightVertex.x - upper.x))) { 

			printf("upperTrapezoid: (%f, %f), (%f, %f), (%f, %f), (%f, %f)\n", 
					upperTrapezoid->upperLeftVertex.x, upperTrapezoid->upperLeftVertex.y, 
					upperTrapezoid->upperRightVertex.x, upperTrapezoid->upperRightVertex.y, 
					upperTrapezoid->lowerLeftVertex.x, upperTrapezoid->lowerLeftVertex.y, 
					upperTrapezoid->lowerRightVertex.x, upperTrapezoid->lowerRightVertex.y);

			current = upperTrapezoid->splitHorizontal(upper);
		} else {
			current = upperTrapezoid;
		}
		
		Trapezoid* lowerTrapezoid = root->getTrapezoid(lower);
		if((lowerTrapezoid->upperLeftVertex.y != lower.y || 
				HP_EPSILON < glm::abs(lowerTrapezoid->upperLeftVertex.x - lower.x))
			&& (lowerTrapezoid->upperRightVertex.y != lower.y || 
				HP_EPSILON < glm::abs(lowerTrapezoid->upperRightVertex.x - lower.x))
			&& (lowerTrapezoid->upperLeft->lowerRightVertex.y != lower.y || 
				HP_EPSILON < glm::abs(lowerTrapezoid->upperLeft->lowerRightVertex.x - lower.x))) { 

			printf("lowerTrapezoid: (%f, %f), (%f, %f), (%f, %f), (%f, %f)\n", 
				lowerTrapezoid->upperLeftVertex.x, lowerTrapezoid->upperLeftVertex.y, 
				lowerTrapezoid->upperRightVertex.x, lowerTrapezoid->upperRightVertex.y, 
				lowerTrapezoid->lowerLeftVertex.x, lowerTrapezoid->lowerLeftVertex.y, 
				lowerTrapezoid->lowerRightVertex.x, lowerTrapezoid->lowerRightVertex.y);

			end = lowerTrapezoid->splitHorizontal(lower);
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
			right = new Trapezoid();
			printf("l %d, r %d replace %d\n", Trapezoid::getID(left), Trapezoid::getID(right), Trapezoid::getID(current));

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

	root->getTrapezoid(hpvec2(INFINITY, INFINITY))->initializeMerge();
	InstanceHolder::getInstance().clear();
	
	printf("merging done\n");
	
	root->getTrapezoid(hpvec2(INFINITY, INFINITY))->flagOutside(true);

	printf("flaging done\n");
	
	//vector<vector<hpvec2>*>* chains = root->generateMonotoneChains();

	//printf("chaining done\n");
	
	return (TrapezoidMesh2D*)root;
}

void PlanarGraphTriangulatorSeidel::Trapezoid::merge(Trapezoid* upper, Trapezoid* lower) {
	Trapezoid* trapezoid = new Trapezoid();
	
	Trapezoid* tmp = upper->upperRight;
	if (tmp) {
		unlink(tmp, upper);
		link(tmp, trapezoid);
	}
	tmp = upper->upperLeft;
	if (tmp) {
		unlink(tmp, upper);
		link(tmp, trapezoid);
	}

	tmp = lower->lowerRight;
	if (tmp) {
		unlink(lower, tmp);
		link(trapezoid, tmp);
	}
	tmp = lower->lowerLeft;
	if (tmp) {
		unlink(lower, tmp);
		link(trapezoid, tmp);
	}
	
	tmp = upper->upperCornerNeighbor;
	if (tmp) {
		unlink(tmp, upper);
		link(tmp, trapezoid);
	}
	tmp = upper->upperCornerNeighbor;
	if (tmp) {
		unlink(lower, tmp);
		link(trapezoid, tmp);
	}
	
	unlink(upper, lower);

	trapezoid->leftSegment = upper->leftSegment;
	trapezoid->rightSegment = upper->rightSegment;

	trapezoid->lowerLeftVertex = lower->lowerLeftVertex;
	trapezoid->lowerRightVertex = lower->lowerRightVertex;
	trapezoid->upperLeftVertex = upper->upperLeftVertex;
	trapezoid->upperRightVertex = upper->upperRightVertex;
	
	Sink::merge(upper->getSink(), lower->getSink(), trapezoid);

	delete upper;
	delete lower;

	for (auto trapezoid: InstanceHolder::getInstance().getTrapezoids()) {
		printf("id: %d (%f, %f), (%f, %f), (%f, %f), (%f, %f)\n", Trapezoid::getID(trapezoid),
				trapezoid->upperLeftVertex.x, trapezoid->upperLeftVertex.y, 
				trapezoid->upperRightVertex.x, trapezoid->upperRightVertex.y, 
				trapezoid->lowerLeftVertex.x, trapezoid->lowerLeftVertex.y, 
				trapezoid->lowerRightVertex.x, trapezoid->lowerRightVertex.y);
	}
}

void PlanarGraphTriangulatorSeidel::Trapezoid::initializeMerge() {
	
	//
	vector<Trapezoid*> trapezoids = InstanceHolder::getInstance().getTrapezoids();
	for (auto trapezoid : trapezoids) {
		printf("try to merge %d with %d\n", Trapezoid::getID(trapezoid), Trapezoid::getID(trapezoid->upperLeft));	
		if (trapezoid->upperLeft 
			&& (trapezoid->leftSegment == trapezoid->upperLeft->leftSegment) 
			&& (trapezoid->rightSegment == trapezoid->upperLeft->rightSegment)) {

			printf("merging\n");
			Trapezoid::merge(trapezoid->upperLeft, trapezoid);		
		}
	}
}

PlanarGraphTriangulatorSeidel::Trapezoid::~Trapezoid() {
	InstanceHolder::getInstance().removeTrapezoid(this);

	if(lowerRight) unlink(this, lowerRight);
	if(lowerLeft) unlink(this, lowerLeft);
	if(upperRight) unlink(upperRight, this);
	if(upperLeft) unlink(upperLeft, this);

	upperLeftVertex = hpvec2(-0.5, -0.5);
	upperRightVertex = hpvec2(0.5, 0.5);
	lowerLeftVertex = hpvec2(-0.5, -0.5);
	lowerRightVertex = hpvec2(0.5, 0.5);
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
	
	Trapezoid* uc = upperCornerNeighbor;
	if(uc) {
		unlink(uc, this);
		link(uc, upper);
	}
	
	Trapezoid* lc = lowerCornerNeighbor;
	if(lc) {
		unlink(lc, this);
		link(lc, upper);
	}
	
	
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
	
	if(upper && lower) {
		float a = upper->lowerLeftVertex.x - lower->upperRightVertex.x;
		float b = upper->lowerRightVertex.x - lower->upperLeftVertex.x;
		a = (glm::abs(a) > HP_EPSILON) ? glm::sign(a) : 0;
		b = (glm::abs(b) > HP_EPSILON) ? glm::sign(b) : 0;
		
		if(HP_EPSILON > glm::abs(upper->lowerLeftVertex.x - upper->lowerRightVertex.x)
			|| HP_EPSILON > glm::abs(lower->upperLeftVertex.x - lower->upperRightVertex.x)) {
		
			if((a != b && a != 0.0 && b != 0.0)) {
				upper->lowerCornerNeighbor = lower;
				lower->upperCornerNeighbor = upper;
			}

		} else {

			if((a != b && a != 0.0 && b != 0.0)) {
				printf("link %d, %d\n", Trapezoid::getID(upper), Trapezoid::getID(lower));
				upper->addLower(lower);
				lower->addUpper(upper);
			} else {
				printf("didnt link %d, %d\n", Trapezoid::getID(upper), Trapezoid::getID(lower));
				printf("[%f,%f] [%f, %f] dont overlap\n", 
					upper->lowerLeftVertex.x, upper->lowerRightVertex.x, 
					lower->upperLeftVertex.x, lower->upperRightVertex.x);
			}
		}
	}
}

void PlanarGraphTriangulatorSeidel::Trapezoid::removeUpper(Trapezoid* removee) {
	
	if(upperLeft == removee) {
		upperLeft = upperRight;
		upperRight = NULL;
	} else if(upperRight == removee) {
		upperRight = NULL;
	} else if(upperCornerNeighbor == removee) {
		upperCornerNeighbor = NULL;
	} else {
		printf("tried to remove not linked %lX from above %lX\n", removee, this);
	}
}

void PlanarGraphTriangulatorSeidel::Trapezoid::removeLower(Trapezoid* removee) {
	
	if(lowerLeft == removee) {
		lowerLeft = lowerRight;
		lowerRight = NULL;
	} else if(lowerRight == removee) {
		lowerRight = NULL;
	} else if(lowerCornerNeighbor == removee) {
		upperCornerNeighbor = NULL;
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
	return (trapezoid) ? trapezoid->ID : -1;
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
	
	YNode* yNode = new YNode(key, parents);
	
	yNode->setChild1(new Sink(upper, yNode));
	yNode->setChild2(new Sink(lower, yNode));

	for(auto parent : parents) {	
		if(parent->child1 == this) {
			parent->child1 = yNode;
		} else {
			parent->child2 = yNode;
		}
	}

	delete this;

	return yNode;
}

PlanarGraphTriangulatorSeidel::XNode* PlanarGraphTriangulatorSeidel::Sink::splitVertical(SegmentEndpoints2D* segment, Trapezoid* left, Trapezoid* right) {
	
	XNode* xNode = new XNode(segment, parents);
	
	xNode->setChild1(new Sink(left, xNode));
	xNode->setChild2(new Sink(right, xNode));

	for(auto parent : parents) {	
		if(parent->child1 == this) {
			parent->child1 = xNode;
		} else {
			parent->child2 = xNode;
		}
	}

	delete this;

	return xNode;
}

void PlanarGraphTriangulatorSeidel::Sink::merge(PlanarGraphTriangulatorSeidel::Sink* upperSink, PlanarGraphTriangulatorSeidel::Sink* lowerSink, PlanarGraphTriangulatorSeidel::Trapezoid* trapezoid) {

	vector<Node*> parents;
	parents.insert(parents.begin(), upperSink->parents.begin(), upperSink->parents.end());
	parents.insert(parents.begin(), lowerSink->parents.begin(), lowerSink->parents.end());
	Sink* mergedSink = new Sink(trapezoid, parents);

	for(auto parent : parents) {
		if(parent->child1 == upperSink || parent->child1 == lowerSink) {
			parent->child1 = mergedSink;
		} else {
			parent->child2 = mergedSink;
		}
	}

	delete upperSink;
	delete lowerSink;
}

TriangleMesh2D* PlanarGraphTriangulatorSeidel::TrapezoidMesh2D::toTriangleMesh() {

	getTrapezoid(hpvec2(INFINITY, INFINITY))->flagOutside(true);

	return NULL;
}

void PlanarGraphTriangulatorSeidel::Trapezoid::flagOutside(bool isOutside) {
	
	if(this->isOutside == MaybeBool::UNDEFINED) {

		if(isOutside) {
			this->isOutside = MaybeBool::TRUE;
		} else {
			this->isOutside = MaybeBool::FALSE;
		}

		if(upperLeft)  upperLeft->flagOutside(isOutside);
		if(upperRight) upperRight->flagOutside(isOutside);
		if(lowerLeft)  lowerLeft->flagOutside(isOutside);
		if(lowerRight) lowerRight->flagOutside(isOutside);

		if(upperCornerNeighbor) upperCornerNeighbor->flagOutside(!isOutside);
		if(lowerCornerNeighbor) lowerCornerNeighbor->flagOutside(!isOutside);
		
	} else {

		if((this->isOutside == MaybeBool::TRUE && !isOutside)
			|| (this->isOutside == MaybeBool::FALSE && isOutside)) {

			printf("%d is outside AND inside ???", Trapezoid::getID(this));
		}
	}
}

vector<vector<hpvec2>*>* PlanarGraphTriangulatorSeidel::Trapezoid::generateMonotoneChains() {

	vector<vector<hpvec2>*>* chains = new vector<vector<hpvec2>*>;	

	if(isOutside == MaybeBool::FALSE && lowerLeft) {
		if(HP_EPSILON > glm::abs(upperLeftVertex.x - upperRightVertex.x)) {
			if(leftSegment->a.y == lowerLeftVertex.y || leftSegment->b.y == lowerLeftVertex.y) {
				vector<hpvec2>* chain = new vector<hpvec2>;
				chains->push_back(chain);
				chain->push_back(upperLeftVertex);
				chain->push_back(lowerLeftVertex);
				lowerLeft->continueChain(chain, rightSegment);
				
			} else if(rightSegment->a.y == lowerRightVertex.y || rightSegment->b.y == lowerRightVertex.y) {
				vector<hpvec2>* chain = new vector<hpvec2>;
				chains->push_back(chain);
				chain->push_back(upperLeftVertex);
				chain->push_back(lowerRightVertex);
				lowerLeft->continueChain(chain, leftSegment);
				
			} else {
				
				vector<hpvec2>* chain = new vector<hpvec2>;
				chains->push_back(chain);
				chain->push_back(upperLeftVertex);
				chain->push_back(lowerLeft->upperRightVertex);
				lowerLeft->continueChain(chain, leftSegment);
				
				chain = new vector<hpvec2>;
				chains->push_back(chain);
				chain->push_back(upperLeftVertex);
				chain->push_back(lowerLeft->upperRightVertex);
				lowerLeft->continueChain(chain, rightSegment);
			}
		} else if(leftSegment->a.y == upperLeftVertex.y || leftSegment->b.y == upperLeftVertex.y) {
			if(HP_EPSILON > glm::length(rightSegment->a - lowerRightVertex)
				|| HP_EPSILON > glm::length(rightSegment->b - lowerRightVertex)) {

				vector<hpvec2>* chain = new vector<hpvec2>;
				chains->push_back(chain);
				chain->push_back(upperLeftVertex);
				chain->push_back(lowerRightVertex);
				if(!lowerLeft)printf("id: %d (%f, %f), (%f, %f), (%f, %f), (%f, %f)\n", Trapezoid::getID(this),
				this->upperLeftVertex.x, this->upperLeftVertex.y, 
				this->upperRightVertex.x, this->upperRightVertex.y, 
				this->lowerLeftVertex.x, this->lowerLeftVertex.y, 
				this->lowerRightVertex.x, this->lowerRightVertex.y);
				lowerLeft->continueChain(chain, leftSegment);
			} else if(lowerLeft 
				&& (lowerLeft->rightSegment->a.y == lowerLeftVertex.y 
					|| lowerLeft->rightSegment->b.y == lowerLeftVertex.y)) {

				vector<hpvec2>* chain = new vector<hpvec2>;
				chains->push_back(chain);
				chain->push_back(upperLeftVertex);
				chain->push_back(lowerLeft->upperRightVertex);
				lowerLeft->continueChain(chain, leftSegment);
			}
		} else if(rightSegment->a.y == upperRightVertex.y || rightSegment->b.y == upperRightVertex.y) {
			if(HP_EPSILON > glm::length(leftSegment->a - lowerLeftVertex)
				|| HP_EPSILON > glm::length(leftSegment->b - lowerLeftVertex)) {

				vector<hpvec2>* chain = new vector<hpvec2>;
				chains->push_back(chain);
				chain->push_back(upperRightVertex);
				chain->push_back(lowerLeftVertex);
				lowerLeft->continueChain(chain, rightSegment);
			} else if(lowerRight 
				&& (lowerRight->leftSegment->a.y == lowerLeftVertex.y 
					|| lowerRight->leftSegment->b.y == lowerLeftVertex.y)) {

				vector<hpvec2>* chain = new vector<hpvec2>;
				chains->push_back(chain);
				chain->push_back(upperRightVertex);
				chain->push_back(lowerRight->upperLeftVertex);
				lowerRight->continueChain(chain, rightSegment);
			}
		}
	}

	return chains;
}

void PlanarGraphTriangulatorSeidel::Trapezoid::continueChain(vector<hpvec2>* chain, SegmentEndpoints2D* segment) {

	if(leftSegment->a.y == lowerLeftVertex.y || leftSegment->b.y == lowerLeftVertex.y) {
		chain->push_back(lowerLeftVertex);

	} else if(rightSegment->a.y == lowerRightVertex.y || rightSegment->b.y == lowerRightVertex.y) {
		chain->push_back(lowerRightVertex);
	} else {
		chain->push_back(lowerLeft->rightSegment->a.y == lowerLeftVertex.y
							? lowerLeft->rightSegment->a
							: lowerLeft->rightSegment->b);
	}
	
	if(segment == leftSegment
		&& HP_EPSILON < glm::length(chain->back() - lowerLeftVertex)) {
		
		lowerLeft->continueChain(chain, leftSegment);
	}

	if(segment == rightSegment
		&& HP_EPSILON < glm::length(chain->back() - lowerRightVertex)) {
		
		lowerRight->continueChain(chain, rightSegment);
	}
}









