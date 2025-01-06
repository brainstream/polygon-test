// This is free and unencumbered software released into the public domain.
//
// Anyone is free to copy, modify, publish, use, compile, sell, or
// distribute this software, either in source code form or as a compiled
// binary, for any purpose, commercial or non-commercial, and by any means.

#include "Polygon.h"
#include <clipper2/clipper.h>
#include <mapbox/earcut.hpp>

namespace {

void mapPolygonToPath(const Polygon & _polygon, Clipper2Lib::PathD & _path)
{
    _path.reserve(_polygon.size() + 1);
    for(const QPointF & point: _polygon)
    {
        _path.emplace_back(point.x(), point.y());
    }
    _path.emplace_back(_polygon.front().x(), _polygon.front().y());
}

void mapPathsToPolygonList(const Clipper2Lib::PathsD & _paths, PolygonList & _polygons)
{
    _polygons.reserve(_paths.size());
    for(const Clipper2Lib::PathD & path : _paths)
    {
        Polygon result_points;
        result_points.reserve(path.size());
        for(const Clipper2Lib::PointD & point : path)
        {
            result_points.emplace_back(point.x, point.y);
        }
        _polygons.push_back(result_points);
    }
}

} // namespace

namespace mapbox {
namespace util {

template <>
struct nth<0, QPointF>
{
    static auto get(const QPointF & _t) { return _t.x(); };
};

template <>
struct nth<1, QPointF>
{
    static auto get(const QPointF & _t) { return _t.y(); };
};

} // namespace util
} // namespace mapbox

PolygonList removePolygonSelfIntersections(const Polygon & _polygon)
{
    Clipper2Lib::PathD subject;
    mapPolygonToPath(_polygon, subject);
    Clipper2Lib::PathsD solution = Clipper2Lib::Difference(
        {subject},
        {},
        Clipper2Lib::FillRule::EvenOdd);
    PolygonList result;
    mapPathsToPolygonList(solution, result);
    return result;
}

PolygonList calculateInnerPolygons(const Polygon & _polygon, float _offset)
{
    Clipper2Lib::PathsD polyline, solution;
    Clipper2Lib::PathD subject;
    mapPolygonToPath(_polygon, subject);
    polyline.push_back(subject);
    solution = Clipper2Lib::InflatePaths(
        polyline,
        -_offset,
        Clipper2Lib::JoinType::Square,
        Clipper2Lib::EndType::Polygon);
    PolygonList result;
    mapPathsToPolygonList(solution, result);
    return result;
}

Triangulation triangulate(const Polygon & _polygon)
{
    return mapbox::earcut(PolygonList{ _polygon });
}

AABB calculateAABB(const PolygonList & _polygons)
{
    if(_polygons.empty())
        return {};
    AABB aabb = calculateAABB(_polygons[0]);
    for(qsizetype i = 1; i < _polygons.size(); ++i)
    {
        AABB next_aabb = calculateAABB(_polygons[i]);
        if(next_aabb.min.x() < aabb.min.x())
            aabb.min.setX(next_aabb.min.x());
        if(next_aabb.min.y() < aabb.min.y())
            aabb.min.setY(next_aabb.min.y());
        if(next_aabb.max.x() > aabb.max.x())
            aabb.max.setX(next_aabb.max.x());
        if(next_aabb.max.y() > aabb.max.y())
            aabb.max.setY(next_aabb.max.y());
    }
    return aabb;
}

AABB calculateAABB(const Polygon & _polygon)
{
    if(_polygon.empty())
        return {};
    AABB aabb { .min = _polygon[0], .max = _polygon[0] };
    for(qsizetype i = 1; i < _polygon.size(); ++i)
    {
        if(_polygon[i].x() < aabb.min.x())
            aabb.min.setX(_polygon[i].x());
        else if(_polygon[i].x() > aabb.max.x())
            aabb.max.setX(_polygon[i].x());
        if(_polygon[i].y() < aabb.min.y())
            aabb.min.setY(_polygon[i].y());
        else if(_polygon[i].y() > aabb.max.y())
            aabb.max.setY(_polygon[i].y());
    }
    return aabb;
}

struct PolygonDistance::Edge
{
    QPointF a;
    QPointF b;
    QPointF diff;
    qreal squared_length;
};

PolygonDistance::PolygonDistance(const PolygonList & _polygons)
{
    qsizetype size = 0;
    for(const Polygon & polygon : _polygons)
        size += polygon.size();
    m_edges.reserve(size);

    for(const Polygon & polygon : _polygons)
    {
        if(polygon.size() < 3) continue;
        const QPointF * a = &polygon.at(0);
        for(qsizetype i = 1; i < polygon.size(); ++i)
        {
            const QPointF * b = &polygon.at(i);
            m_edges.push_back(makeEdge(*a, *b));
            a = b;
        }
        m_edges.push_back(makeEdge(polygon.at(0), *a));
    }
}

PolygonDistance::~PolygonDistance()
{
    foreach(Edge * edge, m_edges)
        delete edge;
}

PolygonDistance::Edge * PolygonDistance::makeEdge(const QPointF & _a, const QPointF & _b)
{
    QPointF::dotProduct(_a, _b);
    return new Edge
    {
        .a = _a,
        .b = _b,
        .diff = _b - _a,
        .squared_length = calculateSquaredLength(_a, _b)
    };
}

qreal PolygonDistance::calculateSquaredLength(const QPointF & _vec_a, const QPointF & _vec_b)
{
    const QPointF diff = _vec_a - _vec_b;
    return QPointF::dotProduct(diff, diff);
}

qreal PolygonDistance::calculateUnsigned(const QPointF & _point) const
{
    qreal result = std::numeric_limits<qreal>::max();
    for(const Edge * edge : m_edges)
    {
        const QPointF ap = _point - edge->a;
        const qreal dot = QPointF::dotProduct(ap, edge->diff);
        qreal t = dot / edge->squared_length;

        if(t <= 0.0) t = 0.0;
        else if(t > 1.0) t = 1.0;

        const QPointF closest_point = edge->a + t * edge->diff;

        const qreal sq_dist = calculateSquaredLength(_point, closest_point);
        const qreal dist = std::sqrt(sq_dist);
        if(dist < result)
            result = dist;
    }
    return result;
}
