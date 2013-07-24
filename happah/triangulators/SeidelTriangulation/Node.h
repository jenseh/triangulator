#pragma once

#include "happah/geometries/Segment.h"
#include "happah/geometries/TriangleMesh.h"

namespace SeidelTriangulation {

	using namespace std;
	
	class Trapezoid;
	
	/**
    * Represents a Node in the binary structure 
    */
	class Node {
		
		public:

			/**
			 * ~Node
			 * 
			 * @brief Destruktor
			 */
			virtual ~Node() {}

			/**
			 * deleteStructures
			 * 
			 * @brief deletes the Trapezoid Structure and Querry Structure recursively
			 */
			virtual void deleteStructures() = 0;

			/**
			 * getTrapezoid
			 * 
			 * @param const hpvec2& point Point to get the Trapezoid for
			 * @return Trapezoid* Trapezoid containing the point
			 * 
			 * @brief Searches the Trapezoid containing thhe given point
			 */
			virtual Trapezoid* getTrapezoid(const hpvec2& point) = 0;
			
			/**
			 * drawDebug
			 * 
			 * @brief calls drawDebug on all Trapezoids
			 */
			virtual void drawDebug() = 0;

			/**
			 * treeMerge
			 * 
			 * @brief calls tryToMerge on all Trapezoids
			 */
			virtual void treeMerge() = 0;

            /**
			 * getChild1
			 *
			 * @return Node* The child node
			 *
			 * @brief Simple getter for child1
			 */
			Node* getChild1();

             /**
			 * getChild2
			 *
			 * @return Node* The child node
			 *
			 * @brief Simple getter for child2
			 */
			Node* getChild2();
			
			/**
			 * setChild1(Node* child)
			 * 
			 * @param Node* child The Node to be set as child
			 * 
			 * @brief Simple setter for the child1 attribute
			 */
			void setChild1(Node* child);

			/**
			 * setChild2(Node* child)
			 * 
			 * @param Node* child The Node to be set as child
			 * 
			 * @brief Simple setter for the child2 attribute
			 */
			void setChild2(Node* child);

             /**
			 * getParent
			 *
			 * @return Node* The parent node
			 *
			 * @brief Simple getter for the parent
			 */
			vector<Node*> getParents();

			/**
			 * generateMonotoneChains
			 * 
			 * @return vector<vector<hpvec2>*>* Vector to the generated Monotone Chains
			 * 
			 * @brief calls generateMonotoneChains on all Trapezoids and collects the Monotone Chains
			 */
			virtual vector<vector<hpvec2>*>* generateMonotoneChains() = 0;

		protected:
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
			XNode(SegmentEndpoints2D* key, vector<Node*> parents);
    
			/**
			 * ~XNode
			 * 
			 * @brief Destruktor
			 */
			~XNode() {}

			/**
			 * deleteStructures
			 * 
			 * @brief deletes the Trapezoid Structure and Querry Structure recursively
			 */
			void deleteStructures();

			/**
			 * treeMerge
			 * 
			 * @brief calls tryToMerge on all Trapezoids
			 */
			void treeMerge();

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
			 * drawDebug
			 * 
			 * @brief calls drawDebug on all Trapezoids
			 */
			void drawDebug();

			/**
			 * generateMonotoneChains
			 * 
			 * @return vector<vector<hpvec2>*>* Vector to the generated Monotone Chains
			 * 
			 * @brief calls generateMonotoneChains on all Trapezoids and collects the Monotone Chains
			 */
			vector<vector<hpvec2>*>* generateMonotoneChains();


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
			YNode(hpvec2 key, vector<Node*> parents);

			~YNode() {}

			/**
			 * deleteStructures
			 * 
			 * @brief deletes the Trapezoid Structure and Querry Structure recursively
			 */
			void deleteStructures();

			/**
			 * treeMerge
			 * 
			 * @brief calls tryToMerge on all Trapezoids
			 */
			void treeMerge();

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
			 * drawDebug
			 * 
			 * @brief calls drawDebug on all Trapezoids
			 */
			void drawDebug();

