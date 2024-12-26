#include "Polygon.h"
#include "Line.h"
#include <QSet>
#include <optional>
#include <cmath>

namespace {

float calculateSignedArea(const QList<QPointF> & _points)
{
    if(_points.size() < 3)
    {
        return 0.0f;
    }
    float sum = _points[0].x() * (_points[1].y() - _points[_points.size() - 1].y()) +
                _points[_points.size() - 1].x() * (_points[0].y() - _points[_points.size() - 2].y());
    for(qsizetype i = 1; i < _points.size() - 1; ++i)
    {
        sum += _points[i].x() * (_points[i + 1].y() - _points[i - 1].y());
    }
    return 0.5f * sum;
}

QList<QPointF> calculatePolygonFromLines(const QList<Line> & _lines)
{
    QList<QPointF> result;
    result.reserve(_lines.size());
    const Line * prev_line = &_lines.last();
    for(const Line & line : _lines)
    {
        std::optional<QPointF> point = prev_line->calculateInterception(line);
        if(point.has_value())
            result.push_back(*point);
        prev_line = &line;
    }
    return result;
}

} // namespace

QList<QPointF> Polygon::calculateInnerPolygon(float _padding) const
{
    const std::optional<VertexDirection> direction = calculateDirection();
    if(!direction.has_value())
        return QList<QPointF>();

    LineList origin_edges(*this);
    LineList inner_lines;
    inner_lines.reserve(origin_edges.size());
    for(qsizetype i = 0; i < origin_edges.size(); ++i)
    {
        const Line & origin_edge = origin_edges[i];
        const std::optional<Line> inner_edge = origin_edge.calculateParallelLine(_padding, *direction);
        if(inner_edge.has_value())
            inner_lines.push_back(*inner_edge);
    }

    QList<QPointF> result = calculatePolygonFromLines(inner_lines);
    if(result.size() != size())
    {
        // TODO: error?
        return result;
    }

    return result;
}

std::optional<VertexDirection> Polygon::calculateDirection() const
{
    const float signed_area = calculateSignedArea(*this);
    if(signed_area == 0.0f)
        return std::nullopt;
    return signed_area > 0.0f ? VertexDirection::Clockwise : VertexDirection::Counterclockwise;
}
