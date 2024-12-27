#include "Polygon.h"
#include <clipper2/clipper.h>

QList<QList<QPointF> > Polygon::calculateInnerPolygon(float _offset) const
{
    Clipper2Lib::PathsD polyline, solution;
    std::vector<Clipper2Lib::PointD> points;
    points.reserve(size() + 1);
    for(const QPointF & point: *this)
    {
        points.emplace_back(point.x(), point.y());
    }
    points.push_back({at(0).x(), at(0).y()});
    polyline.push_back(points);
    solution = InflatePaths(polyline, -_offset, Clipper2Lib::JoinType::Miter, Clipper2Lib::EndType::Polygon);
    QList<QList<QPointF>> result;
    result.reserve(solution.size());
    for(const Clipper2Lib::PathD & path : solution)
    {
        QList<QPointF> result_points;
        result_points.reserve(path.size());
        for(const Clipper2Lib::PointD & point : path) // FIXME: invert
        {
            result_points.emplace_back(point.x, point.y);
        }
        result.push_back(result_points);
    }
    return result;
}
