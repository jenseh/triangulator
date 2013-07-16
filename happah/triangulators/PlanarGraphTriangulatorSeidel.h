#pragma once

#include "happah/triangulators/PlanarGraphTriangulator.h"

#include <GL/glfw.h>
#include <algorithm>
typedef vector<SegmentEndpoints2D>::iterator PlanarGraphSegmentEndpointsIterator;

/**
* Offers triangulation functionalty of given polygons according to Seidel's algorithm
*/
class PlanarGraphTriangulatorSeidel : public PlanarGraphTriangulator {

public:

	class Sink;  
	class InstanceHolder;


	
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

		void reset() {
		
			currentID = 0;
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
				InstanceHolder::getInstance().addTrapezoid(this);
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
			
			hpvec2 mid() {
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
			
			void drawDebug() {
				
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


			static void merge(Trapezoid* upper, Trapezoid* lower);
			void initializeMerge();
			

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

			void flagOutside(bool isOutside);

			vector<vector<hpvec2>*>* generateMonotoneChains();

			void continueChain(vector<hpvec2>* chain, SegmentEndpoints2D* segment);

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
			Trapezoid* upperRight = NULL;	/**< Adjacent Trapezoid at the upper right */
			Trapezoid* lowerLeft = NULL;	/**< Adjacent Trapezoid at the lower left */
			Trapezoid* lowerRight = NULL;	/**< Adjacent Trapezoid at the lower right */

			Trapezoid* upperCornerNeighbor = NULL;
			Trapezoid* lowerCornerNeighbor = NULL;

			enum MaybeBool {FALSE = 0, TRUE = 1, UNDEFINED = -1};

			MaybeBool isOutside = MaybeBool::UNDEFINED;
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
			vector<Node*> getParents() {return parents;};

			virtual vector<vector<hpvec2>*>* generateMonotoneChains() = 0;

		public:
			Node* child1 = NULL;    /**< First child */
			Node* child2 = NULL;    /**< Second child */
			vector<Node*> parents;    /**< Parent node */
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
			XNode(SegmentEndpoints2D* key, vector<Node*> parents) : key(key) {
				this->parents = parents;
			}
    
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

			vector<vector<hpvec2>*>* generateMonotoneChains() {
				vector<vector<hpvec2>*>* chains1 = child1->generateMonotoneChains();
				vector<vector<hpvec2>*>* chains2 = child2->generateMonotoneChains();
				vector<vector<hpvec2>*>* result = new vector<vector<hpvec2>*>();
				result->insert(result->end(), chains1->begin(), chains1->end());
				result->insert(result->end(), chains2->begin(), chains2->end());
				delete chains1;
				delete chains2;
				return result;
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
			YNode(hpvec2 key, vector<Node*> parents) : key(key) {
				this->parents = parents;
			}

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

			vector<vector<hpvec2>*>* generateMonotoneChains() {
				vector<vector<hpvec2>*>* chains1 = child1->generateMonotoneChains();
				vector<vector<hpvec2>*>* chains2 = child2->generateMonotoneChains();
				vector<vector<hpvec2>*>* result = new vector<vector<hpvec2>*>();
				result->insert(result->end(), chains1->begin(), chains1->end());
				result->insert(result->end(), chains2->begin(), chains2->end());
				delete chains1;
				delete chains2;
				return result;
			}

		private:
			hpvec2 key;   /**< Every YNode has a double value as key*/
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
			Sink(Trapezoid* trapezoid, vector<Node*>& parents) : trapezoid(trapezoid) {
				trapezoid->setSink(this);
				this->parents = parents;
			}
            
            Sink(Trapezoid* trapezoid, Node* parent) : trapezoid(trapezoid) {
				trapezoid->setSink(this);
				this->parents.push_back(parent);
			}
            
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
			YNode* splitHorizontal(const hpvec2& key, Trapezoid* upper, Trapezoid* lower);

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
			
			static void merge(Sink* upperSink, Sink* lowerSink, Trapezoid* trapezoid);

			void drawDebug() {
				
				trapezoid->drawDebug();
			}

			vector<vector<hpvec2>*>* generateMonotoneChains() {
				
				return trapezoid->generateMonotoneChains();
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
		}

		TriangleMesh2D* toTriangleMesh();

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
		}
			
		void drawDebug() {
			
			child1->drawDebug();
		}

		vector<vector<hpvec2>*>* generateMonotoneChains() {
			return child1->generateMonotoneChains();
		}
	};


	class InstanceHolder {
		public:
			static InstanceHolder& getInstance()
        	{
            	static InstanceHolder    instance; // Guaranteed to be destroyed.
                                  // Instantiated on first use.
            	return instance;
        	}

			void addTrapezoid(Trapezoid* trapezoid) {trapezoids.push_back(trapezoid);};

			void removeTrapezoid(Trapezoid* trapezoid) {
				trapezoids.erase(std::find(trapezoids.begin(), trapezoids.end(), trapezoid)); 
			}
			void clear() {trapezoids = vector<Trapezoid*>();};
			vector<Trapezoid*> getTrapezoids() {return trapezoids;};
		private:
			vector<Trapezoid*> trapezoids;
			int currentID; /**< the increment counter */

		
        	InstanceHolder() {};         


        	InstanceHolder(InstanceHolder const&);              // Don't Implement
        	void operator=(InstanceHolder const&); 		  // Don't implement
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


