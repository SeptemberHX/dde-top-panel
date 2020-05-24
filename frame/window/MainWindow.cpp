//
// Created by septemberhx on 2020/5/23.
//

#include <xcb/xcb_misc.h>
#include "MainWindow.h"
#include "controller/dockitemmanager.h"
#include "util/utils.h"

MainWindow::MainWindow(QWidget *parent)
    : DBlurEffectWidget(parent)
    , m_mainPanel(new MainPanelControl(this))
    , m_xcbMisc(XcbMisc::instance())
    , m_platformWindowHandle(this, this)
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

    m_settings = &TopPanelSettings::Instance();
    m_xcbMisc->set_window_type(winId(), XcbMisc::Dock);
//    m_size = m_settings->m_mainWindowSize;
    m_mainPanel->setDisplayMode(m_settings->displayMode());
    m_mainPanel->move(0, 0);

    this->resizeMainPanelWindow();
    this->initConnections();

//    m_mainPanel->setDelegate(this);
    for (auto item : DockItemManager::instance()->itemList())
        m_mainPanel->insertItem(-1, item);

    m_curDockPos = m_settings->position();
    setStrutPartial();
}

void MainWindow::resizeMainPanelWindow()
{
    m_mainPanel->setFixedSize(m_settings->m_mainWindowSize);

//    switch (m_curDockPos) {
//        case Dock::Top:
//            m_dragWidget->setGeometry(0, height() - DRAG_AREA_SIZE, width(), DRAG_AREA_SIZE);
//            break;
//        case Dock::Bottom:
//            m_dragWidget->setGeometry(0, 0, width(), DRAG_AREA_SIZE);
//            break;
//        case Dock::Left:
//            m_dragWidget->setGeometry(width() - DRAG_AREA_SIZE, 0, DRAG_AREA_SIZE, height());
//            break;
//        case Dock::Right:
//            m_dragWidget->setGeometry(0, 0, DRAG_AREA_SIZE, height());
//            break;
//        default: break;
//    }
}

MainWindow::~MainWindow() {
    delete m_xcbMisc;
}

const QPoint rawXPosition(const QPoint &scaledPos)
{
    QScreen const *screen = Utils::screenAtByScaled(scaledPos);

    return screen ? screen->geometry().topLeft() +
                    (scaledPos - screen->geometry().topLeft()) *
                    screen->devicePixelRatio()
                  : scaledPos;
}

void MainWindow::clearStrutPartial()
{
    m_xcbMisc->clear_strut_partial(winId());
}

void MainWindow::setStrutPartial()
{
    // first, clear old strut partial
    clearStrutPartial();

    // reset env
    //resetPanelEnvironment(true);

    const auto ratio = devicePixelRatioF();
    const int maxScreenHeight = m_settings->screenRawHeight();
    const int maxScreenWidth = m_settings->screenRawWidth();
    const Position side = m_curDockPos;
    const QPoint &p = rawXPosition(m_settings->windowRect(m_curDockPos, false).topLeft());
    const QSize &s = m_settings->windowSize();
    const QRect &primaryRawRect = m_settings->primaryRawRect();

    XcbMisc::Orientation orientation = XcbMisc::OrientationTop;
    uint strut = 0;
    uint strutStart = 0;
    uint strutEnd = 0;

    QRect strutArea(0, 0, maxScreenWidth, maxScreenHeight);
    switch (side) {
        case Position::Top:
            orientation = XcbMisc::OrientationTop;
            strut = p.y() + s.height() * ratio;
            strutStart = p.x();
            strutEnd = qMin(qRound(p.x() + s.width() * ratio), primaryRawRect.right());
            strutArea.setLeft(strutStart);
            strutArea.setRight(strutEnd);
            strutArea.setBottom(strut);
            break;
        case Position::Bottom:
            orientation = XcbMisc::OrientationBottom;
            strut = maxScreenHeight - p.y();
            strutStart = p.x();
            strutEnd = qMin(qRound(p.x() + s.width() * ratio), primaryRawRect.right());
            strutArea.setLeft(strutStart);
            strutArea.setRight(strutEnd);
            strutArea.setTop(p.y());
            break;
        case Position::Left:
            orientation = XcbMisc::OrientationLeft;
            strut = p.x() + s.width() * ratio;
            strutStart = p.y();
            strutEnd = qMin(qRound(p.y() + s.height() * ratio), primaryRawRect.bottom());
            strutArea.setTop(strutStart);
            strutArea.setBottom(strutEnd);
            strutArea.setRight(strut);
            break;
        case Position::Right:
            orientation = XcbMisc::OrientationRight;
            strut = maxScreenWidth - p.x();
            strutStart = p.y();
            strutEnd = qMin(qRound(p.y() + s.height() * ratio), primaryRawRect.bottom());
            strutArea.setTop(strutStart);
            strutArea.setBottom(strutEnd);
            strutArea.setLeft(p.x());
            break;
        default:
            Q_ASSERT(false);
    }

    // pass if strut area is intersect with other screen
    int count = 0;
    const QRect pr = m_settings->primaryRect();
    for (auto *screen : qApp->screens()) {
        const QRect sr = screen->geometry();
        if (sr == pr)
            continue;

        if (sr.intersects(strutArea))
            ++count;
    }
    if (count > 0) {
        qWarning() << "strutArea is intersects with another screen.";
        qWarning() << maxScreenHeight << maxScreenWidth << side << p << s;
        return;
    }

    m_xcbMisc->set_strut_partial(winId(), orientation, strut + m_settings->dockMargin() * ratio, strutStart, strutEnd);
}

void MainWindow::initConnections() {
//    connect(m_settings, &DockSettings::windowHideModeChanged, this, &MainWindow::setStrutPartial, Qt::QueuedConnection);
    connect(DockItemManager::instance(), &DockItemManager::itemInserted, m_mainPanel, &MainPanelControl::insertItem, Qt::DirectConnection);
    connect(DockItemManager::instance(), &DockItemManager::itemUpdated, m_mainPanel, &MainPanelControl::itemUpdated, Qt::DirectConnection);
    connect(DockItemManager::instance(), &DockItemManager::itemRemoved, m_mainPanel, &MainPanelControl::removeItem, Qt::DirectConnection);
}

void MainWindow::mousePressEvent(QMouseEvent *e)
{
    e->ignore();
    if (e->button() == Qt::RightButton) {
        m_settings->showDockSettingsMenu();
        return;
    }
}