#include "mainwindow.h"


MainWindow::~MainWindow()
{
    m_mainLayout->removeWidget(static_cast<QChartView*>(m_view));
}

