#pragma once

#include "VertexDirection.h"
#include <QPointF>
#include <QList>
#include <optional>

class Line
{
public:
    Line(const QPointF & _p1, const QPointF & _p2) :
        mc_p1(_p1),
        mc_p2(_p2)
    {
    }

    const QPointF & getPoint1() const
    {
        return mc_p1;
    }

    const QPointF & getPoint2() const
    {
        return mc_p2;
    }

    QPointF toVector() const
    {
        return mc_p1 - mc_p2;
    }

    QPointF toUnitVector() const;
    QPointF toNormalVector(VertexDirection _direction) const;
    Line calculateParallelLine(float _offset, VertexDirection _direction) const;
    std::optional<QPointF> calculateInterception(const Line & _other_edge) const;
    bool isCoDirectional(const Line & _other_line) const;

private:
    const QPointF mc_p1;
    const QPointF mc_p2;
};


class LineList : public QList<Line>
{
public:
    LineList()
    {
    }

    explicit LineList(const QList<QPointF> & _vertices);
};
