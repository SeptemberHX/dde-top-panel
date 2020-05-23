/*
 * Copyright (C) 2011 ~ 2018 Deepin Technology Co., Ltd.
 *
 * Author:     listenerri <listenerri@gmail.com>
 *
 * Maintainer: listenerri <listenerri@gmail.com>
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

#ifndef SYSTEMTRAYITEM_H
#define SYSTEMTRAYITEM_H

#include "constants.h"
#include "../abstracttraywidget.h"
#include "util/dockpopupwindow.h"
#include "pluginsiteminterface.h"

#include <QGestureEvent>
#include <QMenu>

class QGSettings;
class SystemTrayItem : public AbstractTrayWidget
{
    Q_OBJECT

public:
    SystemTrayItem(PluginsItemInterface* const pluginInter, const QString &itemKey, QWidget *parent = nullptr);
    virtual ~SystemTrayItem();

public:
    QString itemKeyForConfig() override;
    void setActive(const bool active) Q_DECL_OVERRIDE;
    void updateIcon() Q_DECL_OVERRIDE;
    const QImage trayImage() Q_DECL_OVERRIDE;
    void sendClick(uint8_t mouseButton, int x, int y) Q_DECL_OVERRIDE;
    inline TrayType trayTyep() const Q_DECL_OVERRIDE { return TrayType::SystemTray; }

    QWidget *trayTipsWidget();
    QWidget *trayPopupApplet();
    const QString trayClickCommand();
    const QString contextMenu() const;
    void invokedMenuItem(const QString &menuId, const bool checked);

    static void setDockPostion(const Dock::Position pos) { DockPosition = pos; }

    QWidget *centralWidget() const;
    void detachPluginWidget();
    void showContextMenu();

    void showPopupApplet(QWidget * const applet);
    void hidePopup();

signals:
    void itemVisibleChanged(bool visible);

protected:
    bool event(QEvent *event) Q_DECL_OVERRIDE;
    void enterEvent(QEvent *event) Q_DECL_OVERRIDE;
    void leaveEvent(QEvent *event) Q_DECL_OVERRIDE;
    void mousePressEvent(QMouseEvent *event) Q_DECL_OVERRIDE;
    void mouseReleaseEvent(QMouseEvent *event) Q_DECL_OVERRIDE;
    void showEvent(QShowEvent* event) override;

protected:
    const QPoint popupMarkPoint() const;
    const QPoint topleftPoint() const;

    void hideNonModel();
    void popupWindowAccept();

    virtual void showPopupWindow(QWidget * const content, const bool model = false);
    virtual void showHoverTips();

    bool checkAndResetTapHoldGestureState();
    virtual void gestureEvent(QGestureEvent *event);

protected Q_SLOTS:
    void onContextMenuAccepted();

private:
    void updatePopupPosition();
    void onGSettingsChanged(const QString &key);
    bool checkGSettingsControl() const;
    void menuActionClicked(QAction *action);

private:
    bool m_popupShown;
    bool m_tapAndHold;
    QMenu m_contextMenu;

    PluginsItemInterface* m_pluginInter;
    QWidget *m_centralWidget;

    QTimer *m_popupTipsDelayTimer;
    QTimer *m_popupAdjustDelayTimer;

    QPointer<QWidget> m_lastPopupWidget;
    QString m_itemKey;

    static Dock::Position DockPosition;
    static QPointer<DockPopupWindow> PopupWindow;
    QGSettings* m_gsettings;
};

#endif // SYSTEMTRAYITEM_H
