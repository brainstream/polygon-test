#pragma once

#include <QObject>
#include <QList>
#include <QPointF>

class Polygon : public QList<QPointF>
{
public:
    QList<QList<QPointF>> calculateInnerPolygons(float _offset) const;
};
