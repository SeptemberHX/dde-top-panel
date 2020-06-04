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

#include "fashiontraycontrolwidget.h"

#include <QMouseEvent>
#include <QPainter>
#include <QPainterPath>
#include <DHiDPIHelper>
#include <DStyle>
#include <DGuiApplicationHelper>

#include "../frame/util/imageutil.h"

DWIDGET_USE_NAMESPACE

#define ExpandedKey "fashion-tray-expanded"

FashionTrayControlWidget::FashionTrayControlWidget(Dock::Position position, QWidget *parent)
    : QWidget(parent),
      m_expandDelayTimer(new QTimer(this)),
      m_dockPosition(position),
      m_expanded(true),
      m_hover(false),
      m_pressed(false)
{
    m_expandDelayTimer->setInterval(400);
    m_expandDelayTimer->setSingleShot(true);

    setDockPostion(m_dockPosition);
    setExpanded(m_expanded);

    setMinimumSize(PLUGIN_BACKGROUND_MIN_SIZE, PLUGIN_BACKGROUND_MIN_SIZE);
    setMaximumSize(PLUGIN_BACKGROUND_MAX_SIZE, PLUGIN_BACKGROUND_MAX_SIZE);

    connect(DGuiApplicationHelper::instance(), &DGuiApplicationHelper::themeTypeChanged, this, [ = ] {
        update();
    });
}

void FashionTrayControlWidget::setDockPostion(Dock::Position pos)
{
    m_dockPosition = pos;
    refreshArrowPixmap();
}

bool FashionTrayControlWidget::expanded() const
{
    return m_expanded;
}

void FashionTrayControlWidget::setExpanded(const bool &expanded)
{
    if (m_expanded == expanded) {
        return;
    }

    m_expanded = expanded;
    refreshArrowPixmap();

    Q_EMIT expandChanged(m_expanded);
}

void FashionTrayControlWidget::paintEvent(QPaintEvent *event)
{
    QPainter painter(this);
    painter.setRenderHint(QPainter::Antialiasing, true);

    QColor color;
    if (DGuiApplicationHelper::instance()->themeType() == DGuiApplicationHelper::LightType) {
        color = Qt::black;
        painter.setOpacity(0.5);

        if (m_hover) {
            painter.setOpacity(0.6);
        }

        if (m_pressed) {
            painter.setOpacity(0.3);
        }
    } else {
        color = Qt::white;
        painter.setOpacity(0.1);

        if (m_hover) {
            painter.setOpacity(0.2);
        }

        if (m_pressed) {
            painter.setOpacity(0.05);
        }
    }

    // draw background
    QPainterPath path;
    if (rect().height() > PLUGIN_BACKGROUND_MIN_SIZE) {
        DStyleHelper dstyle(style());
        const int radius = dstyle.pixelMetric(DStyle::PM_FrameRadius);

        int minSize = std::min(width(), height());
        QRect rc(0, 0, minSize, minSize);
        rc.moveTo(rect().center() - rc.center());

        path.addRoundedRect(rc, radius, radius);
        painter.fillPath(path, color);
    }
    // reset opacity
    painter.setOpacity(1);

    // draw arrow pixmap
    refreshArrowPixmap();
    QRectF rf = QRectF(rect());
    QRectF rfp = QRectF(m_arrowPix.rect());
    QPointF p = rf.center() - rfp.center() / m_arrowPix.devicePixelRatioF();

    painter.drawPixmap(p, m_arrowPix);
}

void FashionTrayControlWidget::mouseReleaseEvent(QMouseEvent *event)
{
    if (m_expandDelayTimer->isActive()) {
        return;
    }

    m_expandDelayTimer->start();

    m_pressed = false;
    update();

    if (event->button() == Qt::LeftButton) {
        event->accept();
        setExpanded(!m_expanded);
        return;
    }

    QWidget::mouseReleaseEvent(event);
}

void FashionTrayControlWidget::mousePressEvent(QMouseEvent *event)
{
    if (event->button() != Qt::LeftButton) {
        event->ignore();
        return QWidget::mousePressEvent(event);
    }

    m_pressed = true;
    update();

    QWidget::mousePressEvent(event);
}

void FashionTrayControlWidget::enterEvent(QEvent *event)
{
    m_hover = true;
    update();

    QWidget::enterEvent(event);
}

void FashionTrayControlWidget::leaveEvent(QEvent *event)
{
    m_hover = false;
    m_pressed = false;
    update();

    QWidget::leaveEvent(event);
}

void FashionTrayControlWidget::resizeEvent(QResizeEvent *event)
{
    QWidget::resizeEvent(event);
}

void FashionTrayControlWidget::refreshArrowPixmap()
{
    QString iconPath;

    switch (m_dockPosition) {
    case Dock::Top:
    case Dock::Bottom:
        iconPath = m_expanded ? "arrow-right" : "arrow-left";
        break;
    case Dock::Left:
    case Dock::Right:
        iconPath = m_expanded ?  "arrow-down" : "arrow-up";
        break;
    }

    if (height() <= PLUGIN_BACKGROUND_MIN_SIZE && DGuiApplicationHelper::instance()->themeType() == DGuiApplicationHelper::LightType) {
        iconPath.append("-dark");
    }

    const auto ratio = devicePixelRatioF();
    m_arrowPix = ImageUtil::loadSvg(iconPath, ":/icons/resources/", PLUGIN_ICON_MAX_SIZE, ratio);
}
