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

#include "fashiontraywidgetwrapper.h"
#include "../xembedtraywidget.h"

#include <QPainter>
#include <QDebug>
#include <QMouseEvent>
#include <QDrag>
#include <QMimeData>

#include <DStyle>
#include <DGuiApplicationHelper>

#include "constants.h"

#define TRAY_ITEM_DRAG_THRESHOLD 20

DWIDGET_USE_NAMESPACE

FashionTrayWidgetWrapper::FashionTrayWidgetWrapper(const QString &itemKey, AbstractTrayWidget *absTrayWidget, QWidget *parent)
    : QWidget(parent),
      m_absTrayWidget(absTrayWidget),
      m_layout(new QVBoxLayout(this)),
      m_attention(false),
      m_dragging(false),
      m_hover(false),
      m_pressed(false),
      m_itemKey(itemKey)

{
    setStyleSheet("background: transparent;");
    setAcceptDrops(true);

    m_layout->setSpacing(0);
    m_layout->setMargin(0);
    m_layout->setContentsMargins(0, 0, 0, 0);

    m_layout->addWidget(m_absTrayWidget);

    setLayout(m_layout);

    connect(m_absTrayWidget, &AbstractTrayWidget::needAttention, this, &FashionTrayWidgetWrapper::onTrayWidgetNeedAttention);
    connect(m_absTrayWidget, &AbstractTrayWidget::clicked, this, &FashionTrayWidgetWrapper::onTrayWidgetClicked);

    setMinimumSize(PLUGIN_BACKGROUND_MIN_SIZE, PLUGIN_BACKGROUND_MIN_SIZE);

    m_absTrayWidget->show();
}

QPointer<AbstractTrayWidget> FashionTrayWidgetWrapper::absTrayWidget() const
{
    return m_absTrayWidget;
}

QString FashionTrayWidgetWrapper::itemKey() const
{
    return m_itemKey;
}

void FashionTrayWidgetWrapper::paintEvent(QPaintEvent *event)
{
    Q_UNUSED(event);

    if (m_dragging) {
        return;
    }

    if (rect().height() > PLUGIN_BACKGROUND_MIN_SIZE) {
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

        DStyleHelper dstyle(style());
        const int radius = dstyle.pixelMetric(DStyle::PM_FrameRadius);

        QPainterPath path;

        int minSize = std::min(width(), height());
        QRect rc(0, 0, minSize, minSize);
        rc.moveTo(rect().center() - rc.center());

        path.addRoundedRect(rc, radius, radius);
        painter.fillPath(path, color);
    }
}

void FashionTrayWidgetWrapper::mousePressEvent(QMouseEvent *event)
{
    if (event->button() == Qt::MouseButton::LeftButton) {
        MousePressPoint = event->pos();
    }

    m_pressed = true;
    update();

    QWidget::mousePressEvent(event);
}

void FashionTrayWidgetWrapper::mouseMoveEvent(QMouseEvent *event)
{
    handleMouseMove(event);
}

void FashionTrayWidgetWrapper::mouseReleaseEvent(QMouseEvent *event)
{
    m_pressed = false;
    m_hover = false;
    update();

    QWidget::mouseReleaseEvent(event);
}

void FashionTrayWidgetWrapper::dragEnterEvent(QDragEnterEvent *event)
{
    if (event->mimeData()->hasFormat(TRAY_ITEM_DRAG_MIMEDATA)) {
        event->accept();
        Q_EMIT requestSwapWithDragging();
        return;
    }

    QWidget::dragEnterEvent(event);
}

void FashionTrayWidgetWrapper::enterEvent(QEvent *event)
{
    m_hover = true;
    update();

    QWidget::enterEvent(event);
}

void FashionTrayWidgetWrapper::leaveEvent(QEvent *event)
{
    // Note:
    // here we should check the mouse position to ensure the mouse is really leaved
    // because this leaveEvent will also be called if setX11PassMouseEvent(false) is invoked
    // in XWindowTrayWidget::sendHoverEvent()

    if (qobject_cast<XEmbedTrayWidget *>(m_absTrayWidget) && rect().contains(mapFromGlobal(QCursor::pos()))) {
        return QWidget::leaveEvent(event);
    }

    m_hover = false;
    m_pressed = false;
    update();

    QWidget::leaveEvent(event);
}

void FashionTrayWidgetWrapper::resizeEvent(QResizeEvent *event)
{
    const Dock::Position position = qApp->property(PROP_POSITION).value<Dock::Position>();
    // 保持横纵比
    if (position == Dock::Bottom || position == Dock::Top) {
        setMaximumWidth(height());
        setMaximumHeight(QWIDGETSIZE_MAX);
    } else {
        setMaximumHeight(width());
        setMaximumWidth(QWIDGETSIZE_MAX);
    }

    QWidget::resizeEvent(event);
}

void FashionTrayWidgetWrapper::handleMouseMove(QMouseEvent *event)
{
    if(m_absTrayWidget.isNull())
        return;

    if (event->buttons() != Qt::MouseButton::LeftButton) {
        return QWidget::mouseMoveEvent(event);
    }

    if ((event->pos() - MousePressPoint).manhattanLength() < TRAY_ITEM_DRAG_THRESHOLD) {
        return;
    }

    event->accept();

    QDrag drag(this);
    QMimeData *mimeData = new QMimeData;
    mimeData->setData(TRAY_ITEM_DRAG_MIMEDATA, m_itemKey.toLocal8Bit());
    QPixmap pixmap = grab();

    drag.setMimeData(mimeData);
    drag.setPixmap(pixmap);
    drag.setHotSpot(pixmap.rect().center() / pixmap.devicePixelRatioF());

    m_absTrayWidget->setVisible(false);
    m_dragging = true;
    Q_EMIT dragStart();

    // start drag
    drag.exec();

    m_absTrayWidget->setVisible(true);
    m_dragging = false;
    m_hover = false;
    m_pressed = false;
    Q_EMIT dragStop();
}

void FashionTrayWidgetWrapper::onTrayWidgetNeedAttention()
{
    setAttention(true);
}

void FashionTrayWidgetWrapper::onTrayWidgetClicked()
{
    setAttention(false);
}

bool FashionTrayWidgetWrapper::attention() const
{
    return m_attention;
}

void FashionTrayWidgetWrapper::setAttention(bool attention)
{
    m_attention = attention;

    Q_EMIT attentionChanged(m_attention);
}

bool FashionTrayWidgetWrapper::isDragging()
{
    return m_dragging;
}

void FashionTrayWidgetWrapper::cancelDragging()
{
    QDrag::cancel();
}
