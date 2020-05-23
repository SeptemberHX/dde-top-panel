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

#ifndef PREVIEWCONTAINER_H
#define PREVIEWCONTAINER_H

#include <QWidget>
#include <QBoxLayout>
#include <QTimer>

#include "constants.h"
#include "appsnapshot.h"
#include "floatingpreview.h"

#include <com_deepin_dde_daemon_dock_entry.h>

#include <DWindowManagerHelper>

DWIDGET_USE_NAMESPACE

class PreviewContainer : public QWidget
{
    Q_OBJECT

public:
    explicit PreviewContainer(QWidget *parent = 0);

signals:
    void requestActivateWindow(const WId wid) const;
    void requestPreviewWindow(const WId wid) const;
    void requestCheckWindows() const;
    void requestCancelPreviewWindow() const;
    void requestHidePopup() const;

public:
    void setWindowInfos(const WindowInfoMap &infos, const WindowList &allowClose);
    void updateSnapshots();

public slots:
    void updateLayoutDirection(const Dock::Position dockPos);
    void checkMouseLeave();
    void prepareHide();

private:
    void adjustSize();
    void appendSnapWidget(const WId wid);

    void enterEvent(QEvent *e);
    void leaveEvent(QEvent *e);
    void dragEnterEvent(QDragEnterEvent *e);
    void dragLeaveEvent(QDragLeaveEvent *e);

private slots:
    void onSnapshotClicked(const WId wid);
    void previewEntered(const WId wid);
    void previewFloating();

private:
    bool m_needActivate;
    QMap<WId, AppSnapshot *> m_snapshots;

    FloatingPreview *m_floatingPreview;
    QBoxLayout *m_windowListLayout;

    QTimer *m_mouseLeaveTimer;
    DWindowManagerHelper *m_wmHelper;
    QTimer *m_waitForShowPreviewTimer;
    WId m_currentWId;
};

#endif // PREVIEWCONTAINER_H
