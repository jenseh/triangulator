#include "happah/geometries/TriangleMesh.h"

template<typename T>
TriangleMesh<T>::TriangleMesh(vector<T>* verticesAndNormals,vector<hpuint>* indices)
		: Mesh<T>(verticesAndNormals, indices) {}

template<typename T>
TriangleMesh<T>::~TriangleMesh() {}

template class TriangleMesh<hpvec2>;
template class TriangleMesh<hpvec3>;

