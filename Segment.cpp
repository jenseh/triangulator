#include "happah/geometries/Segment.h"

hpdouble getSegment2DX(hpdouble y, const SegmentEndpoints2D* segment) {
hpdouble m = (segment->b.y - segment->a.y) / (segment->b.x - segment->a.x);
hpdouble b = segment->a.y - m * segment->a.x;
hpdouble x = (y - b) / m;
return x;
}