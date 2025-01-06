// This is free and unencumbered software released into the public domain.
//
// Anyone is free to copy, modify, publish, use, compile, sell, or
// distribute this software, either in source code form or as a compiled
// binary, for any purpose, commercial or non-commercial, and by any means.

#include "Canvas.h"
#include <libheatmap/heatmap.h>
#include <libheatmap/colorschemes/PuBuGn.h>
#include <QPaintEvent>
#include <QPainter>
#include <QPainterPath>
#include <QBrush>
#include <QThreadPool>

Canvas::Canvas(const Preferences & _preferences, QWidget * _parent) :
    QWidget(_parent),
    mr_preferences(_preferences),
    mp_sdfs_image(nullptr),
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
    connect(this, SIGNAL(sdfImageReady(QImage*)), this, SLOT(onSdfImageReady(QImage*)));
}

Canvas::~Canvas()
{
    delete mp_sdfs_image;
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
    else if(!m_polygons.empty())
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
            foreach(const Polygon & polygon, m_polygons)
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
            foreach(const Polygon & inner_polygon, m_inner_polygons)
            {
                QPainterPath path;
                path.addPolygon(inner_polygon);
                painter.fillPath(path, brush_inner_polygon);
                painter.strokePath(path, pen_inner_polygon);
            }
        }
        if(mr_preferences.showDistanceMap())
        {
            if(mp_sdfs_image)
            {
                painter.drawImage(aabb.min, *mp_sdfs_image);
            }
            else
            {
                can_finish_update = false;
                drawSdfImage(aabb);
            }
        }
    }

    if(can_finish_update)
    {
        m_is_updating = false;
    }
}

void Canvas::drawSdfImage(const AABB & _aabb)
{
    if(mp_sdfs_image)
    {
        return;
    }
    setWaitingState(true);
    QThreadPool::globalInstance()->start([this, _aabb]() {
        const uint32_t min_x = static_cast<uint32_t>(_aabb.min.x());
        const uint32_t max_x = static_cast<uint32_t>(_aabb.max.x());
        const uint32_t min_y = static_cast<uint32_t>(_aabb.min.y());
        const uint32_t max_y = static_cast<uint32_t>(_aabb.max.y());
        const int32_t w = max_x - min_x;
        const int32_t h = max_y - min_y;
        if(w <= 0 || h <= 0)
        {
            return;
        }

        heatmap_t * hm = heatmap_new(w, h);
        PolygonDistance pd(m_polygons);
        for(uint32_t y = min_y; y < max_y; ++y)
        {
            for(uint32_t x = min_x; x < max_x; ++x)
            {
                double dist = pd.calculateUnsigned({static_cast<qreal>(x), static_cast<qreal>(y)});
                heatmap_add_weighted_point(hm, x - min_x, y - min_y, dist);
            }
        }
        std::vector<unsigned char> data(w * h * 4);
        heatmap_render_to(hm, heatmap_cs_PuBuGn_soft, data.data());
        heatmap_free(hm);

        QImage * image = new QImage(w, h, QImage::Format_RGBA8888);
        for(int32_t y = 0; y < h; ++y)
        {
            for(int32_t x = 0; x < w; ++x)
            {
                unsigned char * pixel = &data[(y * w + x) * 4];
                image->setPixelColor(x, y, QColor(pixel[0], pixel[1], pixel[2], 135));

            }
        }

        emit sdfImageReady(image, QPrivateSignal());
    });
}

void Canvas::onSdfImageReady(QImage * _pixmap)
{
    QImage * old = _pixmap;
    std::swap(mp_sdfs_image, old);
    if(old) delete old;
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
    delete mp_sdfs_image;
    mp_sdfs_image = nullptr;
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
        foreach(const Polygon & poligon, m_polygons)
        {
            Triangulation triangulation = triangulate(poligon);
            m_triangulations.push_back(triangulation);
            foreach(const Polygon & inner_polygon, calculateInnerPolygons(poligon, 20.0f))
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
