#pragma once

#include "Polygon.h"
#include <QWidget>

class Canvas : public QWidget
{
    Q_OBJECT
public:
    explicit Canvas(PolygonMesh & _polygon, QWidget * _parent = nullptr);

protected:
    void paintEvent(QPaintEvent * _event);
    void mouseReleaseEvent(QMouseEvent * _event);
    void mouseMoveEvent(QMouseEvent * _event);
    void keyPressEvent(QKeyEvent * _event);

private:
    PolygonMesh & mr_polygon;
    QList<QPointF> m_points;
    QList<QPointF> m_inner_points;
    bool m_is_drawing;
};
