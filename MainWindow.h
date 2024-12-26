#pragma once

#include <QMainWindow>
#include "ui_MainWindow.h"

class MainWindow : public QMainWindow, private Ui_MainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget * _parent = nullptr);
};
