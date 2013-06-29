#pragma once

#include "happah/HappahTypes.h"

template<typename T>
struct SegmentEndpoints {
SegmentEndpoints(T a, T b) : a(a), b(b) {};

T a;
T b;
};

typedef SegmentEndpoints<hpvec2> SegmentEndpoints2D;
typedef SegmentEndpoints<hpvec3> SegmentEndpoints3D;

hpdouble getSegment2DX(hpdouble y, const SegmentEndpoints2D* segment);

