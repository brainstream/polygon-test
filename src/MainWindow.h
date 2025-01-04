// This is free and unencumbered software released into the public domain.
//
// Anyone is free to copy, modify, publish, use, compile, sell, or
// distribute this software, either in source code form or as a compiled
// binary, for any purpose, commercial or non-commercial, and by any means.

#pragma once

#include "PreferencesWidget.h"
#include "ui_MainWindow.h"
#include <QMainWindow>

class MainWindow : public QMainWindow, private Ui_MainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget * _parent = nullptr);

protected:
    void closeEvent(QCloseEvent * _event) override;

private:
    void readSettings();
    void writeSettigns();

private:
    Preferences m_preferences;
    PreferencesWidget * mp_preferences_widget;
};
