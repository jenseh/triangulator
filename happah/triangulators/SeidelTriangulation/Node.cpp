#include "happah/triangulators/SeidelTriangulation/Node.h"

#include "happah/HappahConstants.h"
#include "happah/triangulators/SeidelTriangulation/Trapezoid.h"

using namespace SeidelTriangulation;

Node* Node::getChild1() {
	return child1;
}

Node* Node::getChild2() {
	return child2;
}

void Node::setChild1(Node* child) {
	child1 = child;
}

void Node::setChild2(Node* child) {
	child2 = child;
}

vector<Node*> Node::getParents() {
	return parents;
}

XNode::XNode(SegmentEndpoints2D* key, vector<Node*> parents) 
	: key(key) {

	this->parents = parents;
}

void XNode::deleteStructures() {
	if(child1) {
		child1->deleteStructures();
		delete child1;
	}
	if(child2) {
		child2->deleteStructures();
		delete child2;
	}
	for(auto parent : parents) {
		if(this == parent->getChild1()) {
			parent->setChild1(NULL);
		}
		if(this == parent->getChild2()) {
			parent->setChild2(NULL);
		}
	}
	delete this;
}

void XNode::treeMerge() {
	if(child1) {
		child1->treeMerge();
	}
	if(child2) {
		child2->treeMerge();
	}
}

Trapezoid* XNode::getTrapezoid(const hpvec2& point) {
	
	hpdouble x = getSegment2DX(point.y, key);
	return (point.x <= x)
			? child1->getTrapezoid(point)
			: child2->getTrapezoid(point);
}

void XNode::drawDebug() {
				
	child1->drawDebug();
	child2->drawDebug();
}

vector<vector<hpvec2>*>* XNode::generateMonotoneChains() {
	
	vector<vector<hpvec2>*>* chains1 = child1->generateMonotoneChains();
	vector<vector<hpvec2>*>* chains2 = child2->generateMonotoneChains();
	vector<vector<hpvec2>*>* result = new vector<vector<hpvec2>*>();
	result->insert(result->end(), chains1->begin(), chains1->end());
	result->insert(result->end(), chains2->begin(), chains2->end());
	delete chains1;
	delete chains2;
	return result;
}

YNode::YNode(hpvec2 key, vector<Node*> parents) 
	: key(key) {

	this->parents = parents;
}

void YNode::deleteStructures() {
	if(child1) {
		child1->deleteStructures();
	}
	if(child2) {
		child2->deleteStructures();
	}
	for(auto parent : parents) {
		if(this == parent->getChild1()) {
			parent->setChild1(NULL);
		}
		if(this == parent->getChild2()) {
			parent->setChild2(NULL);
		}
	}
	delete this;
}

void YNode::treeMerge() {
	if(child1) {
		child1->treeMerge();
	}
	if(child2) {
		child2->treeMerge();
	}
}

Trapezoid* YNode::getTrapezoid(const hpvec2& point) {

	return (point.y > key.y || (point.y == key.y && point.x > key.x))
			? child1->getTrapezoid(point)
			: child2->getTrapezoid(point);
}

void YNode::drawDebug() {
				
	child1->drawDebug();
	child2->drawDebug();
}

vector<vector<hpvec2>*>* YNode::generateMonotoneChains() {
	
	vector<vector<hpvec2>*>* chains1 = child1->generateMonotoneChains();
	vector<vector<hpvec2>*>* chains2 = child2->generateMonotoneChains();
	vector<vector<hpvec2>*>* result = new vector<vector<hpvec2>*>();
	result->insert(result->end(), chains1->begin(), chains1->end());
	result->insert(result->end(), chains2->begin(), chains2->end());
	delete chains1;
	delete chains2;
	return result;
}

Sink::Sink(Trapezoid* trapezoid, vector<Node*>& parents)
	: trapezoid(trapezoid) {

	trapezoid->setSink(this);
	this->parents = parents;
}

Sink::Sink(Trapezoid* trapezoid, Node* parent) 
	: trapezoid(trapezoid) {

	trapezoid->setSink(this);
	this->parents.push_back(parent);
}

void Sink::deleteStructures() {
	if(trapezoid) {
		delete trapezoid;
	}
	
	for(auto parent : parents) {
		if(this == parent->getChild1()) {
			parent->setChild1(NULL);
		}
		if(this == parent->getChild2()) {
			parent->setChild2(NULL);
		}
	}
	delete this;
}

void Sink::treeMerge() {
	trapezoid->tryToMerge();
}

Trapezoid* Sink::getTrapezoid(const hpvec2& point) {

	return trapezoid;
}

