// This is free and unencumbered software released into the public domain.
//
// Anyone is free to copy, modify, publish, use, compile, sell, or
// distribute this software, either in source code form or as a compiled
// binary, for any purpose, commercial or non-commercial, and by any means.

#pragma once

#include <QWidget>
#include "Polygon.h"
#include "Preferences.h"

class Canvas : public QWidget
{
    Q_OBJECT
public:
    explicit Canvas(const Preferences & _preferences, QWidget * _parent = nullptr);

protected:
    void paintEvent(QPaintEvent * _event);
    void mouseReleaseEvent(QMouseEvent * _event);
    void mouseMoveEvent(QMouseEvent * _event);
    void keyPressEvent(QKeyEvent * _event);

private:
    void clear();

private:
    const Preferences & mr_preferences;
    Polygon m_drawing;
    PolygonList m_polygons;
    QList<Triangulation> m_triangulations;
    PolygonList m_inner_polygons;
    bool m_is_drawing;
};
