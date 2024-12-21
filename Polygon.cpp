#include "Polygon.h"
#include "Math.h"
#include <optional>
#include <cmath>

namespace {

enum class Direction
{
    Clockwise,
    Counterclockwise
};

std::optional<Direction> getDirection(const QList<QPointF> & _polygon)
{
    const float signed_area = calculateSignedArea(_polygon);
    if(signed_area == 0.0f)
        return std::nullopt;
    return signed_area > 0.0f ? Direction::Clockwise : Direction::Counterclockwise;
}

class Edge
{
public:
    Edge(const QPointF & _p1, const QPointF & _p2);
    const QPointF & getPoint1() const;
    const QPointF & getPoint2() const;
    std::optional<Edge> calculateParallelOffset(float _offset, Direction _direction) const;
    std::optional<QPointF> calculateInterception(const Edge & _other_edge) const;

private:
    std::optional<QPointF> calculateNormalVector(Direction _direction) const;
    std::optional<QPointF> calculateUnitVector() const;

private:
    const QPointF mc_p1;
    const QPointF mc_p2;
};

} // namespace

QList<QPointF> calculateInnerPolygon(const QList<QPointF> & _polygon, float _padding)
{
    const std::optional<Direction> direction = getDirection(_polygon);
    if(!direction.has_value())
        return QList<QPointF>();
    QList<Edge> edges;
    edges.reserve(_polygon.size());
    QPointF vertex_a = _polygon[_polygon.size() - 1];
    for(const QPointF & vertex_b: _polygon)
    {
        Edge edge(vertex_a, vertex_b);
        std::optional<Edge> parallel_edge = edge.calculateParallelOffset(_padding, *direction);
        if(parallel_edge.has_value())
            edges.push_back(*parallel_edge);
        vertex_a = vertex_b;
    }

    QList<QPointF> result;
    result.reserve(_polygon.size());

    const Edge * prev_edge = &edges.last();
    for(const Edge & edge : edges)
    {
        std::optional<QPointF> point = prev_edge->calculateInterception(edge);
        if(point.has_value())
            result.push_back(*point);
        prev_edge = &edge;
    }

    return result;
}

Edge::Edge(const QPointF & _p1, const QPointF & _p2) :
    mc_p1(_p1),
    mc_p2(_p2)
{
}

inline const QPointF & Edge::getPoint1() const
{
    return mc_p1;
}

inline const QPointF & Edge::getPoint2() const
{
    return mc_p2;
}

std::optional<Edge> Edge::calculateParallelOffset(float _offset, Direction _direction) const
{
    std::optional<QPointF> normal = calculateNormalVector(_direction);
    if(normal.has_value())
    {
        QPointF offset_vector = *normal * _offset;
        return Edge(mc_p1 + offset_vector, mc_p2 + offset_vector);
    }
    return std::nullopt;
}

std::optional<QPointF> Edge::calculateNormalVector(Direction _direction) const
{
    std::optional<QPointF> unit_vector = calculateUnitVector();
    if(!unit_vector.has_value())
        return std::nullopt;
    return _direction == Direction::Clockwise
        ? QPointF(unit_vector->y(), -unit_vector->x())
        : QPointF(-unit_vector->y(), unit_vector->x());
}

std::optional<QPointF> Edge::calculateUnitVector() const
{
    const QPointF diff = mc_p1 - mc_p2;
    const float length = std::sqrt(diff.x() * diff.x() + diff.y() * diff.y());
    if(length == 0.0f)
        return std::nullopt;
    const float x = diff.x() / length;
    const float y = diff.y() / length;
    return QPointF(x, y);
}

// https://en.wikipedia.org/wiki/Line%E2%80%93line_intersection#Given_two_points_on_each_line
std::optional<QPointF> Edge::calculateInterception(const Edge & _other_edge) const
{
    const float x1 = mc_p1.x();
    const float y1 = mc_p1.y();
    const float x2 = mc_p2.x();
    const float y2 = mc_p2.y();
    const float x3 = _other_edge.mc_p1.x();
    const float y3 = _other_edge.mc_p1.y();
    const float x4 = _other_edge.mc_p2.x();
    const float y4 = _other_edge.mc_p2.y();

    const float denominator = (x1 - x2) * (y3 - y4) - (y1 - y2) * (x3 - x4);
    if(denominator == 0.0f)
        return std::nullopt;
    const float k1 = x1 * y2 - y1 * x2;
    const float k2 = x3 * y4 - y3 * x4;
    const float x = (k1 * (x3 - x4) - k2 * (x1 - x2)) / denominator;
    const float y = (k1 * (y3 - y4) - k2 * (y1 - y2)) / denominator;
    return QPointF(x, y);
}
