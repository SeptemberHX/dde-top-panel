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

#ifndef FLOATINGPREVIEW_H
#define FLOATINGPREVIEW_H

#include <QWidget>
#include <QPointer>

#include <dimagebutton.h>
#include <DPushButton>

DWIDGET_USE_NAMESPACE

class AppSnapshot;
class FloatingPreview : public QWidget
{
    Q_OBJECT

public:
    explicit FloatingPreview(QWidget *parent = 0);

    WId trackedWid() const;
    AppSnapshot *trackedWindow();

public slots:
    void trackWindow(AppSnapshot *const snap);

private:
    void paintEvent(QPaintEvent *e) override;
    void mouseReleaseEvent(QMouseEvent *e) override;
    bool eventFilter(QObject *watched, QEvent *event) override;
    void hideEvent(QHideEvent *event) override;

private slots:
    void onCloseBtnClicked();

private:
    QPointer<AppSnapshot> m_tracked;

    DImageButton *m_closeBtn3D;
    DPushButton *m_titleBtn;
};

#endif // FLOATINGPREVIEW_H
