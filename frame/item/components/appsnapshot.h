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

#ifndef APPSNAPSHOT_H
#define APPSNAPSHOT_H

#include <QWidget>
#include <QDebug>
#include <QTimer>
#include "../widgets/tipswidget.h"

#include <dimagebutton.h>
#include <DWindowManagerHelper>

#include <com_deepin_dde_daemon_dock_entry.h>

DWIDGET_USE_NAMESPACE
DGUI_USE_NAMESPACE

#define SNAP_WIDTH       200
#define SNAP_HEIGHT      130

struct SHMInfo;
struct _XImage;
typedef _XImage XImage;

class AppSnapshot : public QWidget
{
    Q_OBJECT

public:
    explicit AppSnapshot(const WId wid, QWidget *parent = 0);

    inline WId wid() const { return m_wid; }
    inline bool attentioned() const { return m_windowInfo.attention; }
    inline bool closeAble() const { return m_closeAble; }
    inline void setCloseAble(const bool value) { m_closeAble = value; }
    inline const QImage snapshot() const { return m_snapshot; }
    inline const QRectF snapshotGeometry() const { return m_snapshotSrcRect; }
    inline const QString title() const { return m_windowInfo.title; }

signals:
    void entered(const WId wid) const;
    void clicked(const WId wid) const;
    void requestCheckWindow() const;

public slots:
    void fetchSnapshot();
    void closeWindow() const;
    void compositeChanged() const;
    void setWindowInfo(const WindowInfo &info);

private:
    void dragEnterEvent(QDragEnterEvent *e);
    void enterEvent(QEvent *e);
    void leaveEvent(QEvent *e);
    void paintEvent(QPaintEvent *e);
    void resizeEvent(QResizeEvent *e);
    void mousePressEvent(QMouseEvent *e);
    SHMInfo *getImageDSHM();
    XImage * getImageXlib();
    QRect rectRemovedShadow(const QImage &qimage, unsigned char *prop_to_return_gtk);

private:
    const WId m_wid;
    WindowInfo m_windowInfo;

    bool m_closeAble;

    QImage m_snapshot;
    QRectF m_snapshotSrcRect;

    TipsWidget *m_title;
    QTimer *m_waitLeaveTimer;
    DImageButton *m_closeBtn2D;
    DWindowManagerHelper *m_wmHelper;
};

#endif // APPSNAPSHOT_H
