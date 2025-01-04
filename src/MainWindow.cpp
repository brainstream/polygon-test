// This is free and unencumbered software released into the public domain.
//
// Anyone is free to copy, modify, publish, use, compile, sell, or
// distribute this software, either in source code form or as a compiled
// binary, for any purpose, commercial or non-commercial, and by any means.

#include "MainWindow.h"
#include "Canvas.h"
#include <QHBoxLayout>

namespace {

const char gc_settings_group[] = "main-window";
const char gc_settings_key_geometry[] = "geometry";

} // namespace

MainWindow::MainWindow(QWidget * _parent) :
    QMainWindow(_parent),
    m_preferences()
{
    setupUi(this);
    readSettings();
    QHBoxLayout * layout = new QHBoxLayout(mp_central_widget);
    layout->setContentsMargins(0, 0, 0, 0);
    layout->addWidget(new Canvas(m_preferences, mp_central_widget));
    mp_preferences_widget = new PreferencesWidget(m_preferences, mp_central_widget);
    mp_preferences_widget->setSizePolicy(QSizePolicy::Maximum, QSizePolicy::Expanding);
    layout->addWidget(mp_preferences_widget);

}

void MainWindow::closeEvent(QCloseEvent * _event)
{
    writeSettigns();
    QMainWindow::closeEvent(_event);
}

void MainWindow::readSettings()
{
    QSettings settings;
    settings.beginGroup(gc_settings_group);
    restoreGeometry(settings.value(gc_settings_key_geometry).toByteArray());
    settings.endGroup();
    m_preferences.load(settings);
}

void MainWindow::writeSettigns()
{
    QSettings settings;
    settings.beginGroup(gc_settings_group);
    settings.setValue(gc_settings_key_geometry, saveGeometry());
    settings.endGroup();
    m_preferences.save(settings);
}