YNode* Sink::splitHorizontal(const hpvec2& key, Trapezoid* upper, Trapezoid* lower) {
	
	YNode* yNode = new YNode(key, parents);
	
	yNode->setChild1(new Sink(upper, yNode));
	yNode->setChild2(new Sink(lower, yNode));

	upper->setSink((Sink*)yNode->getChild1());
	lower->setSink((Sink*)yNode->getChild2());

	for(auto parent : parents) {	
		if(parent->getChild1() == this) {
			parent->setChild1(yNode);
		} else {
			parent->setChild2(yNode);
		}
	}

	delete this;

	return yNode;
}

XNode* Sink::splitVertical(SegmentEndpoints2D* segment, Trapezoid* left, Trapezoid* right) {
	
	XNode* xNode = new XNode(segment, parents);
	
	xNode->setChild1(new Sink(left, xNode));
	xNode->setChild2(new Sink(right, xNode));

	left->setSink((Sink*)xNode->getChild1());
	right->setSink((Sink*)xNode->getChild2());
				
	for(auto parent : parents) {	
		if(parent->getChild1() == this) {
			parent->setChild1(xNode);
		} else {
			parent->setChild2(xNode);
		}
	}

	delete this;
	
	return xNode;
}

void Sink::merge(Sink* upperSink, Sink* lowerSink, Trapezoid* trapezoid) {

	vector<Node*> parents;
	parents.insert(parents.begin(), upperSink->parents.begin(), upperSink->parents.end());
	parents.insert(parents.begin(), lowerSink->parents.begin(), lowerSink->parents.end());
	Sink* mergedSink = new Sink(trapezoid, parents);

	for(auto parent : parents) {
		if(parent->getChild1() == upperSink || parent->getChild1() == lowerSink) {
			parent->setChild1(mergedSink);
		} else {
			parent->setChild2(mergedSink);
		}
	}

	delete upperSink;
	delete lowerSink;
}

void Sink::drawDebug() {
				
	trapezoid->drawDebug();
}

vector<vector<hpvec2>*>* Sink::generateMonotoneChains() {
				
	return trapezoid->generateMonotoneChains();
}

TrapezoidMesh2D::TrapezoidMesh2D() {
			
	child1 = new Sink(new Trapezoid(), this);
}

TrapezoidMesh2D::~TrapezoidMesh2D() {
	deleteStructures();
	for(auto segment : segments) {
		delete segment;
	}
}

void TrapezoidMesh2D::deleteStructures() {
	printf("deleting Structures\n");
	if(child1) {
		child1->deleteStructures();
	}
}

void TrapezoidMesh2D::treeMerge() {
	if(child1) {
		child1->treeMerge();
	}
}

bool convex(hpvec2 previous, hpvec2 current, hpvec2 next, bool left) {

	SegmentEndpoints2D segment(previous, current);
	bool result = next.x < getSegment2DX(next.y, &segment);
	if(current.y > previous.y) result = !result;
	if(left) result = !result;

	return result;
}

TriangleMesh2D* TrapezoidMesh2D::toTriangleMesh() {

	vector<vector<hpvec2>*>* chains = generateMonotoneChains();

	printf("chaining done\n");
	
	vector<hpvec2>* mesh = new vector<hpvec2>;
	vector<hpuint>* indices = new vector<hpuint>;

	hpuint index = 0;

	for(auto chain : *chains) {

		SegmentEndpoints2D segment(chain->front(), *(chain->begin() + 1));
		bool left = chain->back().x > getSegment2DX(chain->back().y, &segment);

		vector<hpvec2>::iterator current = chain->begin() + 1;

		while(chain->size() > 2) {
			
			vector<hpvec2>::iterator previous;
			vector<hpvec2>::iterator next;
			previous = current - 1;
			next = current + 1;

			while(!convex(*previous, *current, *next, left)) {
				previous = current;
				current = next;
				next = current + 1;
			}
			
			mesh->push_back(*previous);
			mesh->push_back(hpvec2());
			indices->push_back(index++);
			mesh->push_back(*current);
			mesh->push_back(hpvec2());
			indices->push_back(index++);
			mesh->push_back(*next);
			mesh->push_back(hpvec2());
			indices->push_back(index++);

			chain->erase(current);
			current = previous;
		}
	}
	
	return new TriangleMesh2D(mesh, indices);
}

Trapezoid* TrapezoidMesh2D::getTrapezoid(const hpvec2& point) {
	return child1->getTrapezoid(point);
}

void TrapezoidMesh2D::drawDebug() {
			
	child1->drawDebug();
}

vector<vector<hpvec2>*>* TrapezoidMesh2D::generateMonotoneChains() {
	return child1->generateMonotoneChains();
}

void TrapezoidMesh2D::storeSegment(SegmentEndpoints2D* segment) {
	segments.push_back(segment);
}