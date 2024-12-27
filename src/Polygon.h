#pragma once

#include <QObject>
#include <QList>
#include <QPointF>

class Polygon : public QList<QPointF>
{
public:
    QList<QList<QPointF>> calculateInnerPolygon(float _offset) const;
};
