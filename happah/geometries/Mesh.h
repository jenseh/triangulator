#pragma once

#include <memory>
#include <vector>

#include "happah/HappahTypes.h"

using namespace std;

template<typename T>
class Mesh {
public:
	Mesh(vector<T>* verticesAndNormals, vector<hpuint>* indices);
	virtual ~Mesh();

	vector<T>* getVerticesAndNormals();
	vector<hpuint>* getIndices();

private:
	vector<T>* m_verticesAndNormals;
	vector<hpuint>* m_indices;

};

