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

#ifndef DOCKSETTINGS_H
#define DOCKSETTINGS_H

#include "constants.h"
#include "dbus/dbusmenumanager.h"
#include "dbus/dbusdisplay.h"
#include "controller/dockitemmanager.h"

#include <com_deepin_dde_daemon_dock.h>

#include <QAction>
#include <QMenu>

#include <QObject>
#include <QSize>

#include <QStyleFactory>

DWIDGET_USE_NAMESPACE

using namespace Dock;
using DBusDock = com::deepin::dde::daemon::Dock;

class DockSettings : public QObject
{
    Q_OBJECT

public:
    static DockSettings &Instance();

    inline DisplayMode displayMode() const { return m_displayMode; }
    inline HideMode hideMode() const { return m_hideMode; }
    inline HideState hideState() const { return m_hideState; }
    inline Position position() const { return m_position; }
    inline int screenRawHeight() const { return m_screenRawHeight; }
    inline int screenRawWidth() const { return m_screenRawWidth; }
    inline int expandTimeout() const { return m_dockInter->showTimeout(); }
    inline int narrowTimeout() const { return 100; }
    inline bool autoHide() const { return m_autoHide; }
    const QRect primaryRect() const;
    inline const QRect primaryRawRect() const { return m_primaryRawRect; }
    inline const QRect frontendWindowRect() const { return m_frontendRect; }
    inline const QSize windowSize() const { return m_mainWindowSize; }
    inline const quint8 Opacity() const { return m_opacity * 255; }
    const int dockMargin() const;

    const QSize panelSize() const;
    const QRect windowRect(const Position position, const bool hide = false) const;
    qreal dockRatio() const;

    void showDockSettingsMenu();
    void updateFrontendGeometry();
    
    QSize m_mainWindowSize;
    DBusDock *m_dockInter;
    bool m_menuVisible;

signals:
    void dataChanged() const;
    void positionChanged(const Position prevPosition, const Position nextPosition) const;
    void autoHideChanged(const bool autoHide) const;
    void displayModeChanegd() const;
    void windowVisibleChanged() const;
    void windowHideModeChanged() const;
    void windowGeometryChanged() const;
    void opacityChanged(const quint8 value) const;
    void trayCountChanged() const;

public slots:
    void updateGeometry();
    void setAutoHide(const bool autoHide);

private slots:
    void menuActionClicked(QAction *action);
    void onGSettingsChanged(const QString &key);
    void onPositionChanged();
    void onDisplayModeChanged();
    void hideModeChanged();
    void hideStateChanged();
    void dockItemCountChanged();
    void primaryScreenChanged();
    void resetFrontendGeometry();
    void updateForbidPostions();
    void onOpacityChanged(const double value);
    void trayVisableCountChanged(const int &count);
    void onWindowSizeChanged();
    void onTrashGSettingsChanged(const QString &key);

private:
    DockSettings(QWidget *parent = 0);
    DockSettings(DockSettings const &) = delete;
    DockSettings operator =(DockSettings const &) = delete;

    bool test(const Position pos, const QList<QRect> &otherScreens) const;
    void calculateWindowConfig();
    void gtkIconThemeChanged();
    void checkService();

private:
    int m_dockWindowSize;
    bool m_autoHide;
    int m_screenRawHeight;
    int m_screenRawWidth;
    double m_opacity;
    int m_dockMargin;
    QSet<Position> m_forbidPositions;
    Position m_position;
    HideMode m_hideMode;
    HideState m_hideState;
    DisplayMode m_displayMode;
    QRect m_primaryRawRect;
    QRect m_frontendRect;

    QMenu m_settingsMenu;
    QMenu *m_hideSubMenu;
    QAction m_fashionModeAct;
    QAction m_efficientModeAct;
    QAction m_topPosAct;
    QAction m_bottomPosAct;
    QAction m_leftPosAct;
    QAction m_rightPosAct;
    QAction m_keepShownAct;
    QAction m_keepHiddenAct;
    QAction m_smartHideAct;

    DBusDisplay *m_displayInter;
    DockItemManager *m_itemManager;
    bool m_trashPluginShow;
};

#endif // DOCKSETTINGS_H
