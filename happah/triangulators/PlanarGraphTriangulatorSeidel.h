#pragma once

#include "happah/triangulators/PlanarGraphTriangulator.h"

#include <GL/glfw.h>

typedef vector<SegmentEndpoints2D>::iterator PlanarGraphSegmentEndpointsIterator;

/**
* Offers triangulation functionalty of given polygons according to Seidel's algorithm
*/
class PlanarGraphTriangulatorSeidel : public PlanarGraphTriangulator {

public:

	class Sink;    

    /**
    * Represents a trapezoid used for trapezoidulation of given segments.
    */
	class Trapezoid {
		friend PlanarGraphTriangulatorSeidel;
		public:
			~Trapezoid();
            /**
			 * splitHorizontal
			 *
			 * @param hpvec2& point point to split at
			 * @return void
			 *
			 * @brief Splits a trapezoid horizontally at a given point
			 */
			void splitHorizontal(hpvec2& point);

            /**
			 * splitVertical
			 *
			 * @param SegmentEndpoints2D& segment The segment to split at
			 * @return void
			 *
			 * @brief Splits a trapezoid vertically by a given segment
			 */
			//void splitVertical(SegmentEndpoints2D& segment);

            /**
			 * setSink
			 *
			 * @param Sink* sink The sink to set
			 * @return void
			 *
			 * @brief Simple setter for the sink
			 */
			void setSink(Sink* sink) {this->sink = sink;};
			
			Sink* getSink() {return sink;};

			Trapezoid* getNextAlongSegment(const SegmentEndpoints2D* segment);
			
			void drawDebug() {
				if(upperLeftVertex.x!=-INFINITY&&upperRightVertex.x!=INFINITY&&
				lowerLeftVertex.x!=-INFINITY&&lowerRightVertex.x!=INFINITY)
					printf("(%.3f, %.3f)->(%.3f, %.3f)\n(%.3f, %.3f)->(%.3f, %.3f)\n", upperLeftVertex.x, upperLeftVertex.y, upperRightVertex.x, upperRightVertex.y, lowerLeftVertex.x, lowerLeftVertex.y, lowerRightVertex.x, lowerRightVertex.y);
				glBegin(GL_LINES);
				
				glColor3f(1.0f, 1.0f, 1.0f);
				
				glVertex2fv((float*)&upperLeftVertex);
				glVertex2fv((float*)&upperRightVertex);
				glVertex2fv((float*)&lowerLeftVertex);
				glVertex2fv((float*)&lowerRightVertex);
				
				glEnd();
			};

			static void link(Trapezoid* upper, Trapezoid* lower);
			static void unlink(Trapezoid* first, Trapezoid* second);
			void addUpper(Trapezoid* upper);
			void addLower(Trapezoid* lower);
			void removeUpper(Trapezoid* removee);
			void removeLower(Trapezoid* removee);

		public:
			Sink* sink; /**< The sink object*/
            
			hpvec2 lowerLeftVertex = hpvec2(-INFINITY, -INFINITY);
			hpvec2 lowerRightVertex = hpvec2(INFINITY, -INFINITY);
			hpvec2 upperLeftVertex = hpvec2(-INFINITY, INFINITY);
			hpvec2 upperRightVertex = hpvec2(INFINITY, INFINITY);
			
			SegmentEndpoints2D* leftSegment = NULL;   /**< Left bounding segment */
			SegmentEndpoints2D* rightSegment = NULL;  /**< Right bounding segment */

			Trapezoid* upperLeft = NULL;
			Trapezoid* upperRight = NULL;
			Trapezoid* lowerLeft = NULL;
			Trapezoid* lowerRight = NULL;
	};

    /**
    * Represents a Node in the binary structure 
    */
	class Node {
		
		public:
			virtual Trapezoid* getTrapezoid(const hpvec2& point) = 0;

			virtual void drawDebug() = 0;

            /**
			 * getChild1
			 *
			 * @return Node* The child node
			 *
			 * @brief Simple getter for child1
			 */
			Node* getChild1() {return child1;};

             /**
			 * getChild2
			 *
			 * @return Node* The child node
			 *
			 * @brief Simple getter for child2
			 */
			Node* getChild2() {return child2;};

			void setChild1(Node* child) {child1 = child;};
			void setChild2(Node* child) {child2 = child;};

             /**
			 * getParent
			 *
			 * @return Node* The parent node
			 *
			 * @brief Simple getter for the parent
			 */
			Node* getParent() {return parent;};

		public:
			Node* child1 = NULL;    /**< First child */
			Node* child2 = NULL;    /**< Second child */
			Node* parent = NULL;    /**< Parent node */
	};
    
    /**
    * Represents an XNode in the query structure
    */
	class XNode : public Node {
		
		public:
            /**
			 * XNode
			 *
			 * @param SegmentEndpoints2D* key the segment which acts as a key
			 * @brief Simple constructor which sets the key
			 */
			XNode(SegmentEndpoints2D* key, Node* parent) : key(key) {
				this->parent = parent;
			};
    
