/*
 * Copyright (C) 2011 ~ 2018 Deepin Technology Co., Ltd.
 *
 * Author:     sbw <sbw@sbw.so>
 *
 * Maintainer: sbw <sbw@sbw.so>
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

#include "docksettings.h"
#include "util/utils.h"

#include <QDebug>
#include <QX11Info>
#include <QGSettings>

#include <DApplication>
#include <QScreen>
#include <QGSettings>

#define FASHION_MODE_PADDING    30
#define MAINWINDOW_MARGIN       10

#define FASHION_DEFAULT_HEIGHT 72
#define EffICIENT_DEFAULT_HEIGHT 30
#define WINDOW_MAX_SIZE          100
#define WINDOW_MIN_SIZE          30

DWIDGET_USE_NAMESPACE

extern const QPoint rawXPosition(const QPoint &scaledPos);

static QGSettings *GSettingsByMenu()
{
    static QGSettings settings("com.deepin.dde.dock.module.menu");
    return &settings;
}

static QGSettings *GSettingsByTrash()
{
    static QGSettings settings("com.deepin.dde.dock.module.trash");
    return &settings;
}

DockSettings::DockSettings(QWidget *parent)
    : QObject(parent)
    , m_dockInter(new DBusDock("com.deepin.dde.daemon.Dock", "/com/deepin/dde/daemon/Dock", QDBusConnection::sessionBus(), this))
    , m_menuVisible(true)
    , m_autoHide(true)
    , m_opacity(0.4)
    , m_fashionModeAct(tr("Fashion Mode"), this)
    , m_efficientModeAct(tr("Efficient Mode"), this)
    , m_topPosAct(tr("Top"), this)
    , m_bottomPosAct(tr("Bottom"), this)
    , m_leftPosAct(tr("Left"), this)
    , m_rightPosAct(tr("Right"), this)
    , m_keepShownAct(tr("Keep Shown"), this)
    , m_keepHiddenAct(tr("Keep Hidden"), this)
    , m_smartHideAct(tr("Smart Hide"), this)
    , m_displayInter(new DBusDisplay(this))
    , m_itemManager(DockItemManager::instance(this))
    , m_trashPluginShow(true)
{
    m_settingsMenu.setAccessibleName("dock-settingsmenu");
    checkService();

    m_primaryRawRect = m_displayInter->primaryRawRect();
    m_screenRawHeight = m_displayInter->screenRawHeight();
    m_screenRawWidth = m_displayInter->screenRawWidth();

    m_position = Dock::Position(m_dockInter->position());
    m_position = Dock::Top;

    m_displayMode = Dock::DisplayMode(m_dockInter->displayMode());
    m_displayMode = Dock::Efficient;

    m_hideMode = Dock::HideMode(m_dockInter->hideMode());
    m_hideState = Dock::HideState(m_dockInter->hideState());
    DockItem::setDockPosition(m_position);
    qApp->setProperty(PROP_POSITION, QVariant::fromValue(m_position));
    DockItem::setDockDisplayMode(m_displayMode);
    qApp->setProperty(PROP_DISPLAY_MODE, QVariant::fromValue(m_displayMode));

    m_fashionModeAct.setCheckable(true);
    m_efficientModeAct.setCheckable(true);
    m_topPosAct.setCheckable(true);
    m_bottomPosAct.setCheckable(true);
    m_leftPosAct.setCheckable(true);
    m_rightPosAct.setCheckable(true);
    m_keepShownAct.setCheckable(true);
    m_keepHiddenAct.setCheckable(true);
    m_smartHideAct.setCheckable(true);

    QMenu *modeSubMenu = new QMenu(&m_settingsMenu);
    modeSubMenu->setAccessibleName("modesubmenu");
    modeSubMenu->addAction(&m_fashionModeAct);
    modeSubMenu->addAction(&m_efficientModeAct);
    QAction *modeSubMenuAct = new QAction(tr("Mode"), this);
    modeSubMenuAct->setMenu(modeSubMenu);

    QMenu *locationSubMenu = new QMenu(&m_settingsMenu);
    locationSubMenu->setAccessibleName("locationsubmenu");
    locationSubMenu->addAction(&m_topPosAct);
    locationSubMenu->addAction(&m_bottomPosAct);
    locationSubMenu->addAction(&m_leftPosAct);
    locationSubMenu->addAction(&m_rightPosAct);
    QAction *locationSubMenuAct = new QAction(tr("Location"), this);
    locationSubMenuAct->setMenu(locationSubMenu);

    QMenu *statusSubMenu = new QMenu(&m_settingsMenu);
    statusSubMenu->setAccessibleName("statussubmenu");
    statusSubMenu->addAction(&m_keepShownAct);
    statusSubMenu->addAction(&m_keepHiddenAct);
    statusSubMenu->addAction(&m_smartHideAct);
    QAction *statusSubMenuAct = new QAction(tr("Status"), this);
    statusSubMenuAct->setMenu(statusSubMenu);

    m_hideSubMenu = new QMenu(&m_settingsMenu);
    m_hideSubMenu->setAccessibleName("pluginsmenu");
    QAction *hideSubMenuAct = new QAction(tr("Plugins"), this);
    hideSubMenuAct->setMenu(m_hideSubMenu);

    m_settingsMenu.addAction(modeSubMenuAct);
    m_settingsMenu.addAction(locationSubMenuAct);
    m_settingsMenu.addAction(statusSubMenuAct);
    m_settingsMenu.addAction(hideSubMenuAct);
    m_settingsMenu.setTitle("Settings Menu");

    connect(&m_settingsMenu, &QMenu::triggered, this, &DockSettings::menuActionClicked);
    connect(GSettingsByMenu(), &QGSettings::changed, this, &DockSettings::onGSettingsChanged);
    connect(m_dockInter, &DBusDock::PositionChanged, this, &DockSettings::onPositionChanged);
    connect(m_dockInter, &DBusDock::DisplayModeChanged, this, &DockSettings::onDisplayModeChanged);
    connect(m_dockInter, &DBusDock::HideModeChanged, this, &DockSettings::hideModeChanged, Qt::QueuedConnection);
    connect(m_dockInter, &DBusDock::HideStateChanged, this, &DockSettings::hideStateChanged);
    connect(m_dockInter, &DBusDock::ServiceRestarted, this, &DockSettings::resetFrontendGeometry);
    connect(m_dockInter, &DBusDock::OpacityChanged, this, &DockSettings::onOpacityChanged);
    connect(m_dockInter, &DBusDock::WindowSizeEfficientChanged, this, &DockSettings::onWindowSizeChanged);
    connect(m_dockInter, &DBusDock::WindowSizeFashionChanged, this, &DockSettings::onWindowSizeChanged);

    connect(m_itemManager, &DockItemManager::itemInserted, this, &DockSettings::dockItemCountChanged, Qt::QueuedConnection);
    connect(m_itemManager, &DockItemManager::itemRemoved, this, &DockSettings::dockItemCountChanged, Qt::QueuedConnection);
    connect(m_itemManager, &DockItemManager::trayVisableCountChanged, this, &DockSettings::trayVisableCountChanged, Qt::QueuedConnection);

    connect(m_displayInter, &DBusDisplay::PrimaryRectChanged, this, &DockSettings::primaryScreenChanged, Qt::QueuedConnection);
    connect(m_displayInter, &DBusDisplay::ScreenHeightChanged, this, &DockSettings::primaryScreenChanged, Qt::QueuedConnection);
    connect(m_displayInter, &DBusDisplay::ScreenWidthChanged, this, &DockSettings::primaryScreenChanged, Qt::QueuedConnection);
    connect(m_displayInter, &DBusDisplay::PrimaryChanged, this, &DockSettings::primaryScreenChanged, Qt::QueuedConnection);
    connect(GSettingsByTrash(), &QGSettings::changed, this, &DockSettings::onTrashGSettingsChanged);
    QTimer::singleShot(0, this, [=] {onGSettingsChanged("enable");});

    DApplication *app = qobject_cast<DApplication *>(qApp);
    if (app) {
        connect(app, &DApplication::iconThemeChanged, this, &DockSettings::gtkIconThemeChanged);
    }

    calculateWindowConfig();
    updateForbidPostions();
    resetFrontendGeometry();

    QTimer::singleShot(0, this, [ = ] {onOpacityChanged(m_dockInter->opacity());});
    QTimer::singleShot(0, this, [=] {
        onGSettingsChanged("enable");
    });
}

DockSettings &DockSettings::Instance()
{
    static DockSettings settings;
    return settings;
}

const QRect DockSettings::primaryRect() const
{
    QRect rect = m_primaryRawRect;
    qreal scale = qApp->primaryScreen()->devicePixelRatio();

    rect.setWidth(std::round(qreal(rect.width()) / scale));
    rect.setHeight(std::round(qreal(rect.height()) / scale));

    return rect;
}

const int DockSettings::dockMargin() const
{
    if (m_displayMode == Dock::Efficient)
        return 0;

    return 10;
}

const QSize DockSettings::panelSize() const
{
    return m_mainWindowSize;
}

const QRect DockSettings::windowRect(const Position position, const bool hide) const
{
    QSize size = m_mainWindowSize;
    if (hide) {
        switch (position) {
        case Top:
        case Bottom:    size.setHeight(2);      break;
        case Left:
        case Right:     size.setWidth(2);       break;
        }
    }

    const QRect primaryRect = this->primaryRect();
    const int offsetX = (primaryRect.width() - size.width()) / 2;
    const int offsetY = (primaryRect.height() - size.height()) / 2;
    int margin = hide ?  0 : this->dockMargin();
    QPoint p(0, 0);
    switch (position) {
    case Top:
        p = QPoint(offsetX, margin);
        break;
    case Left:
        p = QPoint(margin, offsetY);
        break;
    case Right:
        p = QPoint(primaryRect.width() - size.width() - margin, offsetY);
        break;
    case Bottom:
        p = QPoint(offsetX, primaryRect.height() - size.height() - margin);
        break;
    default: Q_UNREACHABLE();
    }

    return QRect(primaryRect.topLeft() + p, size);
}

void DockSettings::showDockSettingsMenu()
{
    QGSettings gsettings("com.deepin.dde.dock.module.menu");
    if (gsettings.keys().contains("enable") && !gsettings.get("enable").toBool()) {
        return;
    }

    m_autoHide = false;

    // create actions
    QList<QAction *> actions;
    for (auto *p : m_itemManager->pluginList()) {
        if (!p->pluginIsAllowDisable())
            continue;

        const bool enable = !p->pluginIsDisable();
        const QString &name = p->pluginName();
        const QString &display = p->pluginDisplayName();

        if (!m_trashPluginShow && name == "trash") {
            continue;
        }

        QAction *act = new QAction(display, this);
        act->setCheckable(true);
        act->setChecked(enable);
        act->setData(name);

        actions << act;
    }

    // sort by name
    std::sort(actions.begin(), actions.end(), [](QAction * a, QAction * b) -> bool {
        return a->data().toString() > b->data().toString();
    });

    // add actions
    qDeleteAll(m_hideSubMenu->actions());
    for (auto act : actions)
        m_hideSubMenu->addAction(act);

    m_fashionModeAct.setChecked(m_displayMode == Fashion);
    m_efficientModeAct.setChecked(m_displayMode == Efficient);
    m_topPosAct.setChecked(m_position == Top);
    m_topPosAct.setEnabled(!m_forbidPositions.contains(Top));
    m_bottomPosAct.setChecked(m_position == Bottom);
    m_bottomPosAct.setEnabled(!m_forbidPositions.contains(Bottom));
    m_leftPosAct.setChecked(m_position == Left);
    m_leftPosAct.setEnabled(!m_forbidPositions.contains(Left));
    m_rightPosAct.setChecked(m_position == Right);
    m_rightPosAct.setEnabled(!m_forbidPositions.contains(Right));
    m_keepShownAct.setChecked(m_hideMode == KeepShowing);
    m_keepHiddenAct.setChecked(m_hideMode == KeepHidden);
    m_smartHideAct.setChecked(m_hideMode == SmartHide);

    m_settingsMenu.exec(QCursor::pos());

    setAutoHide(true);
}

void DockSettings::updateGeometry()
{

}

void DockSettings::setAutoHide(const bool autoHide)
{
    if (m_autoHide == autoHide)
        return;

    m_autoHide = autoHide;
    emit autoHideChanged(m_autoHide);
}

void DockSettings::menuActionClicked(QAction *action)
{
    Q_ASSERT(action);

    if (action == &m_fashionModeAct)
        return m_dockInter->setDisplayMode(Fashion);
    if (action == &m_efficientModeAct)
        return m_dockInter->setDisplayMode(Efficient);

    if (action == &m_topPosAct)
        return m_dockInter->setPosition(Top);
    if (action == &m_bottomPosAct)
        return m_dockInter->setPosition(Bottom);
    if (action == &m_leftPosAct)
        return m_dockInter->setPosition(Left);
    if (action == &m_rightPosAct)
        return m_dockInter->setPosition(Right);
    if (action == &m_keepShownAct)
        return m_dockInter->setHideMode(KeepShowing);
    if (action == &m_keepHiddenAct)
        return m_dockInter->setHideMode(KeepHidden);
    if (action == &m_smartHideAct)
        return m_dockInter->setHideMode(SmartHide);

    // check plugin hide menu.
    const QString &data = action->data().toString();
    if (data.isEmpty())
        return;
    for (auto *p : m_itemManager->pluginList()) {
        if (p->pluginName() == data)
            return p->pluginStateSwitched();
    }
}

void DockSettings::onGSettingsChanged(const QString &key)
{
    if (key != "enable") {
        return;
    }

    QGSettings *setting = GSettingsByMenu();

    if (setting->keys().contains("enable")) {
        const bool isEnable = GSettingsByMenu()->keys().contains("enable") && GSettingsByMenu()->get("enable").toBool();
        m_menuVisible=isEnable && setting->get("enable").toBool();
    }
}

void DockSettings::onPositionChanged()
{
    const Position prevPos = m_position;
    const Position nextPos = Dock::Position(m_dockInter->position());

    if (prevPos == nextPos)
        return;

    emit positionChanged(prevPos, nextPos);

    QTimer::singleShot(200, this, [this, nextPos] {
        m_position = nextPos;
        DockItem::setDockPosition(nextPos);
        qApp->setProperty(PROP_POSITION, QVariant::fromValue(nextPos));

        calculateWindowConfig();

        m_itemManager->refershItemsIcon();
    });
}

void DockSettings::onDisplayModeChanged()
{
//    qDebug() << Q_FUNC_INFO;
    m_displayMode = Dock::DisplayMode(m_dockInter->displayMode());
    DockItem::setDockDisplayMode(m_displayMode);
    qApp->setProperty(PROP_DISPLAY_MODE, QVariant::fromValue(m_displayMode));

    emit displayModeChanegd();
    calculateWindowConfig();

   //QTimer::singleShot(1, m_itemManager, &DockItemManager::sortPluginItems);
}

void DockSettings::hideModeChanged()
{
//    qDebug() << Q_FUNC_INFO;
    m_hideMode = Dock::HideMode(m_dockInter->hideMode());

    emit windowHideModeChanged();
}

void DockSettings::hideStateChanged()
{
//    qDebug() << Q_FUNC_INFO;
    const Dock::HideState state = Dock::HideState(m_dockInter->hideState());

    if (state == Dock::Unknown)
        return;

    m_hideState = state;

    emit windowVisibleChanged();
}

void DockSettings::dockItemCountChanged()
{
    emit windowGeometryChanged();
}

void DockSettings::primaryScreenChanged()
{
//    qDebug() << Q_FUNC_INFO;
    m_primaryRawRect = m_displayInter->primaryRawRect();
    m_screenRawHeight = m_displayInter->screenRawHeight();
    m_screenRawWidth = m_displayInter->screenRawWidth();

    updateForbidPostions();
    emit dataChanged();
    calculateWindowConfig();

    // 主屏切换时，如果缩放比例不一样，需要刷新item的图标(bug:3176)
    m_itemManager->refershItemsIcon();
}

void DockSettings::resetFrontendGeometry()
{
    const QRect r = windowRect(m_position);
    const qreal ratio = dockRatio();
    const QPoint p = rawXPosition(r.topLeft());
    const uint w = r.width() * ratio;
    const uint h = r.height() * ratio;

    m_frontendRect = QRect(p.x(), p.y(), w, h);
    m_dockInter->SetFrontendWindowRect(p.x(), p.y(), w, h);
}

void DockSettings::updateFrontendGeometry()
{
    resetFrontendGeometry();
}

bool DockSettings::test(const Position pos, const QList<QRect> &otherScreens) const
{
    QRect maxStrut(0, 0, m_screenRawWidth, m_screenRawHeight);
    switch (pos) {
    case Top:
        maxStrut.setBottom(m_primaryRawRect.top() - 1);
        maxStrut.setLeft(m_primaryRawRect.left());
        maxStrut.setRight(m_primaryRawRect.right());
        break;
    case Bottom:
        maxStrut.setTop(m_primaryRawRect.bottom() + 1);
        maxStrut.setLeft(m_primaryRawRect.left());
        maxStrut.setRight(m_primaryRawRect.right());
        break;
    case Left:
        maxStrut.setRight(m_primaryRawRect.left() - 1);
        maxStrut.setTop(m_primaryRawRect.top());
        maxStrut.setBottom(m_primaryRawRect.bottom());
        break;
    case Right:
        maxStrut.setLeft(m_primaryRawRect.right() + 1);
        maxStrut.setTop(m_primaryRawRect.top());
        maxStrut.setBottom(m_primaryRawRect.bottom());
        break;
    default:;
    }

    if (maxStrut.width() == 0 || maxStrut.height() == 0)
        return true;

    for (const auto &r : otherScreens)
        if (maxStrut.intersects(r))
            return false;

    return true;
}

void DockSettings::updateForbidPostions()
{
    qDebug() << Q_FUNC_INFO;

    const auto &screens = qApp->screens();
    if (screens.size() < 2)
        return m_forbidPositions.clear();

    QSet<Position> forbids;
    QList<QRect> rawScreenRects;
    for (auto *s : screens) {
        qInfo() << s->name() << s->geometry();

        if (s == qApp->primaryScreen())
            continue;

        const QRect &g = s->geometry();
        rawScreenRects << QRect(g.topLeft(), g.size() * s->devicePixelRatio());
    }

    qInfo() << rawScreenRects << m_screenRawWidth << m_screenRawHeight;

    if (!test(Top, rawScreenRects))
        forbids << Top;
    if (!test(Bottom, rawScreenRects))
        forbids << Bottom;
    if (!test(Left, rawScreenRects))
        forbids << Left;
    if (!test(Right, rawScreenRects))
        forbids << Right;

    m_forbidPositions = std::move(forbids);
}

void DockSettings::onOpacityChanged(const double value)
{
    if (m_opacity == value) return;

    m_opacity = value;

    emit opacityChanged(value * 255);
}

void DockSettings::trayVisableCountChanged(const int &count)
{
    emit trayCountChanged();
}

void DockSettings::calculateWindowConfig()
{
    if (m_displayMode == Dock::Efficient) {
        m_dockWindowSize = m_dockInter->windowSizeEfficient();
        if (m_dockWindowSize > WINDOW_MAX_SIZE || m_dockWindowSize < WINDOW_MIN_SIZE) {
            m_dockWindowSize = EffICIENT_DEFAULT_HEIGHT;
            m_dockInter->setWindowSize(EffICIENT_DEFAULT_HEIGHT);
        }

        switch (m_position) {
        case Top:
        case Bottom:
            m_mainWindowSize.setHeight(m_dockWindowSize);
            m_mainWindowSize.setWidth(primaryRect().width());
            break;

        case Left:
        case Right:
            m_mainWindowSize.setHeight(primaryRect().height());
            m_mainWindowSize.setWidth(m_dockWindowSize);
            break;

        default:
            Q_ASSERT(false);
        }
    } else if (m_displayMode == Dock::Fashion) {
        m_dockWindowSize = m_dockInter->windowSizeFashion();
        if (m_dockWindowSize > WINDOW_MAX_SIZE || m_dockWindowSize < WINDOW_MIN_SIZE) {
            m_dockWindowSize = FASHION_DEFAULT_HEIGHT;
            m_dockInter->setWindowSize(FASHION_DEFAULT_HEIGHT);
        }

        switch (m_position) {
        case Top:
        case Bottom: {
            m_mainWindowSize.setHeight(m_dockWindowSize);
            m_mainWindowSize.setWidth(this->primaryRect().width() - MAINWINDOW_MARGIN * 2);
            break;
        }
        case Left:
        case Right: {
            m_mainWindowSize.setHeight(this->primaryRect().height() - MAINWINDOW_MARGIN * 2);
            m_mainWindowSize.setWidth(m_dockWindowSize);
            break;
        }
        default:
            Q_ASSERT(false);
        }
    } else {
        Q_ASSERT(false);
    }

    resetFrontendGeometry();
}

void DockSettings::gtkIconThemeChanged()
{
    qDebug() << Q_FUNC_INFO;
    m_itemManager->refershItemsIcon();
}

qreal DockSettings::dockRatio() const
{
    QScreen const *screen = Utils::screenAtByScaled(m_frontendRect.center());

    return screen ? screen->devicePixelRatio() : qApp->devicePixelRatio();
}

void DockSettings::onWindowSizeChanged()
{
    calculateWindowConfig();
    emit windowGeometryChanged();
}

void DockSettings::checkService()
{
    // com.deepin.dde.daemon.Dock服务比dock晚启动，导致dock启动后的状态错误

    QString serverName = "com.deepin.dde.daemon.Dock";
    QDBusConnectionInterface *ifc = QDBusConnection::sessionBus().interface();

    if (!ifc->isServiceRegistered(serverName)) {
        connect(ifc, &QDBusConnectionInterface::serviceOwnerChanged, this, [ = ](const QString & name, const QString & oldOwner, const QString & newOwner) {
            if (name == serverName && !newOwner.isEmpty()) {

                m_dockInter = new DBusDock(serverName, "/com/deepin/dde/daemon/Dock", QDBusConnection::sessionBus(), this);
                onPositionChanged();
                onDisplayModeChanged();
                hideModeChanged();
                hideStateChanged();
                onOpacityChanged(m_dockInter->opacity());
                onWindowSizeChanged();

                disconnect(ifc);
            }
        });
    }
}

void DockSettings::onTrashGSettingsChanged(const QString &key)
{
    if (key != "enable") {
        return ;
    }

    QGSettings *setting = GSettingsByTrash();

    if (setting->keys().contains("enable")) {
         m_trashPluginShow = GSettingsByTrash()->keys().contains("enable") && GSettingsByTrash()->get("enable").toBool();
    }
}
