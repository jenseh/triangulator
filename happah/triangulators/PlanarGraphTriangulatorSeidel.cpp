#include "happah/triangulators/PlanarGraphTriangulatorSeidel.h"

PlanarGraphTriangulatorSeidel::PlanarGraphTriangulatorSeidel() {}

PlanarGraphTriangulatorSeidel::~PlanarGraphTriangulatorSeidel() {}

void PlanarGraphTriangulatorSeidel::trapezoidulate(PlanarGraphSegmentEndpointsIterator begin, PlanarGraphSegmentEndpointsIterator end, TrapezoidMesh2D& trapezoidation) {
	
}

TriangleMesh2D* PlanarGraphTriangulatorSeidel::TrapezoidMesh2D::toTriangleMesh() {
	
	std::vector<hpvec2>* verticesAndNormals = new std::vector<hpvec2>();
	std::vector<hpuint>* indices = new std::vector<hpuint>();
	verticesAndNormals->emplace_back(0.0f, 0.0f);
	verticesAndNormals->emplace_back();
	verticesAndNormals->emplace_back(0.5f, 0.0f);
	verticesAndNormals->emplace_back();
	verticesAndNormals->emplace_back(0.0f, 0.5f);
	verticesAndNormals->emplace_back();
	indices->push_back(0);
	indices->push_back(1);
	indices->push_back(2);
	return new TriangleMesh2D(verticesAndNormals, indices);
}

TriangleMesh2D* PlanarGraphTriangulatorSeidel::triangulate(PlanarGraphSegmentEndpointsIterator first, PlanarGraphSegmentEndpointsIterator last) {
	
	for(PlanarGraphSegmentEndpointsIterator it = first; it != last; it++) {
		SegmentEndpoints2D seg = *it;
	}
	
	TrapezoidMesh2D trapezoidMesh;
	trapezoidulate(first, last, trapezoidMesh);
	return trapezoidMesh.toTriangleMesh();
}

