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
    ~Canvas() override;

protected:
    void paintEvent(QPaintEvent * _event) override;
    void mouseReleaseEvent(QMouseEvent * _event) override;
    void mouseMoveEvent(QMouseEvent * _event) override;
    void keyPressEvent(QKeyEvent * _event) override;

private:
    void setWaitingState(bool _waiting);
    void drawSdfImage(const AABB & _aabb);
    void clear();

signals:
    void sdfImageReady(QImage * _pixmap, QPrivateSignal);

private slots:
    void onSdfImageReady(QImage * _pixmap);

private:
    const Preferences & mr_preferences;
    Polygon m_drawing;
    PolygonList m_polygons;
    QList<Triangulation> m_triangulations;
    PolygonList m_inner_polygons;
    QImage * mp_sdfs_image;
    bool m_is_updating;
    bool m_is_drawing;
};