            /**
			 * getTrapezoid
			 *
			 * @param const hpvec2& point the point where we search for the nearest child instance
			 * @return Trapezoid* the nearest child instance
			 *
			 * @brief Gets the associated Trapezoid child instance closest to the given point.
			 */
			Trapezoid* getTrapezoid(const hpvec2& point);
			
			void drawDebug() {
				
				child1->drawDebug();
				child2->drawDebug();
			}

		private:
			SegmentEndpoints2D* key;    /**< Every XNode has a segment as key */
	};

    /**
    * Represents a YNode in the query structure
    */
	class YNode : public Node {
		
		public:
            /**
			 * YNode
			 *
			 * @param hpdouble key the double value which acts as a key
			 * @brief Simple constructor which sets the key
			 */
			YNode(hpdouble key, Node* parent) : key(key) {
				this->parent = parent;
			};

            /**
			 * getTrapezoid
			 *
			 * @param const hpvec2& point the point where we search for the nearest child instance
			 * @return Trapezoid* the nearest child instance
			 *
			 * @brief Gets the associated Trapezoid child instance closest to the given point.
			 */
			Trapezoid* getTrapezoid(const hpvec2& point);
			
			void drawDebug() {
				
				child1->drawDebug();
				child2->drawDebug();
			}

		private:
			hpdouble key;   /**< Every YNode has a double value as key*/
	};

    /**
    * Represents a Sink in the query structure
    */
	class Sink : public Node {

		public:
            /**
			 * Sink
			 *
			 * @param Trapezoid* trapezoid the sinks trapezoid
			 * @brief Simple constructor which sets the trapezoid reference and its sink
			 */
			Sink(Trapezoid* trapezoid, Node* parent) : trapezoid(trapezoid) {
				trapezoid->setSink(this);
				this->parent = parent;
			};
            
            /**
			 * getTrapezoid
			 *
			 * @param const hpvec2& point the point where we search for the nearest child instance
			 * @return Trapezoid* the nearest child instance
			 *
			 * @brief Gets the associated Trapezoid child instance closest to the given point.
			 */
			Trapezoid* getTrapezoid(const hpvec2& point);

            /**
			 * splitHorizontal
			 *
			 * @param const hpdouble& yValue Y-Value to split at
			 * @param Trapezoid* upper the upper accompanying trapezoid
			 * @param Trapezoid* lower the lower accompanying trapezoid
			 * @return YNode* the freshly created YNode
			 *
			 * @brief Split a Sink horizontally into a YNode with 2 children, deletes itself
			 */
			YNode* splitHorizontal(const hpdouble& yValue, Trapezoid* upper, Trapezoid* lower);

            /**
			 * splitVertical
			 *
			 * @param SegmentEndpoints2D& segment The segment to split at
			 * @param Trapezoid* left the left accompanying trapezoid
			 * @param Trapezoid* right the right accompanying trapezoid
			 * @return XNode* the freshly created XNode
			 *
			 * @brief  Split a Sink vertically into an XNode with 2 children, deletes itself
			 */
			XNode* splitVertical(SegmentEndpoints2D* segment, Trapezoid* left, Trapezoid* right);
			
			void drawDebug() {
				
				trapezoid->drawDebug();
			}

		private:
			Trapezoid* trapezoid;   /**< Every sink need to have a trapezoid  */
	};

	class TrapezoidMesh2D : public Node {
		public:
		TrapezoidMesh2D() {
			child1 = new Sink(new Trapezoid(), this);
		};

		Trapezoid* getTrapezoid(const hpvec2& point) {
			return child1->getTrapezoid(point);
		};
			
		void drawDebug() {
				
			child1->drawDebug();
		}
	};

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
	//template<class PlanarGraphSegmentEndpointsIterator>
	TrapezoidMesh2D* trapezoidulate(PlanarGraphSegmentEndpointsIterator first, PlanarGraphSegmentEndpointsIterator last);
    
    /**
	 * PlanarGraphTriangulatorSeidel
	 *
	 * @brief Default constructor
	 */
	PlanarGraphTriangulatorSeidel();

    /**
	 * ~PlanarGraphTriangulatorSeidel
	 *
	 * @brief Default destructor
	 */
	~PlanarGraphTriangulatorSeidel();
    
    /**
	 * triangulate
	 *
	 * @param PlanarGraphSegmentEndpointsIterator first Iterator with first SegmentEndpoints2D as starting point
	 * @param PlanarGraphSegmentEndpointsIterator last Iterator with last SegmentEndpoints2D as ending point
	 * @return TriangleMesh2D* a triangulized mesh
	 *
	 * @brief Triangulates a given amount of SegmentEndpoints into a TriangleMesh2D*
	 */
	//template<class PlanarGraphSegmentEndpointsIterator>
	TriangleMesh2D* triangulate(PlanarGraphSegmentEndpointsIterator first, PlanarGraphSegmentEndpointsIterator last);


};


