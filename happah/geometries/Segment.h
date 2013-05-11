#pragma once

#include "happah/HappahTypes.h"

template<typename T>
struct SegmentEndpoints {
	T& a;
	T& b;
	
	SegmentEndpoints(T& a, T& b) : a(a), b(b) {}
};

typedef SegmentEndpoints<hpvec2> SegmentEndpoints2D;
typedef SegmentEndpoints<hpvec3> SegmentEndpoints3D;

