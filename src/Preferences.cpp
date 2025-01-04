// This is free and unencumbered software released into the public domain.
//
// Anyone is free to copy, modify, publish, use, compile, sell, or
// distribute this software, either in source code form or as a compiled
// binary, for any purpose, commercial or non-commercial, and by any means.

#include "Preferences.h"

namespace {

const char gc_settings_group[] = "preferences";
const char gc_settings_key_show_triangulation[] = "show-triangulation";
const char gc_settings_key_show_offset_polygons[] = "show-offset-polygons";

} // namespace

void Preferences::save(QSettings & _settings) const
{
    _settings.beginGroup(gc_settings_group);
    _settings.setValue(gc_settings_key_show_triangulation, m_show_triangulation);
    _settings.setValue(gc_settings_key_show_offset_polygons, m_show_offset_polygons);
    _settings.endGroup();
}

void Preferences::load(QSettings & _settings)
{
    _settings.beginGroup(gc_settings_group);
    {
        QVariant value = _settings.value(gc_settings_key_show_triangulation);
        if(!value.isNull())
            m_show_triangulation = value.toBool();
    }
    {
        QVariant value = _settings.value(gc_settings_key_show_offset_polygons);
        if(!value.isNull())
            m_show_offset_polygons = value.toBool();
    }
    _settings.endGroup();
}
