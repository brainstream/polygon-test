#pragma once

#include <QList>
#include <QPointF>

using Polygon = QList<QPointF>;
using PolygonList = QList<QList<QPointF>>;
using Triangulation = std::vector<uint32_t>;

PolygonList removePolygonSelfIntersections(const Polygon & _polygon);
PolygonList calculateInnerPolygons(const Polygon & _polygon, float _offset);
Triangulation triangulate(const Polygon & _polygon);
