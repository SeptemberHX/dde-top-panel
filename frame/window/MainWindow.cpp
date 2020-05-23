//
// Created by septemberhx on 2020/5/23.
//

#include "MainWindow.h"
#include "controller/dockitemmanager.h"
#include "util/utils.h"

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

//    m_mainPanel->setDelegate(this);
    for (auto item : DockItemManager::instance()->itemList())
        m_mainPanel->insertItem(-1, item);

}

const QPoint rawXPosition(const QPoint &scaledPos)
{
    QScreen const *screen = Utils::screenAtByScaled(scaledPos);

    return screen ? screen->geometry().topLeft() +
                    (scaledPos - screen->geometry().topLeft()) *
                    screen->devicePixelRatio()
                  : scaledPos;
}