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

#ifndef FASHIONTRAYCONTROLWIDGET_H
#define FASHIONTRAYCONTROLWIDGET_H

#include "constants.h"

#include <QLabel>

class FashionTrayControlWidget : public QWidget
{
    Q_OBJECT

public:
    explicit FashionTrayControlWidget(Dock::Position position, QWidget *parent = nullptr);

    void setDockPostion(Dock::Position pos);

    bool expanded() const;
    void setExpanded(const bool &expanded);

Q_SIGNALS:
    void expandChanged(const bool expanded);

protected:
    void paintEvent(QPaintEvent *event) Q_DECL_OVERRIDE;
    void mouseReleaseEvent(QMouseEvent *event) Q_DECL_OVERRIDE;
    void mousePressEvent(QMouseEvent *event) Q_DECL_OVERRIDE;
    void enterEvent(QEvent *event) Q_DECL_OVERRIDE;
    void leaveEvent(QEvent *event) Q_DECL_OVERRIDE;
    void resizeEvent(QResizeEvent *event) override;

private:
    void refreshArrowPixmap();

private:
    QTimer *m_expandDelayTimer;
    QPixmap m_arrowPix;

    Dock::Position m_dockPosition;
    bool m_expanded;
    bool m_hover;
    bool m_pressed;
};

#endif // FASHIONTRAYCONTROLWIDGET_H
