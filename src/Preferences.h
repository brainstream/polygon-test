// This is free and unencumbered software released into the public domain.
//
// Anyone is free to copy, modify, publish, use, compile, sell, or
// distribute this software, either in source code form or as a compiled
// binary, for any purpose, commercial or non-commercial, and by any means.

#pragma once

#include <QObject>
#include <QSettings>

class Preferences : public QObject
{
    Q_OBJECT

public:
    explicit Preferences(QObject * _parent = nullptr) :
        QObject(_parent),
        m_show_polygons(true),
        m_show_triangulation(true),
        m_show_offset_polygons(true),
        m_show_aabb(false),
        m_show_distance_map(false)
    {
    }

    void save(QSettings & _settings) const;
    void load(QSettings & _settings);

public slots:
    bool showPolygons() const
    {
        return m_show_polygons;
    }

    void setShowPolygons(bool _show)
    {
        if(m_show_polygons != _show)
        {
            m_show_polygons = _show;
            emit changed();
        }
    }

    bool showTriangulation() const
    {
        return m_show_triangulation;
    }

    void setShowTriangulation(bool _show)
    {
        if(m_show_triangulation != _show)
        {
            m_show_triangulation = _show;
            emit changed();
        }
    }

    bool showOffsetPolygons() const
    {
        return m_show_offset_polygons;
    }

    void setShowOffsetPolygons(bool _show)
    {
        if(m_show_offset_polygons != _show)
        {
            m_show_offset_polygons = _show;
            emit changed();
        }
    }

    bool showAABB() const
    {
        return m_show_aabb;
    }

    void setShowAABB(bool _show)
    {
        if(m_show_aabb != _show)
        {
            m_show_aabb = _show;
            emit changed();
        }
    }

    bool showDistanceMap() const
    {
        return m_show_distance_map;
    }

    void setShowDistanceMap(bool _show)
    {
        if(m_show_distance_map != _show)
        {
            m_show_distance_map = _show;
            emit changed();
        }
    }

signals:
    void changed();

private:
    bool m_show_polygons;
    bool m_show_triangulation;
    bool m_show_offset_polygons;
    bool m_show_aabb;
    bool m_show_distance_map;
};
