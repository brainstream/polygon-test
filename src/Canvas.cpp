#include "Canvas.h"
#include <QPaintEvent>
#include <QPainter>

Canvas::Canvas(QWidget *_parent) :
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
    static const QPen color_regular_edge(QColorConstants::Color0);
    static const QPen color_removed_edge(QColorConstants::Red);
    static const QPen color_inner_polygon(QColorConstants::Blue);

    QWidget::paintEvent(_event);
    QPainter painter(this);
    if(m_polygon.empty())
        return;
    painter.setPen(color_regular_edge);
    for(int i = 1; i < m_polygon.size(); ++i)
        painter.drawLine(m_polygon[i - 1], m_polygon[i]);
    if(m_is_drawing)
    {
        painter.drawLine(m_polygon.last(), cursor().pos());
    }
    else if(m_triangulation.getVertexCount() >= 3)
    {
        for(size_t i = 0; i < m_triangulation.getVertexCount(); i += 3)
        {
            uint32_t v1 = m_triangulation.getVertices()[i];
            uint32_t v2 = m_triangulation.getVertices()[i + 1];
            uint32_t v3 = m_triangulation.getVertices()[i + 2];

            if(m_triangulation.isInternalEdge(v1, v2))
                painter.setPen(color_removed_edge);
            else
                painter.setPen(color_regular_edge);
            painter.drawLine(m_polygon[v1] , m_polygon[v2]);

            if(m_triangulation.isInternalEdge(v2, v3))
                painter.setPen(color_removed_edge);
            else
                painter.setPen(color_regular_edge);
            painter.drawLine(m_polygon[v2] , m_polygon[v3]);

            if(m_triangulation.isInternalEdge(v3, v1))
                painter.setPen(color_removed_edge);
            else
                painter.setPen(color_regular_edge);
            painter.drawLine(m_polygon[v3] , m_polygon[v1]);
        }
    }

    painter.setPen(color_inner_polygon);
    for(const QList<QPointF> & inner_polygon : m_inner_polygons)
    {
        for(int i = 1; i < inner_polygon.size(); ++i)
            painter.drawLine(inner_polygon[i - 1], inner_polygon[i]);
    }
}

void Canvas::mouseReleaseEvent(QMouseEvent * _event)
{
    if(m_is_drawing)
    {
        m_polygon.push_back(_event->pos());
    }
    else
    {
        m_polygon.clear();
        m_inner_polygons.clear();
        m_polygon.push_back(_event->pos());
        m_is_drawing = true;
    }
    update();
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
    if(m_polygon.size() <= 1)
    {
        m_polygon.clear();
        m_inner_polygons.clear();
    }
    else
    {
        m_triangulation.calculate(m_polygon);
        m_inner_polygons = m_polygon.calculateInnerPolygon(62.0f);
        for(QList<QPointF> & inner_polygon : m_inner_polygons)
        {
            if(!inner_polygon.empty())
                inner_polygon.push_back(inner_polygon.front());
        }
        m_polygon.push_back(m_polygon.front());
    }
    m_is_drawing = false;
    update();
}
