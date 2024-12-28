// This is free and unencumbered software released into the public domain.
//
// Anyone is free to copy, modify, publish, use, compile, sell, or
// distribute this software, either in source code form or as a compiled
// binary, for any purpose, commercial or non-commercial, and by any means.

#include "MainWindow.h"
#include "Canvas.h"
#include <QHBoxLayout>

MainWindow::MainWindow(QWidget * _parent) :
    QMainWindow(_parent)
{
    setupUi(this);
    QHBoxLayout * layout = new QHBoxLayout(mp_central_widget);
    layout->setContentsMargins(0, 0, 0, 0);
    layout->addWidget(new Canvas(mp_central_widget));
}
