#include "Line.h"

Line Line::calculateParallelLine(float _offset, VertexDirection _direction) const
{
    QPointF normal = toNormalVector(_direction);
    QPointF offset_vector = normal * _offset;
    return Line(mc_p1 + offset_vector, mc_p2 + offset_vector);
}

QPointF Line::toNormalVector(VertexDirection _direction) const
{
    QPointF unit_vector = toUnitVector();
    return _direction == VertexDirection::Clockwise
       ? QPointF(unit_vector.y(), -unit_vector.x())
       : QPointF(-unit_vector.y(), unit_vector.x());
}

QPointF Line::toUnitVector() const
{
    const QPointF diff = toVector();
    const float length = std::sqrt(diff.x() * diff.x() + diff.y() * diff.y());
    if(length == 0.0f)
        return QPointF();
    const float x = diff.x() / length;
    const float y = diff.y() / length;
    return QPointF(x, y);
}

std::optional<QPointF> Line::calculateInterception(const Line & _other_edge) const
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

bool Line::isCoDirectional(const Line & _other_line) const
{
    QPointF this_vector = toVector();
    QPointF other_vector = _other_line.toVector();
    bool this_signs[2] { this_vector.x() < 0, this_vector.y() < 0 };
    bool other_signs[2] { other_vector.x() < 0, other_vector.y() < 0 };
    return this_signs[0] == other_signs[0] && this_signs[1] == other_signs[1];
}

LineList::LineList(const QList<QPointF> & _vertices)
{
    reserve(_vertices.size());
    qsizetype vertex_a_idx = 0;
    qsizetype vertex_b_idx;
    for(qsizetype i = 1; i <= _vertices.size(); ++i)
    {
        vertex_b_idx = i == _vertices.size() ? 0 : i;
        emplace_back(_vertices[vertex_a_idx], _vertices[vertex_b_idx]);
        vertex_a_idx = vertex_b_idx;
    }
}
