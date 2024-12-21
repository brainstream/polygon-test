#pragma once

#include "Polygon.h"
#include <QMainWindow>
#include "ui_MainWindow.h"

class MainWindow : public QMainWindow, private Ui_MainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget * _parent = nullptr);

private:
    PolygonMesh * mp_polygon;
};
