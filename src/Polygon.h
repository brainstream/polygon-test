// This is free and unencumbered software released into the public domain.
//
// Anyone is free to copy, modify, publish, use, compile, sell, or
// distribute this software, either in source code form or as a compiled
// binary, for any purpose, commercial or non-commercial, and by any means.

#pragma once

#include "AABB.h"
#include <cstdint>
#include <Discregrid/geometry/TriangleMeshDistance.h>
#include <QList>

using Polygon = QList<QPointF>;
using PolygonList = QList<QList<QPointF>>;
using Triangulation = std::vector<uint32_t>;
using SDF = Discregrid::TriangleMeshDistance;

PolygonList removePolygonSelfIntersections(const Polygon & _polygon);
PolygonList calculateInnerPolygons(const Polygon & _polygon, float _offset);
Triangulation triangulate(const Polygon & _polygon);
SDF calculateSDF(const Polygon & _polygon, const Triangulation & _triangulation);
AABB calculateAABB(const Polygon & _polygon);
AABB calculateAABB(const PolygonList & _polygons);
