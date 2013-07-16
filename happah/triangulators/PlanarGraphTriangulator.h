#pragma once

#include "happah/geometries/Segment.h"
#include "happah/geometries/TriangleMesh.h"

#include <iterator>

using namespace std;

class PlanarGraphTriangulator {

	public:
		static const PlanarGraphTriangulator& TRIANGULATOR;

		PlanarGraphTriangulator();
		virtual ~PlanarGraphTriangulator();

};