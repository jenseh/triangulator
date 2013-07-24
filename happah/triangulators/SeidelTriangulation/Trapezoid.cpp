#include "happah/triangulators/SeidelTriangulation/Trapezoid.h"

#include "happah/HappahConstants.h"
#include "happah/triangulators/SeidelTriangulation/Node.h"

#include <GL/gl.h>

using namespace SeidelTriangulation;

int Trapezoid::currentID;

Trapezoid::Trapezoid() {
	ID = currentID++;
}

Trapezoid::~Trapezoid() {

	if(lowerRight) unlink(this, lowerRight);
	if(lowerLeft) unlink(this, lowerLeft);
	if(upperRight) unlink(upperRight, this);
	if(upperLeft) unlink(upperLeft, this);
	if(upperCornerNeighbor) unlink(upperCornerNeighbor, this);
	if(lowerCornerNeighbor) unlink(this, lowerCornerNeighbor);
}

Trapezoid* Trapezoid::splitHorizontal(hpvec2& point) {
	
	Trapezoid* upper = new Trapezoid();
	Trapezoid* lower = new Trapezoid();
	
	printf("split at (%f, %f)\n", point.x, point.y);
	printf("u %d, l %d replace %d\n", Trapezoid::getID(upper), Trapezoid::getID(lower), Trapezoid::getID(this));
	
	YNode* yNode = sink->splitHorizontal(point, upper, lower);

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
		unlink(this, lc);
		link(lower, lc);
	}
	
	
	delete this;
	return lower;
}

void Trapezoid::setSink(Sink* sink) {
	this->sink = sink;
}

Sink* Trapezoid::getSink() {
	return sink;
}

Trapezoid* Trapezoid::getNextAlongSegment(const SegmentEndpoints2D* segment) {
	
	Trapezoid* result;
	if(!lowerLeft) {
		printf("trying to get next along Segment but lowerLeft is NULL\n");
		throw -1;
	}
	hpvec2 upperPoint = lowerLeft->upperRightVertex;
		
	hpdouble x = getSegment2DX(upperPoint.y, segment);
	result = (x < upperPoint.x)
			? lowerLeft
			: lowerRight;

	return result;	
}

void Trapezoid::merge(Trapezoid* upper, Trapezoid* lower) {
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
	tmp = lower->lowerCornerNeighbor;
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
	
	trapezoid->tryToMerge();
}

void Trapezoid::tryToMerge() {
	
	//
	printf("try to merge %d with %d\n", Trapezoid::getID(this), Trapezoid::getID(upperLeft));	
	if (upperLeft 
		&& (leftSegment == upperLeft->leftSegment) 
		&& (rightSegment == upperLeft->rightSegment)) {

		printf("merging\n");
		Trapezoid::merge(upperLeft, this);		
	}
}

