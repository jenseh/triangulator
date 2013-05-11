#pragma once

#include <memory>

#include "happah/geometries/Mesh.h"

using namespace std;

template<typename T>
class TriangleMesh : public Mesh<T> {
public:
	TriangleMesh(vector<T>* verticesAndNormals, vector<hpuint>* indices);
	~TriangleMesh();

};
typedef TriangleMesh<hpvec2> TriangleMesh2D;
typedef shared_ptr<TriangleMesh2D> TriangleMesh2D_ptr;
typedef TriangleMesh<hpvec3> TriangleMesh3D;
typedef shared_ptr<TriangleMesh3D> TriangleMesh3D_ptr;

