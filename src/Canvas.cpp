// This is free and unencumbered software released into the public domain.
//
// Anyone is free to copy, modify, publish, use, compile, sell, or
// distribute this software, either in source code form or as a compiled
// binary, for any purpose, commercial or non-commercial, and by any means.

#include "Canvas.h"
#include <QPaintEvent>
#include <QPainter>
#include <QPainterPath>
#include <QBrush>

Canvas::Canvas(QWidget * _parent) :
    QWidget(_parent),
    m_is_drawing(false)
{
    QPalette pal = QPalette();
    pal.setColor(QPalette::Window, QColorConstants::LightGray);
    setAutoFillBackground(true);
    setPalette(pal);

    QCursor crs = cursor();
    crs.setShape(Qt::CrossCursor);
    setCursor(crs);

    setFocusPolicy(Qt::ClickFocus);
    setMouseTracking(true);
}

void Canvas::paintEvent(QPaintEvent * _event)
{
    static const QBrush brash_background(QColorConstants::DarkGray, Qt::BrushStyle::Dense7Pattern);

    static const QPen pen_drawing(QColorConstants::Color0);
    static const QPen pen_triangulation(QColor(224, 79, 43));

    static const QPen pen_inner_polygon(QColor(53, 143, 59, 125));
    static const QBrush brush_inner_polygon(QColor(53, 143, 59, 100));

    static const QBrush brush_polygon(QColor(99, 96, 96, 125));
    static const QPen pen_polygon(QColor(99, 96, 96), 3.0);


    QWidget::paintEvent(_event);
    QPainter painter(this);
    painter.setRenderHint(QPainter::Antialiasing);

    {
        QPainterPath path;
        path.addRect(rect());
        painter.fillPath(path, brash_background);
    }

    if(m_drawing.empty())
        return;
    painter.setPen(pen_drawing);

    if(m_is_drawing)
    {
        for(int i = 1; i < m_drawing.size(); ++i)
        {
            painter.drawLine(m_drawing[i - 1], m_drawing[i]);
        }
        painter.drawLine(m_drawing.last(), cursor().pos());
    }
    else
    {
        for(const Polygon & polygon : m_polygons)
        {
            QPainterPath path;
            path.addPolygon(polygon);
            path.closeSubpath();
            painter.fillPath(path, brush_polygon);
            painter.strokePath(path, pen_polygon);
        }
        painter.setPen(pen_triangulation);
        for(qsizetype i = 0; i < m_triangulations.size(); ++i)
        {
            const Triangulation & triangulation = m_triangulations[i];
            const Polygon & polygon = m_polygons[i];
            for(size_t i = 0; i < triangulation.size(); i += 3)
            {
                uint32_t v1 = triangulation[i];
                uint32_t v2 = triangulation[i + 1];
                uint32_t v3 = triangulation[i + 2];
                painter.drawLine(polygon[v1], polygon[v2]);
                painter.drawLine(polygon[v2], polygon[v3]);
                painter.drawLine(polygon[v3], polygon[v1]);
            }
        }
    }


    for(const Polygon & inner_polygon : m_inner_polygons)
    {
        QPainterPath path;
        path.addPolygon(inner_polygon);
        painter.fillPath(path, brush_inner_polygon);
        painter.strokePath(path, pen_inner_polygon);
    }
}

void Canvas::mouseReleaseEvent(QMouseEvent * _event)
{
    if(m_is_drawing)
    {
        m_drawing.push_back(_event->pos());
    }
    else
    {
        clear();
        m_drawing.push_back(_event->pos());
        m_is_drawing = true;
    }
    update();
}

void Canvas::clear()
{
    m_drawing.clear();
    m_inner_polygons.clear();
    m_polygons.clear();
    m_triangulations.clear();
}

void Canvas::mouseMoveEvent(QMouseEvent *)
{
    if(m_is_drawing)
    {
        update();
    }
}

void Canvas::keyPressEvent(QKeyEvent * _event)
{
    if(_event->key() != Qt::Key_Return && _event->key() != Qt::Key_Enter)
    {
        return;
    }
    m_is_drawing = false;
    if(m_drawing.size() <= 1)
    {
        clear();
    }
    else
    {
        m_polygons = removePolygonSelfIntersections(m_drawing);
        m_triangulations.reserve(m_polygons.size());
        for(const Polygon & poligon : m_polygons)
        {
            m_triangulations.push_back(triangulate(poligon));
            for(const Polygon & inner_polygon : calculateInnerPolygons(poligon, 20.0f))
                m_inner_polygons.push_back(inner_polygon);
        }
        for(Polygon & inner_polygon : m_inner_polygons)
        {
            if(!inner_polygon.empty())
            {
                inner_polygon.push_back(inner_polygon.front());
            }
        }
        m_drawing.push_back(m_drawing.front());
    }
    m_is_drawing = false;
    update();
}
