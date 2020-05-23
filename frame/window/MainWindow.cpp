//
// Created by septemberhx on 2020/5/23.
//

#include "MainWindow.h"

MainWindow::MainWindow(QWidget *parent)
    : DBlurEffectWidget(parent)
    , m_platformWindowHandle(this)
    , m_mainPanel(new MainPanelControl(this))
{
    setAccessibleName("dock-top-panel-mainwindow");
    m_mainPanel->setAccessibleName("dock-top-panel-mainpanel");
    setAttribute(Qt::WA_TranslucentBackground);
    setMouseTracking(true);
    setAcceptDrops(true);

    DPlatformWindowHandle::enableDXcbForWindow(this, true);
    m_platformWindowHandle.setEnableBlurWindow(true);
    m_platformWindowHandle.setTranslucentBackground(true);
    m_platformWindowHandle.setWindowRadius(0);
    m_platformWindowHandle.setShadowOffset(QPoint(0, 5));
    m_platformWindowHandle.setShadowColor(QColor(0, 0, 0, 0.3 * 255));

    // remove radius
    m_platformWindowHandle.setWindowRadius(0);
}
