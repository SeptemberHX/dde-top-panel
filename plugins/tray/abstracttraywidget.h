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

#pragma once

#include <QWidget>
#include <QTimer>
#include <QPair>

class QDBusMessage;
class AbstractTrayWidget: public QWidget
{
    Q_OBJECT
public:
    enum TrayType {
        ApplicationTray,
        SystemTray,
    };

    explicit AbstractTrayWidget(QWidget *parent = Q_NULLPTR, Qt::WindowFlags f = Qt::WindowFlags());
    virtual ~AbstractTrayWidget();

    virtual QString itemKeyForConfig() = 0;
    virtual void setActive(const bool active) = 0;
    virtual void updateIcon() = 0;
    virtual void sendClick(uint8_t mouseButton, int x, int y) = 0;
    virtual const QImage trayImage() = 0;
    virtual inline TrayType trayTyep() const { return TrayType::ApplicationTray; } // default is ApplicationTray
    virtual bool isValid() {return true;}

Q_SIGNALS:
    void iconChanged();
    void clicked();
    void needAttention();
    void requestWindowAutoHide(const bool autoHide);
    void requestRefershWindowVisible();

protected:
    void mousePressEvent(QMouseEvent *event) Q_DECL_OVERRIDE;
    void mouseReleaseEvent(QMouseEvent *e) Q_DECL_OVERRIDE;

    void handleMouseRelease();
    const QRect perfectIconRect() const;
    void resizeEvent(QResizeEvent *event) override;

private:
    QTimer *m_handleMouseReleaseTimer;

    QPair<QPoint, Qt::MouseButton> m_lastMouseReleaseData;
};

