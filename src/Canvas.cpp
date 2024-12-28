#include "Canvas.h"
#include <QPaintEvent>
#include <QPainter>
#include <QPainterPath>
#include <QBrush>

#include <clipper2/clipper.h>

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

    m_polygon = Polygon
    {
        {0, 0},
        {200, 400},
        {400, 0},
        {0, 200},
        {400, 200},
        {0, 0}
    };

    using Path = std::vector<Clipper2Lib::PointD>;
    Path polygon = {
        {0, 0},
        {200, 400},
        {400, 0},
        {0, 200},
        {400, 200},
        {0, 0}
    };
    Clipper2Lib::PathsD solution = Clipper2Lib::Union({polygon}, {}, Clipper2Lib::FillRule::EvenOdd);
    for(const auto & poly : solution) {
        Polygon polygon;
        for (const auto& point : poly) {
            polygon.emplace_back(point.x, point.y);
        }
        m_inner_polygons.push_back(polygon);
    }
}

void Canvas::paintEvent(QPaintEvent * _event)
{
    static const QPen pen_edge(QColorConstants::Color0);
    static const QPen pen_triangulation(QColorConstants::Red);
    static const QBrush brush_inner_polygon(QColor(53, 143, 59, 125));

    QWidget::paintEvent(_event);
    QPainter painter(this);
    if(m_polygon.empty())
        return;
    painter.setPen(pen_edge);
    for(int i = 1; i < m_polygon.size(); ++i)
        painter.drawLine(m_polygon[i - 1], m_polygon[i]);
    if(m_is_drawing)
    {
        painter.drawLine(m_polygon.last(), cursor().pos());
    }
    else if(m_triangulation.size() >= 3)
    {
        painter.setPen(pen_triangulation);
        for(size_t i = 0; i < m_triangulation.size(); i += 3)
        {
            uint32_t v1 = m_triangulation[i];
            uint32_t v2 = m_triangulation[i + 1];
            uint32_t v3 = m_triangulation[i + 2];
            painter.drawLine(m_polygon[v1] , m_polygon[v2]);
            painter.drawLine(m_polygon[v2] , m_polygon[v3]);
            painter.drawLine(m_polygon[v3] , m_polygon[v1]);
        }
    }

    for(const Polygon & inner_polygon : m_inner_polygons)
    {
        QPainterPath path;
        path.addPolygon(inner_polygon);
        painter.fillPath(path, brush_inner_polygon);
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
        m_triangulation = triangulate(m_polygon);
        m_inner_polygons = calculateInnerPolygons(m_polygon, 62.0f);
        for(Polygon & inner_polygon : m_inner_polygons)
        {
            if(!inner_polygon.empty())
                inner_polygon.push_back(inner_polygon.front());
        }
        m_polygon.push_back(m_polygon.front());
    }
    m_is_drawing = false;
    update();
}
