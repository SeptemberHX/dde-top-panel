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

#ifndef DOCKITEM_H
#define DOCKITEM_H

#include "constants.h"
#include "util/dockpopupwindow.h"
#include "components/hoverhighlighteffect.h"

#include <QFrame>
#include <QPointer>
#include <QGestureEvent>
#include <QMenu>

#include <memory>

using namespace Dock;

class DockItem : public QWidget
{
    Q_OBJECT

public:
    enum ItemType {
        Launcher,
        App,
        Plugins,
        FixedPlugin,
        Placeholder,
        TrayPlugin,
    };

public:
    explicit DockItem(QWidget *parent = nullptr);
    ~DockItem();

    static void setDockPosition(const Position side);
    static void setDockDisplayMode(const DisplayMode mode);

    inline virtual ItemType itemType() const {Q_UNREACHABLE(); return App;}

    QSize sizeHint() const override;
    virtual QString accessibleName();

public slots:
    virtual void refershIcon() {}

    void showPopupApplet(QWidget *const applet);
    void hidePopup();
    virtual void setDraging(bool bDrag);

    bool isDragging();
signals:
    void dragStarted() const;
    void itemDropped(QObject *destination, const QPoint &dropPoint) const;
    void requestWindowAutoHide(const bool autoHide) const;
    void requestRefreshWindowVisible() const;

protected:
    bool event(QEvent *event);
    void paintEvent(QPaintEvent *e);
    void mousePressEvent(QMouseEvent *e);
    void enterEvent(QEvent *e);
    void leaveEvent(QEvent *e);

    const QRect perfectIconRect() const;
    const QPoint popupMarkPoint() ;
    const QPoint topleftPoint() const;

    void hideNonModel();
    void popupWindowAccept();
    virtual void showPopupWindow(QWidget *const content, const bool model = false);
    virtual void showHoverTips();
    virtual void invokedMenuItem(const QString &itemId, const bool checked);
    virtual const QString contextMenu() const;
    virtual QWidget *popupTips();

    bool checkAndResetTapHoldGestureState();
    virtual void gestureEvent(QGestureEvent *event);

protected slots:
    void showContextMenu();
    void onContextMenuAccepted();

private:
    void updatePopupPosition();
    void menuActionClicked(QAction *action);

protected:
    bool m_hover;
    bool m_popupShown;
    bool m_tapAndHold;
    bool m_draging;
    QMenu m_contextMenu;

    QPointer<QWidget> m_lastPopupWidget;
    QPointer<HoverHighlightEffect> m_hoverEffect;

    QTimer *m_popupTipsDelayTimer;
    QTimer *m_popupAdjustDelayTimer;

    static Position DockPosition;
    static DisplayMode DockDisplayMode;
    static QPointer<DockPopupWindow> PopupWindow;
};

#endif // DOCKITEM_H
