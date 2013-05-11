#pragma once

#include "happah/triangulators/PlanarGraphTriangulator.h"

class PlanarGraphTriangulatorSeidel : public PlanarGraphTriangulator {

	class TrapezoidMesh2D {
		//TODO
	public:
		//LineMesh2D* toLineMesh(); //TODO
		TriangleMesh2D* toTriangleMesh();
	};

public:
	//returns trapezoidal decomposition of the interior of the given polygon according to Seidel's algorithm
	static void trapezoidulate(PlanarGraphSegmentEndpointsIterator first, PlanarGraphSegmentEndpointsIterator last, TrapezoidMesh2D& trapezoidMesh); // Alternative 2

	PlanarGraphTriangulatorSeidel();
	~PlanarGraphTriangulatorSeidel();

	TriangleMesh2D* triangulate(PlanarGraphSegmentEndpointsIterator first, PlanarGraphSegmentEndpointsIterator last);

};
