#pragma once

#include "happah/geometries/Segment.h"
#include "happah/geometries/TriangleMesh.h"

#include <iterator>

using namespace std;

//typedef iterator<random_access_iterator_tag, const SegmentEndpoints2D> PlanarGraphSegmentEndpointsIterator;
typedef vector<SegmentEndpoints2D>::iterator PlanarGraphSegmentEndpointsIterator;

class PlanarGraphTriangulator {

public:
	static const PlanarGraphTriangulator& TRIANGULATOR;

	PlanarGraphTriangulator();
	virtual ~PlanarGraphTriangulator();

	virtual TriangleMesh2D* triangulate(PlanarGraphSegmentEndpointsIterator first, PlanarGraphSegmentEndpointsIterator last) = 0;

};
