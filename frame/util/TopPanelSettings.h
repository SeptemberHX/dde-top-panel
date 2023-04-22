//
// Created by septemberhx on 2020/5/23.
//

#ifndef DDE_TOP_PANEL_TOPPANELSETTINGS_H
#define DDE_TOP_PANEL_TOPPANELSETTINGS_H

#include "constants.h"
#include <QObject>
#include <QWidget>
#include <QMenu>
#include <controller/dockitemmanager.h>
#include "dbus/dbusdisplay.h"
#include "CustomSettings.h"

using namespace Dock;
using DBusDock = com::deepin::dde::daemon::Dock;    // use dbus to get the height/width, position and hide mode of the dock

class TopPanelSettings : public QObject
{
    Q_OBJECT

public: DBusDock *m_dockInter;

public:
    explicit TopPanelSettings(DockItemManager *itemManager, QScreen *screen, QWidget *parent = 0);

    inline DisplayMode displayMode() const { return m_displayMode; }
    inline Position position() const { return m_position; }
    inline int screenRawHeight() const { return m_screenRawHeight; }
    inline int screenRawWidth() const { return m_screenRawWidth; }
    inline const QRect primaryRawRect() const { return m_primaryRawRect; }
    inline const QSize windowSize() const { return m_mainWindowSize; }

    const QRect windowRect(const Position position, const bool hide) const;

    const int dockMargin() const;

    const QRect primaryRect() const;

    void showDockSettingsMenu();
    void calculateWindowConfig();
    void applyCustomSettings(const CustomSettings& customSettings);

    QSize m_mainWindowSize;
    QRect m_frontendRect;
    QScreen *m_screen;

    void moveToScreen(QScreen *screen);

signals:
    void settingActionClicked();
    void windowRectChanged();

private slots:
    void menuActionClicked(QAction *action);

private:
    /**
     * 用于获取当 DDE-Dock 在左右两侧时的实际宽度：Fashion Mode 下会有 10px 的上下间距，Efficient Mode 下则没有。
     * 用来辅助 Top-Panel 宽度设定及左侧需要避让多少距离
     *
     * To obtain the actual width when DDE-Dock is on the left or right side:
     * there is a 10px top and bottom spacing in Fashion Mode, but not in Efficient Mode.
     * This is used to assist in setting the width of the Top-Panel and how much distance needs to be avoided on the left side.
     */
    int realDDEDockWidth();

    TopPanelSettings(TopPanelSettings const &) = delete;
    TopPanelSettings operator =(TopPanelSettings const &) = delete;

    /**
     * 用来维护 Top-Panel 所在的位置变量，每次刷新时都会讲 Panel 移动到该变量的指定点
     *
     * This is used to maintain the position variable of the Top-Panel, and every time it is refreshed,
     * the Panel will be moved to the specified point of that variable.
     */
    void resetFrontendGeometry();
    qreal dockRatio() const;

private:
    int m_dockWindowSize;
    Position m_position;

    DisplayMode m_displayMode;
    DBusDisplay *m_displayInter;
    QRect m_primaryRawRect;
    QMenu m_settingsMenu;
    QMenu *m_hideSubMenu;
    DockItemManager *m_itemManager;
    int m_screenRawHeight;
    int m_screenRawWidth;
};


#endif //DDE_TOP_PANEL_TOPPANELSETTINGS_H