			/**
			 * generateMonotoneChains
			 * 
			 * @return vector<vector<hpvec2>*>* Vector to the generated Monotone Chains
			 * 
			 * @brief calls generateMonotoneChains on all Trapezoids and collects the Monotone Chains
			 */
			vector<vector<hpvec2>*>* generateMonotoneChains();

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
			 * @param vector<Node*>& parents Multiple parents for this sink
			 *
			 * @brief Simple constructor which sets the trapezoid reference and its sink
			 */
			Sink(Trapezoid* trapezoid, vector<Node*>& parents);
            
            /**
			 * Sink
			 *
			 * @param Trapezoid* trapezoid the sinks trapezoid
			 * @param Node* parent Single parent for this sink
			 *
			 * @brief Simple constructor which sets the trapezoid reference and its sink
			 */
			Sink(Trapezoid* trapezoid, Node* parent);
            
			/**
			 * ~Sink
			 * 
			 * @brief Destructor
			 */
			~Sink() {}

			/**
			 * deleteStructures
			 * 
			 * @brief deletes the Trapezoid Structure and Querry Structure recursively
			 */
			void deleteStructures();

			/**
			 * treeMerge
			 * 
			 * @brief calls tryToMerge on all Trapezoids
			 */
			void treeMerge();

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
			
			/**
			 * merge
			 * 
			 * @access static
			 * @param Sink* upperSink Upper Sink to be merged
			 * @param Sink* lowerSink Lower Sink to be merged
			 * @param Trapezoid* Trapezoid the merged Sink should point to
			 * 
			 * @brief merges two Sinks
			 */
			static void merge(Sink* upperSink, Sink* lowerSink, Trapezoid* trapezoid);

			/**
			 * drawDebug
			 * 
			 * @brief calls drawDebug on all Trapezoids
			 */
			void drawDebug();

			/**
			 * generateMonotoneChains
			 * 
			 * @return vector<vector<hpvec2>*>* Vector to the generated Monotone Chains
			 * 
			 * @brief calls generateMonotoneChains on all Trapezoids and collects the Monotone Chains
			 */
			vector<vector<hpvec2>*>* generateMonotoneChains();

		private:
			Trapezoid* trapezoid;   /**< Every sink need to have a trapezoid  */
	};

	/**
	 * Represents a 2D mesh of Trapezoids
	 * Acts as the root Node of the Querry Structure
	 */
	class TrapezoidMesh2D : public Node {
		public:
		
			/**
			 * TrapezoidMesh2D()
			 * 
			 * @brief Constructor which sets the first child
			 */
			TrapezoidMesh2D();
	
			/**
			 * ~TrapezoidMesh2D
			 * 
			 * @brief Destructor. Deletes Querry and Trapezoid Structure
			 */
			~TrapezoidMesh2D();

			/**
			 * deleteStructures
			 * 
			 * @brief deletes the Trapezoid Structure and Querry Structure recursively
			 */
			void deleteStructures();

			/**
			 * treeMerge
			 * 
			 * @brief calls tryToMerge on all Trapezoids
			 */
			void treeMerge();

			/**
			 * toTriangleMesh
			 * 
			 * @return TriangleMesh2D* The generated Triangle Mesh
			 * 
			 * @brief generates a Triangle Mesh for this Trapezoid Mesh
			 */
			TriangleMesh2D* toTriangleMesh();

			/**
			 * getTrapezoid(const hpvec2& point)
			 *
			 * @param const hpvec2& point the point where we search for the nearest child instance
			 * @return Trapezoid* the nearest child instance
			 *
			 * @brief Gets the associated Trapezoid child instance closest to the given point.
			 */
			Trapezoid* getTrapezoid(const hpvec2& point);
				
			/**
			 * drawDebug
			 * 
			 * @brief calls drawDebug on all Trapezoids
			 */
			void drawDebug();

			/**
			 * generateMonotoneChains
			 * 
			 * @return vector<vector<hpvec2>*>* Vector to the generated Monotone Chains
			 * 
			 * @brief calls generateMonotoneChains on all Trapezoids and collects the Monotone Chains
			 */
			vector<vector<hpvec2>*>* generateMonotoneChains();
			
			/**
			 * storeSegment
			 * 
			 * @param segment Segment to be deleted later
			 * 
			 * @brief stores segments to be deleted when the structures get deleted
			 */
			void storeSegment(SegmentEndpoints2D* segment);
		
		private:
		
			vector<SegmentEndpoints2D*> segments; /**< Segments to be deleted when this gets deleted */
	};
}

