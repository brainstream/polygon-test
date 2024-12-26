#pragma once

#include "Polygon.h"
#include "Triangulation.h"
#include <QWidget>

class Canvas : public QWidget
{
    Q_OBJECT
public:
    explicit Canvas(QWidget * _parent = nullptr);

protected:
    void paintEvent(QPaintEvent * _event);
    void mouseReleaseEvent(QMouseEvent * _event);
    void mouseMoveEvent(QMouseEvent * _event);
    void keyPressEvent(QKeyEvent * _event);

private:
    Polygon m_polygon;
    Triangulation m_triangulation;
    QList<QPointF> m_inner_polygon_points;
    bool m_is_drawing;
};
