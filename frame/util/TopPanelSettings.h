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

using namespace Dock;
using DBusDock = com::deepin::dde::daemon::Dock;    // use dbus to get the height/width, position and hide mode of the dock

class TopPanelSettings : public QObject
{
    Q_OBJECT

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

    QSize m_mainWindowSize;
    QRect m_frontendRect;
    QScreen *m_screen;

    void moveToScreen(QScreen *screen);

signals:
    void settingActionClicked();

private slots:
    void menuActionClicked(QAction *action);

private:
    TopPanelSettings(TopPanelSettings const &) = delete;
    TopPanelSettings operator =(TopPanelSettings const &) = delete;

    void resetFrontendGeometry();
    qreal dockRatio() const;

private:
    DBusDock *m_dockInter;
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
