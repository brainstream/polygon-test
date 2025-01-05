// This is free and unencumbered software released into the public domain.
//
// Anyone is free to copy, modify, publish, use, compile, sell, or
// distribute this software, either in source code form or as a compiled
// binary, for any purpose, commercial or non-commercial, and by any means.

#include "PreferencesWidget.h"

PreferencesWidget::PreferencesWidget(Preferences & _preferences, QWidget * _parent) :
    QWidget(_parent),
    mr_preferences(_preferences)
{
    setupUi(this);
    mp_checkbox_show_polygons->setChecked(_preferences.showPolygons());
    mp_checkbox_show_triangulation->setChecked(_preferences.showTriangulation());
    mp_checkbox_show_offset_polygon->setChecked(_preferences.showOffsetPolygons());
    mp_checkbox_show_aabb->setChecked(_preferences.showAABB());
    mp_checkbox_show_distance_map->setChecked(_preferences.showDistanceMap());
    connect(mp_checkbox_show_polygons, &QCheckBox::toggled, &mr_preferences, &Preferences::setShowPolygons);
    connect(mp_checkbox_show_triangulation, &QCheckBox::toggled, &mr_preferences, &Preferences::setShowTriangulation);
    connect(mp_checkbox_show_offset_polygon, &QCheckBox::toggled, &mr_preferences, &Preferences::setShowOffsetPolygons);
    connect(mp_checkbox_show_aabb, &QCheckBox::toggled, &mr_preferences, &Preferences::setShowAABB);
    connect(mp_checkbox_show_distance_map, &QCheckBox::toggled, &mr_preferences, &Preferences::setShowDistanceMap);
}
