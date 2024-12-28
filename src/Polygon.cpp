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
