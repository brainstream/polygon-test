#pragma once

#include "VertexDirection.h"
#include <QObject>

class Polygon : public QList<QPointF>
{
public:
    QList<QPointF> calculateInnerPolygon(float _padding) const;
    std::optional<VertexDirection> calculateDirection() const;
};
