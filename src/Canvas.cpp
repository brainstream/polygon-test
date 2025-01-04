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
#include <QThreadPool>

namespace {

QColor getGradientColor(double _k)
{
    if(_k <= 0.2)
    {
        double t = _k / 0.2;
        return QColor(
            static_cast<int>(173 + t * (144 - 173)),
            static_cast<int>(216 + t * (238 - 216)),
            static_cast<int>(230 - t * (230 - 144)));
    }
    else if(_k <= 0.4)
    {
        double t = (_k - 0.2) / 0.2;
        return QColor(
            static_cast<int>(144 + t * (240 - 144)),
            static_cast<int>(238 + t * (230 - 238)),
            static_cast<int>(144 - t * 144));
    }
    else if(_k <= 0.6)
    {
        double t = (_k - 0.4) / 0.2;
        return QColor(static_cast<int>(240 + t * (255 - 240)), static_cast<int>(230 - t * (230 - 165)), 0);
    }
    else if(_k <= 0.8)
    {
        double t = (_k - 0.6) / 0.2;
        return QColor(255, static_cast<int>(165 - t * 165), 0);
    }
    else
    {
        double t = (_k - 0.8) / 0.2;
        return QColor(static_cast<int>(255 - t * (255 - 139)), 0, 0);
    }
}

} // namespace

Canvas::Canvas(const Preferences & _preferences, QWidget * _parent) :
    QWidget(_parent),
    mr_preferences(_preferences),
    mp_sdfs_pixmap(nullptr),
    m_is_updating(false),
    m_is_drawing(false)
{
    QPalette pal = QPalette();
    pal.setColor(QPalette::Window, QColorConstants::LightGray);
    setAutoFillBackground(true);
    setPalette(pal);
    setWaitingState(false);
    setFocusPolicy(Qt::ClickFocus);
    setMouseTracking(true);
    connect(&mr_preferences, SIGNAL(changed()), this, SLOT(update()));
    connect(this, SIGNAL(sdfPixmapReady(QPixmap*)), this, SLOT(onSdfPixmapReady(QPixmap*)));
}

Canvas::~Canvas()
{
    delete mp_sdfs_pixmap;
}

void Canvas::setWaitingState(bool _waiting)
{
    QCursor crs = cursor();
    crs.setShape(_waiting ? Qt::WaitCursor : Qt::CrossCursor);
    setCursor(crs);
}

void Canvas::paintEvent(QPaintEvent * _event)
{
    static const QBrush brash_background(QColorConstants::DarkGray, Qt::BrushStyle::Dense7Pattern);

    static const QPen pen_drawing(QColorConstants::Color0);
    static const QPen pen_triangulation(QColor(224, 79, 43));

    static const QPen pen_inner_polygon(QColor(53, 143, 59, 125));
    static const QBrush brush_inner_polygon(QColor(53, 143, 59, 100));

    static const QPen pen_aabb(QColor(77, 94, 105));

    static const QBrush brush_polygon(QColor(99, 96, 96, 125));
    static const QPen pen_polygon(QColor(99, 96, 96), 3.0);

    bool can_finish_update = true;
    m_is_updating = true;

    QWidget::paintEvent(_event);
    QPainter painter(this);
    painter.setRenderHint(QPainter::Antialiasing);

    {
        QPainterPath path;
        path.addRect(rect());
        painter.fillPath(path, brash_background);
    }

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
        AABB aabb = calculateAABB(m_polygons);

        if(mr_preferences.showAABB())
        {
            painter.setPen(pen_aabb);
            painter.drawLine(aabb.min, QPointF(aabb.min.x(), aabb.max.y()));
            painter.drawLine(QPointF(aabb.min.x(), aabb.max.y()), aabb.max);
            painter.drawLine(aabb.max, QPointF(aabb.max.x(), aabb.min.y()));
            painter.drawLine(QPointF(aabb.max.x(), aabb.min.y()), aabb.min);
        }

        if(mr_preferences.showPolygons())
        {
            for(const Polygon & polygon : m_polygons)
            {
                QPainterPath path;
                path.addPolygon(polygon);
                path.closeSubpath();
                painter.fillPath(path, brush_polygon);
                painter.strokePath(path, pen_polygon);
            }
        }
        if(mr_preferences.showTriangulation())
        {
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

        if(mr_preferences.showOffsetPolygons())
        {
            for(const Polygon & inner_polygon : m_inner_polygons)
            {
                QPainterPath path;
                path.addPolygon(inner_polygon);
                painter.fillPath(path, brush_inner_polygon);
                painter.strokePath(path, pen_inner_polygon);
            }
        }

        if(mr_preferences.showSDF() && !m_sdfs.empty())
        {
            if(mp_sdfs_pixmap)
            {
                painter.drawPixmap(aabb.min, *mp_sdfs_pixmap);
            }
            else
            {
                can_finish_update = false;
                drawSdfPixmap(aabb);
            }
        }
    }

    if(can_finish_update)
    {
        m_is_updating = false;
    }
}

void Canvas::drawSdfPixmap(const AABB & _aabb)
{
    if(mp_sdfs_pixmap)
    {
        return;
    }
    setWaitingState(true);
    QThreadPool::globalInstance()->start([this, _aabb]() {
        const double w = _aabb.max.x() - _aabb.min.x();
        const double h = (_aabb.max.y() - _aabb.min.y());
        const double max_distance = std::sqrt(w * w + h * h) / 2;
        QPixmap * pixmap = new QPixmap(w, h);
        QPainter painter(pixmap);
        for(double y = _aabb.min.y(); y < _aabb.max.y(); y += 1.0)
        {
            for(double x = _aabb.min.x(); x < _aabb.max.x(); x += 1.0)
            {
                double dist = std::numeric_limits<double>::max();
                for(const auto & sdf : m_sdfs)
                {
                    tmd::Result d = sdf.signed_distance({x, y, 0.0});
                    double dst = std::abs(d.distance);
                    if(dst < dist)
                        dist = dst;
                }
                QColor qc = getGradientColor(dist / max_distance);
                qc.setAlpha(190);
                painter.setPen(qc);
                painter.drawPoint(QPoint(x, y) - _aabb.min);
            }
        }
        emit sdfPixmapReady(pixmap, QPrivateSignal());
    });
}

void Canvas::onSdfPixmapReady(QPixmap * _pixmap)
{
    mp_sdfs_pixmap = _pixmap;
    update();
    setWaitingState(false);
}

void Canvas::mouseReleaseEvent(QMouseEvent * _event)
{
    if(m_is_updating)
    {
        return;
    }
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
    m_sdfs.clear();
    delete mp_sdfs_pixmap;
    mp_sdfs_pixmap = nullptr;
}

void Canvas::mouseMoveEvent(QMouseEvent *)
{
    if(!m_is_updating && m_is_drawing)
    {
        update();
    }
}

void Canvas::keyPressEvent(QKeyEvent * _event)
{
    if(m_is_updating || (_event->key() != Qt::Key_Return && _event->key() != Qt::Key_Enter))
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
            Triangulation triangulation = triangulate(poligon);
            m_triangulations.push_back(triangulation);
            m_sdfs.push_back(calculateSDF(poligon, triangulation));
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
