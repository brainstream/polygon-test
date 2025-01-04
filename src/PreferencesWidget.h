// This is free and unencumbered software released into the public domain.
//
// Anyone is free to copy, modify, publish, use, compile, sell, or
// distribute this software, either in source code form or as a compiled
// binary, for any purpose, commercial or non-commercial, and by any means.

#pragma once

#include "Preferences.h"
#include "ui_PreferencesWidget.h"
#include <QWidget>

namespace Ui {
class PreferencesWidget;
}

class PreferencesWidget : public QWidget, private Ui::PreferencesWidget
{
    Q_OBJECT

public:
    explicit PreferencesWidget(Preferences & _preferences, QWidget * _parent = nullptr);

private:
    Preferences & mr_preferences;
};
