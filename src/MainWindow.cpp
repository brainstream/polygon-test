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
