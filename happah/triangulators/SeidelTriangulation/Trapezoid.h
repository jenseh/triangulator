#pragma once

#include "happah/geometries/Segment.h"

class PlanarGraphTriangulatorSeidel;

namespace SeidelTriangulation {
	
	using namespace std;
	
	class Sink;

	/**
	* Represents a trapezoid used for trapezoidulation of given segments.
	*/
	class Trapezoid {
		friend PlanarGraphTriangulatorSeidel;
		public:

			/**
			 * Trapezoid
			 *
			 * @return Trapezoid instance
			 *
			 * @brief Constructor for the Trapezoid class, sets the UID
			 */
			Trapezoid();
			
			/**
			 * ~Trapezoid
			 *
			 * @brief Destructor for the Trapezoid class
			 */
			~Trapezoid();

            /**
			 * splitHorizontal
			 *
			 * @param hpvec2& point point to split at
			 * @return Trapezoid below the split
			 *
			 * @brief Splits a trapezoid horizontally at a given point
			 */
			Trapezoid* splitHorizontal(hpvec2& point);

            /**
			 * setSink
			 *
			 * @param Sink* sink The sink to set
			 *
			 * @brief Simple setter for the sink
			 */
			void setSink(Sink* sink);
			
			/**
			 * getSink
			 *
			 * @return a pointer to the Trapezoid's Sink
			 *
			 * @brief Getter method for the sink attribute
			 */
			Sink* getSink();
			
			/**
			 * getNextAlongSegment
			 *
			 * @param const SegmentEndpoints2D* segment The segment that the Trapezoid is being split by
			 * @return pointer to the next Trapezoid
			 *
			 * @brief gets the underlying Trapezoid that intersects with the given segment
			 */		
			Trapezoid* getNextAlongSegment(const SegmentEndpoints2D* segment);

			
			/**
			 * merge
			 * 
			 * @access static
			 * @param Trapezoid* upper Upper Trapezoid to be merged
			 * @param Trapezoid* lower Lower Trapezoid to be merged
			 * 
			 * @brief merges two adjacent Trapezoids
			 */
			static void merge(Trapezoid* upper, Trapezoid* lower);
			
			/**
			 * tryToMerge
			 * 
			 * @brief checks if this Trapezoid can be merged with one above
			 */
			void tryToMerge();


			/**
			 * link
			 * 
			 * @access static
			 * @param Trapezoid* upper Pointer to the upper trapezoid
			 * @param Trapezoid* lower Pointer to the lower trapezoid
			 * 
			 * @brief Sets the right Trapezoid references after a horizontal split.
			 */
			static void link(Trapezoid* upper, Trapezoid* lower);

			/**
			 * unlink
			 * 
			 * @access static
			 * @param Trapezoid* upper Pointer to the upper trapezoid
			 * @param Trapezoid* lower Pointer to the lower trapezoid
			 *
			 * @brief Remove the mutual Trapezoid references in two given Trapezoids.  
			 */
			static void unlink(Trapezoid* upper, Trapezoid* lower);

			/**
			 * addUpper
			 * 
			 * @param Trapezoid* upper Pointer to the Trapezoid to be set
			 * 
			 * @brief This sets the correct upper references with the given Trapezoid as upper Trapezoid
			 */
			void addUpper(Trapezoid* upper);

			/**
			 * addLower
			 * 
			 * @param Trapezoid* lower Pointer to the Trapezoid to be set
			 * 
			 * @brief This sets the correct lower references with the given Trapezoid as lower Trapezoid
			 */
			void addLower(Trapezoid* lower);

			/**
			 * removeUpper
			 * 
			 * @param Trapezoid* removee The Trapezoid to be removed
			 * 
			 * @brief Remove the upper references to the given Trapezoid
			 */
			void removeUpper(Trapezoid* removee);

			/**
			 * removeLower
			 * 
			 * @param Trapezoid* removee The Trapezoid to be removed
			 * 
			 * @brief Remove the lower references to the given Trapezoid
			 */
			void removeLower(Trapezoid* removee);

			/**
			 * getID
			 * 
			 * @param Trapezoid* trapezoid The Trapezoid to get the ID for
			 * @return the Trapezoid ID as int, -1 if the trapezoid is NULL
			 * 
			 * @brief Wrapper for Trapezoid->ID, handles the (trapezoid == NULL) case
			 */
			static int getID(Trapezoid* trapezoid);

			/**
			 * flagOutside
			 * 
			 * @param bool isOutside Specifies if the Trapezoid is to be flagged outside the polygon
			 * 
			 * @brief flags the Trapezoid as outside or inside the polygon
			 */
			void flagOutside(bool isOutside);

			/**
			 * generateMonotoneChains
			 * 
			 * @return vector<vector<hpvec2>*>* Vector to the generated Monotone Chains
			 * 
			 * @brief generates the (up to two) monotone Chains representing the unimonotone polygons with this Trapezoid as topmost part
			 */
			vector<vector<hpvec2>*>* generateMonotoneChains();

			/**
			 * continueChain
			 * 
			 * @param vector<hpvec2>* chain Chain to be continued to build
			 * @param SegmentEndpoints2D* segment Segment the Chain is aligned to
			 * 
			 * @brief recursive call to build the monotone Chains representing the unimonotone polygons
			 */
			void continueChain(vector<hpvec2>* chain, SegmentEndpoints2D* segment);
			
			/**
			 * mid
			 * 
			 * @return hpvec2 Midpoint of the Trapezoid
			 * 
			 * @brief computes the Midpoint of a given Trapezoid
			 */
			hpvec2 mid();
			
			/**
			 * drawDebug
			 * 
			 * @brief draws some Debug output to the current OpenGL Context
			 *
			 * Gray lines are Trapezoid boundaries
			 * 
			 * Green Midpoint: Trapezoid flagged as outside
			 * Red Midpoint: Trapezoid flagged as inside
			 * Blue Midpoint: Trapezoid is not flagged
			 * 
			 * Green stippled Line to upper Neighbors
			 * Red stippled Line to lower Neighbors
			 * Blue stippled Line to upper Corner Neighbors
			 * Yellow stippled Line to lower Corner Neighbors
			 */
			void drawDebug();

		private:
			
			int ID;		/**< The Trapezoid ID, used for debugging output to prevent ugly pointers */
            static int currentID; /**< Trapezoid ID, for the next Trapezoid to be created */
			
			Sink* sink; /**< The sink object*/
			
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

			Trapezoid* upperCornerNeighbor = NULL; /**< Adjacent Trapezoid at a polygon corner above this */
			Trapezoid* lowerCornerNeighbor = NULL; /**< Adjacent Trapezoid at a polygon corner below this */

			enum MaybeBool {FALSE, TRUE, UNDEFINED}; /**< Boolean with additional unknown state */

			MaybeBool isOutside = MaybeBool::UNDEFINED; /**< Flag to specify if the Trapezoid is inside, outside or not yet flagged */
	};
}