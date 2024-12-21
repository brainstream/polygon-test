#include "Canvas.h"
#include <QPaintEvent>
#include <QPainter>

Canvas::Canvas(PolygonMesh & _polygon, QWidget *_parent) :
    QWidget(_parent),
    mr_polygon(_polygon),
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
    if(m_points.empty())
        return;
    painter.setPen(color_regular_edge);
    for(int i = 1; i < m_points.size(); ++i)
        painter.drawLine(m_points[i - 1], m_points[i]);
    if(m_is_drawing)
    {
        painter.drawLine(m_points.last(), cursor().pos());
    }
    else if(mr_polygon.getTriangulation().getVertexCount() >= 3)
    {
        const Triangulation & triangulation = mr_polygon.getTriangulation();
        for(size_t i = 0; i < triangulation.getVertexCount(); i += 3)
        {
            uint32_t v1 = triangulation.getVertices()[i];
            uint32_t v2 = triangulation.getVertices()[i + 1];
            uint32_t v3 = triangulation.getVertices()[i + 2];

            if(triangulation.isInternalEdge(v1, v2))
                painter.setPen(color_removed_edge);
            else
                painter.setPen(color_regular_edge);
            painter.drawLine(m_points[v1] , m_points[v2]);

            if(triangulation.isInternalEdge(v2, v3))
                painter.setPen(color_removed_edge);
            else
                painter.setPen(color_regular_edge);
            painter.drawLine(m_points[v2] , m_points[v3]);

            if(triangulation.isInternalEdge(v3, v1))
                painter.setPen(color_removed_edge);
            else
                painter.setPen(color_regular_edge);
            painter.drawLine(m_points[v3] , m_points[v1]);
        }
    }

    painter.setPen(color_inner_polygon);
    for(int i = 1; i < m_inner_points.size(); ++i)
        painter.drawLine(m_inner_points[i - 1], m_inner_points[i]);
}

void Canvas::mouseReleaseEvent(QMouseEvent * _event)
{
    if(m_is_drawing)
    {
        m_points.push_back(_event->pos());
    }
    else
    {
        m_points.clear();
        m_inner_points.clear();
        m_points.push_back(_event->pos());
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
        return;
    m_is_drawing = false;
    if(m_points.size() <= 1)
    {
        m_points.clear();
        m_inner_points.clear();
    }
    else
    {
        mr_polygon.setVertices(m_points);
        m_inner_points = calculateInnerPolygon(m_points, 12.0f);
        if(!m_inner_points.empty())
            m_inner_points.push_back(m_inner_points.front());
        m_points.push_back(m_points.front());
    }
    m_is_drawing = false;
    update();
}
