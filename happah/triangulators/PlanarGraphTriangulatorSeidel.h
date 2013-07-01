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
	* Singleton to generate Trapezoid UIDs for debugging purpose
	*/
	class IDGenerator {
    public:
		/**
		 * getNextID
		 *
		 * @return an auto incrementing UID
		 *
		 * @brief gets the next Trapezoid UID
		 */
		int getNextID() {
			return currentID++;
		}

		/**
		 * getInstance()
		 *
		 * @return a reference to the IDGenerator instance
		 *
		 * @brief access method for the IDGenerator instance
		 */
        static IDGenerator& getInstance()
        {
            static IDGenerator    instance; // Guaranteed to be destroyed.
                                  // Instantiated on first use.
            return instance;
        }

    private:
		int currentID; /**< the increment counter */

		/**
		 * IDGenerator()
		 * 
		 * @return a IDGenerator instance
		 *
		 * @brief Constructor for IDGenerator, sets the UID counter to 0
		 */
        IDGenerator() {currentID = 0;};         

          
        // Dont forget to declare these two. You want to make sure they
        // are unaccessable otherwise you may accidently get copies of
        // your singleton appearing.

        IDGenerator(IDGenerator const&);              // Don't Implement
        void operator=(IDGenerator const&); 		  // Don't implement
};  

    /**
    * Represents a trapezoid used for trapezoidulation of given segments.
    */
	class Trapezoid {
		friend PlanarGraphTriangulatorSeidel;
		public:

			/**
			 * Trapezoid()
			 *
			 * @return Trapezoid instance
			 *
			 * @brief Constructor for the Trapezoid class, sets the UID
			 */
			Trapezoid() {
				ID = IDGenerator::getInstance().getNextID();
			}

			/**
			 * ~Trapezoid()
			 *
			 * @brief Destructor for the Trapezoid class
			 */
			~Trapezoid();

            /**
			 * splitHorizontal
			 *
			 * @param hpvec2& point point to split at
			 * @return void
			 *
			 * @brief Splits a trapezoid horizontally at a given point
			 *///TODO
			Trapezoid* splitHorizontal(hpvec2& point);

            /**
			 * setSink
			 *
			 * @param Sink* sink The sink to set
			 * @return void
			 *
			 * @brief Simple setter for the sink
			 */
			void setSink(Sink* sink) {this->sink = sink;};
			
			/**
			 * getSink()
			 *
			 * @return a pointer to the Trapezoid's Sink
			 *
			 * @brief Getter method for the sink attribute
			 */
			Sink* getSink() {return sink;};
			
			/**
			 * getNextAlongSegment
			 *
			 * @param const SegmentEndpoints2D* segment The segment that the Trapezoid is being split by
			 * @return pointer to the next Trapezoid
			 *
			 * @brief gets the underlying Trapezoid that intersects with the given segment
			 */		
			Trapezoid* getNextAlongSegment(const SegmentEndpoints2D* segment);
			
			void drawDebug() {
				hpvec2 ul = upperLeftVertex;
				hpvec2 ur = upperRightVertex;
				hpvec2 ll = lowerLeftVertex;
				hpvec2 lr = lowerRightVertex;

				if(ul.x == -INFINITY) ul.x = -1.0;				
				if(ur.x == INFINITY) ur.x = 1.0;				
				if(ll.x == -INFINITY) ll.x = -1.0;				
				if(lr.x == INFINITY) lr.x = 1.0;				

				glLineWidth(2.0f);
	
				glBegin(GL_LINES);
				
				glColor3f(0.0f, 1.0f, 0.0f);
				
				glVertex2fv((float*)&ul);
				glVertex2fv((float*)&ur);
				glVertex2fv((float*)&ll);
				glVertex2fv((float*)&lr);
				
				glEnd();
				glLineWidth(3.0f);
			};

			/**
			 * link()
			 * 
			 * @param Trapezoid* upper Pointer to the upper trapezoid
			 * @param Trapezoid* lower Pointer to the lower trapezoid
			 * 
			 * @brief Sets the right Trapezoid references after a horizontal split.
			 */
			static void link(Trapezoid* upper, Trapezoid* lower);

			/**
			 * unlink()
			 * 
			 * @param Trapezoid* first Pointer to the first trapezoid
			 * @param Trapezoid* second Pointer to the second trapezoid
			 *
			 * @brief Remove the mutual Trapezoid references in two given Trapezoids.  
			 */
			static void unlink(Trapezoid* first, Trapezoid* second);

			/**
			 * addUpper(Trapezoid* upper)
			 * 
			 * @param Trapezoid* upper Pointer to the Trapezoid to be set
			 * 
			 * @brief This sets the correct upper references with the given Trapezoid as upper Trapezoid
			 */
			void addUpper(Trapezoid* upper);

			/**
			 * addLower(Trapezoid* upper)
			 * 
			 * @param Trapezoid* lower Pointer to the Trapezoid to be set
			 * 
			 * @brief This sets the correct lower references with the given Trapezoid as lower Trapezoid
			 */
			void addLower(Trapezoid* lower);

			/**
			 * removeUpper(Trapezoid* removee)
			 * 
			 * @param Trapezoid* removee The Trapezoid to be removed
			 * 
			 * @brief Remove the upper references to the given Trapezoid
			 */
			void removeUpper(Trapezoid* removee);

			/**
			 * removeLower(Trapezoid* removee)
			 * 
			 * @param Trapezoid* removee The Trapezoid to be removed
			 * 
			 * @brief Remove the lower references to the given Trapezoid
			 */
			void removeLower(Trapezoid* removee);

			/**
			 * getID(Trapezoid* trapezoid)
			 * 
			 * @param Trapezoid* trapezoid The Trapezoid to get the ID for
			 * @return the Trapezoid ID as int, 0 if the trapezoid is NULL
			 * 
			 * @brief Wrapper for Trapezoid->ID, handles the (trapezoid == NULL) case
			 */
			static int getID(Trapezoid* trapezoid);

		public:
			Sink* sink; /**< The sink object*/
			int ID;		/**< The Trapezoid ID, used for debugging output to prevent ugly pointers */
            
			hpvec2 lowerLeftVertex = hpvec2(-INFINITY, -INFINITY); 	/**< Coords of the lower left corner */
			hpvec2 lowerRightVertex = hpvec2(INFINITY, -INFINITY);	/**< Coords of the lower right corner */
			hpvec2 upperLeftVertex = hpvec2(-INFINITY, INFINITY);	/**< Coords of the upper left corner */
			hpvec2 upperRightVertex = hpvec2(INFINITY, INFINITY);	/**< Coords of the upper right corner */	

			SegmentEndpoints2D* leftSegment = NULL;   /**< Left bounding segment */
			SegmentEndpoints2D* rightSegment = NULL;  /**< Right bounding segment */

			Trapezoid* upperLeft = NULL;	/**< Adjacent Trapezoid at the upper left */
			Trapezoid* upperMiddle = NULL;
			Trapezoid* upperRight = NULL;	/**< Adjacent Trapezoid at the upper right */
			Trapezoid* lowerLeft = NULL;	/**< Adjacent Trapezoid at the lower left */
			Trapezoid* lowerMiddle = NULL;
			Trapezoid* lowerRight = NULL;	/**< Adjacent Trapezoid at the lower right */
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
			
			/**
			 * setChild1(Node* child)
			 * 
			 * @param Node* child The Node to be set as child
			 * 
			 * @brief Simple setter for the child1 attribute
			 */
			void setChild1(Node* child) {child1 = child;};

			/**
			 * setChild2(Node* child)
			 * 
			 * @param Node* child The Node to be set as child
			 * 
			 * @brief Simple setter for the child2 attribute
			 */
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
			 *
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

	/**
	 * Represents a 2D mesh of Trapezoids
	 */
	class TrapezoidMesh2D : public Node {
		public:
		/**
		 * TrapezoidMesh2D()
		 * 
		 * @brief Constructor which sets the first child
		 */
		TrapezoidMesh2D() {
			child1 = new Sink(new Trapezoid(), this);
		};

		/**
		 * getTrapezoid(const hpvec2& point)
		 *
		 * @param const hpvec2& point the point where we search for the nearest child instance
		 * @return Trapezoid* the nearest child instance
		 *
		 * @brief Gets the associated Trapezoid child instance closest to the given point.
		 */
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


