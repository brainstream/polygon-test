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
    mp_checkbox_show_triangulation->setChecked(_preferences.showTriangulation());
    mp_checkbox_show_offset_polygon->setChecked(_preferences.showOffsetPolygons());
    connect(mp_checkbox_show_triangulation, SIGNAL(toggled(bool)), this, SLOT(onShowTriangulationChanged()));
    connect(mp_checkbox_show_offset_polygon, SIGNAL(toggled(bool)), this, SLOT(onShowOffsetPolygonChanged()));
}

void PreferencesWidget::onShowTriangulationChanged()
{
    mr_preferences.setShowTriangulation(mp_checkbox_show_triangulation->isChecked());
}

void PreferencesWidget::onShowOffsetPolygonChanged()
{
    mr_preferences.setShowOffsetPolygons(mp_checkbox_show_offset_polygon->isChecked());
}
