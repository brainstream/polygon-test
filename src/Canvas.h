#pragma once

#include "Polygon.h"
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
    void clear();

private:
    Polygon m_drawing;
    PolygonList m_polygons;
    QList<Triangulation> m_triangulations;
    PolygonList m_inner_polygons;
    bool m_is_drawing;
};
