#include "Mesh.h"

template<typename T>
Mesh<T>::Mesh(vector<T>* verticesAndNormals, vector<hpuint>* indices)
		:m_verticesAndNormals(verticesAndNormals),m_indices(indices) {}

template<typename T>
Mesh<T>::~Mesh() {
	delete m_verticesAndNormals;
	delete m_indices;
}

template<typename T>
vector<T>* Mesh<T>::getVerticesAndNormals() {
	return m_verticesAndNormals;
}

template<typename T>
vector<hpuint>* Mesh<T>::getIndices() {
	return m_indices;
}

template class Mesh<hpvec2>;
template class Mesh<hpvec3>;