void Trapezoid::link(Trapezoid* upper, Trapezoid* lower) {
	
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

void Trapezoid::unlink(Trapezoid* upper, Trapezoid* lower) {
	
	if(upper && lower) {
		printf("unlink %d, %d\n", Trapezoid::getID(upper), Trapezoid::getID(lower));
		upper->removeLower(lower);
		lower->removeUpper(upper);
	}
}

void Trapezoid::addUpper(Trapezoid* upper) {
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

void Trapezoid::addLower(Trapezoid* lower) {
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

void Trapezoid::removeUpper(Trapezoid* removee) {
	
	if(upperLeft == removee) {
		upperLeft = upperRight;
		upperRight = NULL;
	} else if(upperRight == removee) {
		upperRight = NULL;
	} else if(upperCornerNeighbor == removee) {
		upperCornerNeighbor = NULL;
	} else {
		printf("tried to remove not linked %d from above %d\n", Trapezoid::getID(removee), Trapezoid::getID(this));
	}
}

void Trapezoid::removeLower(Trapezoid* removee) {
	
	if(lowerLeft == removee) {
		lowerLeft = lowerRight;
		lowerRight = NULL;
	} else if(lowerRight == removee) {
		lowerRight = NULL;
	} else if(lowerCornerNeighbor == removee) {
		lowerCornerNeighbor = NULL;
	} else {
		printf("tried to remove not linked %d from below %d\n", Trapezoid::getID(removee), Trapezoid::getID(this));
	}
}

int Trapezoid::getID(Trapezoid* trapezoid) {
	return (trapezoid) ? trapezoid->ID : -1;
}

void Trapezoid::flagOutside(bool isOutside) {
	
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

			printf("%d is outside AND inside ???\n", Trapezoid::getID(this));
		}
	}
}

vector<vector<hpvec2>*>* Trapezoid::generateMonotoneChains() {

	vector<vector<hpvec2>*>* chains = new vector<vector<hpvec2>*>;	

	if(isOutside == MaybeBool::FALSE && lowerLeft) {
		if(!leftSegment) {
			printf("inside but no left Segment\n");
		}

		if(!rightSegment) {
			printf("inside but no right Segment\n");
		}

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

void Trapezoid::continueChain(vector<hpvec2>* chain, SegmentEndpoints2D* segment) {

	if(!leftSegment) {
		printf("trying to continue chain but no leftSegment\n");
		throw -1;
	}
	if(!rightSegment) {
		printf("trying to continue chain but no rightSegment\n");
		throw -1;
	}

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
		
		if(!lowerRight) {
			lowerLeft->continueChain(chain, rightSegment);
		} else {
			lowerRight->continueChain(chain, rightSegment);
		}
	}
}

hpvec2 Trapezoid::mid() {
	hpvec2 ul = upperLeftVertex;
	hpvec2 ur = upperRightVertex;
	hpvec2 ll = lowerLeftVertex;
	hpvec2 lr = lowerRightVertex;

	if(ul.x < -2.0) ul.x = -1.0;				
	if(ur.x > 2.0) ur.x = 1.0;				
	if(ll.x < -2.0) ll.x = -1.0;				
	if(lr.x > 2.0) lr.x = 1.0;				

	if(ul.y > 2.0) ul.y = 1.0;				
	if(ur.y > 2.0) ur.y = 1.0;				
	if(ll.y < -2.0) ll.y = -1.0;				
	if(lr.y < -2.0) lr.y = -1.0;
				
	return (ul + ur + ll + lr) * 0.25f;
}

void Trapezoid::drawDebug() {
				
	hpvec2 ul = upperLeftVertex;
	hpvec2 ur = upperRightVertex;
	hpvec2 ll = lowerLeftVertex;
	hpvec2 lr = lowerRightVertex;

	if(ul.x < -2.0) ul.x = -1.0;				
	if(ur.x > 2.0) ur.x = 1.0;				
	if(ll.x < -2.0) ll.x = -1.0;				
	if(lr.x > 2.0) lr.x = 1.0;
	
	if(ul.y > 2.0) ul.y = 1.0;				
	if(ur.y > 2.0) ur.y = 1.0;				
	if(ll.y < -2.0) ll.y = -1.0;				
	if(lr.y < -2.0) lr.y = -1.0;				
	
	glLineWidth(2.0f);
	
	glBegin(GL_LINES);
	
	glColor3f(0.5f, 0.5f, 0.5f);
	glVertex2fv((float*)&ul);
	glVertex2fv((float*)&ur);
	
	glVertex2fv((float*)&ll);
	glVertex2fv((float*)&lr);
	
	glEnd();
	
	glPointSize(10.0f);
	glBegin(GL_POINTS);
	
	if(isOutside == MaybeBool::TRUE) glColor3f(0.0f, 1.0f, 0.0f);
	if(isOutside == MaybeBool::FALSE) glColor3f(1.0f, 0.0f, 0.0f);
	if(isOutside == MaybeBool::UNDEFINED) glColor3f(0.0f, 0.0f, 1.0f);
	hpvec2 m = mid();
	glVertex2fv((float*)&m);
	
	glEnd();
	glPointSize(5.0f);
	
	glEnable(GL_LINE_STIPPLE);
	glLineStipple(3, 0x5C55);
	glBegin(GL_LINES);
	
	hpvec2 b;
	
	glColor3f(0.0f, 1.0f, 0.0f);
	if(upperLeft) {
		b = upperLeft->mid();
		glVertex2fv((float*)&m);
		glVertex2fv((float*)&b);
	}
	
	if(upperRight) {
		b = upperRight->mid();
		glVertex2fv((float*)&m);
		glVertex2fv((float*)&b);
	}
	
	glColor3f(1.0f, 0.0f, 0.0f);
	if(lowerLeft) {
		b = lowerLeft->mid();
		glVertex2fv((float*)&m);
		glVertex2fv((float*)&b);
	}
	
	if(lowerRight) {
		b = lowerRight->mid();
		glVertex2fv((float*)&m);
		glVertex2fv((float*)&b);
	}
	
	glColor3f(0.0f, 0.0f, 1.0f);
	if(upperCornerNeighbor) {
		b = upperCornerNeighbor->mid();
		glVertex2fv((float*)&m);
		glVertex2fv((float*)&b);
	}
	
	glColor3f(1.0f, 1.0f, 0.0f);
	if(lowerCornerNeighbor) {
		b = lowerCornerNeighbor->mid();
		glVertex2fv((float*)&m);
		glVertex2fv((float*)&b);
	}
	
	glEnd();
	glLineStipple(1, 0xFFFF);
	glLineWidth(3.0f);
}

