#include "happah/triangulators/PlanarGraphTriangulator.h"
#include "happah/triangulators/PlanarGraphTriangulatorSeidel.h"

const PlanarGraphTriangulator& PlanarGraphTriangulator::TRIANGULATOR = PlanarGraphTriangulatorSeidel();

PlanarGraphTriangulator::PlanarGraphTriangulator() {}

PlanarGraphTriangulator::~PlanarGraphTriangulator() {}
