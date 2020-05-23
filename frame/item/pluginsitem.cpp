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

#include "constants.h"
#include "pluginsitem.h"
#include "pluginsiteminterface.h"

#include "util/imagefactory.h"

#include <QPainter>
#include <QBoxLayout>
#include <QMouseEvent>
#include <QDrag>
#include <QMimeData>
#include <QGSettings>

#define PLUGIN_ITEM_DRAG_THRESHOLD      20

QPoint PluginsItem::MousePressPoint = QPoint();

PluginsItem::PluginsItem(PluginsItemInterface *const pluginInter, const QString &itemKey, QWidget *parent)
    : DockItem(parent)
    , m_pluginInter(pluginInter)
    , m_centralWidget(m_pluginInter->itemWidget(itemKey))
    , m_itemKey(itemKey)
    , m_dragging(false)
    , m_gsettings(nullptr)
{
    qDebug() << "load plugins item: " << pluginInter->pluginName() << itemKey << m_centralWidget;

    m_centralWidget->setParent(this);
    m_centralWidget->setVisible(true);
    m_centralWidget->setAccessibleName("centralwidget");
    m_centralWidget->installEventFilter(this);

    QBoxLayout *hLayout = new QHBoxLayout;
    hLayout->addWidget(m_centralWidget);
    hLayout->setSpacing(0);
    hLayout->setMargin(0);

    setLayout(hLayout);
    setAccessibleName(pluginInter->pluginName() + "-" + m_itemKey);
    setAttribute(Qt::WA_TranslucentBackground);

    const QByteArray &schema{
        QString("com.deepin.dde.dock.module.%1").arg(pluginInter->pluginName()).toUtf8()
    };

    if (QGSettings::isSchemaInstalled(schema)) {
        m_gsettings = new QGSettings(schema);
        m_gsettings->setParent(this);
        connect(m_gsettings, &QGSettings::changed, this,
                &PluginsItem::onGSettingsChanged);
    }
    else {
        m_gsettings = nullptr;
    }
}

PluginsItem::~PluginsItem()
{
}

int PluginsItem::itemSortKey() const
{
    return m_pluginInter->itemSortKey(m_itemKey);
}

void PluginsItem::setItemSortKey(const int order) const
{
    m_pluginInter->setSortKey(m_itemKey, order);
}

void PluginsItem::detachPluginWidget()
{
    QWidget *widget = m_pluginInter->itemWidget(m_itemKey);
    if (widget)
        widget->setParent(nullptr);
}

QString PluginsItem::pluginName() const
{
    return m_pluginInter->pluginName();
}

DockItem::ItemType PluginsItem::itemType() const
{
    if (m_pluginInter->type() == PluginsItemInterface::Normal) {
        return Plugins;
    } else {
        return FixedPlugin;
    }
}

QSize PluginsItem::sizeHint() const
{
    return m_centralWidget->sizeHint();
}

void PluginsItem::refershIcon()
{
    m_pluginInter->refreshIcon(m_itemKey);
}

void PluginsItem::onGSettingsChanged(const QString& key) {
    if (key != "enable" || !m_gsettings) {
        return;
    }

    if (m_gsettings->keys().contains("enable")) {
        setVisible(m_gsettings->get("enable").toBool());
    }
}

QWidget *PluginsItem::centralWidget() const
{
    return m_centralWidget;
}

void PluginsItem::setDraging(bool bDrag)
{
    DockItem::setDraging(bDrag);

    m_centralWidget->setVisible(!bDrag);
}

void PluginsItem::mousePressEvent(QMouseEvent *e)
{
    if (checkGSettingsControl()) {
        return;
    }

    m_hover = false;
    update();

    if (PopupWindow->isVisible())
        hideNonModel();

    if (e->button() == Qt::LeftButton)
        MousePressPoint = e->pos();

    // context menu will handle in DockItem
    DockItem::mousePressEvent(e);
}

void PluginsItem::mouseMoveEvent(QMouseEvent *e)
{
    if (checkGSettingsControl()) {
        return;
    }

    if (e->buttons() != Qt::LeftButton)
        return DockItem::mouseMoveEvent(e);

    e->accept();

    const QPoint distance = e->pos() - MousePressPoint;
    if (distance.manhattanLength() > PLUGIN_ITEM_DRAG_THRESHOLD)
        startDrag();
}

void PluginsItem::mouseReleaseEvent(QMouseEvent *e)
{
    if (checkGSettingsControl()) {
        return;
    }

    DockItem::mouseReleaseEvent(e);

    if (e->button() != Qt::LeftButton)
        return;

    if (checkAndResetTapHoldGestureState() && e->source() == Qt::MouseEventSynthesizedByQt) {
        qDebug() << "tap and hold gesture detected, ignore the synthesized mouse release event";
        return;
    }

    e->accept();

    const QPoint distance = e->pos() - MousePressPoint;
    if (distance.manhattanLength() < PLUGIN_ITEM_DRAG_THRESHOLD)
        mouseClicked();
}

void PluginsItem::enterEvent(QEvent *event)
{
    if (checkGSettingsControl()) {
        return;
    }

    m_hover = true;
    update();

    DockItem::enterEvent(event);
}

void PluginsItem::leaveEvent(QEvent *event)
{
    // Note:
    // here we should check the mouse position to ensure the mouse is really leaved
    // because this leaveEvent will also be called if setX11PassMouseEvent(false) is invoked
    // in XWindowTrayWidget::sendHoverEvent()
        m_hover = false;
        update();

    DockItem::leaveEvent(event);
}

void PluginsItem::showEvent(QShowEvent *event)
{
    QTimer::singleShot(0, this, [ = ] {
        onGSettingsChanged("enable");
    });

    return DockItem::showEvent(event);
}

bool PluginsItem::eventFilter(QObject *watched, QEvent *event)
{
    if (watched == m_centralWidget) {
        if (event->type() == QEvent::MouseButtonPress ||
                event->type() == QEvent::MouseButtonRelease) {
            if (checkGSettingsControl()) {
                return true;
            }
        }
        if (event->type() == QEvent::MouseButtonRelease) {
            m_hover = false;
            update();
        }
    }

    return false;
}

void PluginsItem::invokedMenuItem(const QString &itemId, const bool checked)
{
    m_pluginInter->invokedMenuItem(m_itemKey, itemId, checked);
}

void PluginsItem::showPopupWindow(QWidget *const content, const bool model)
{
    DockItem::showPopupWindow(content, model);
}

const QString PluginsItem::contextMenu() const
{
    return m_pluginInter->itemContextMenu(m_itemKey);
}

QWidget *PluginsItem::popupTips()
{
    if (checkGSettingsControl()) {
        return nullptr;
    }

    return m_pluginInter->itemTipsWidget(m_itemKey);
}

void PluginsItem::resizeEvent(QResizeEvent *event)
{
    setMaximumSize(m_centralWidget->maximumSize());
    return DockItem::resizeEvent(event);
}

void PluginsItem::startDrag()
{
    // 拖拽已放到MainPanelControl处理
    return;

    const QPixmap pixmap = grab();

    m_dragging = true;
    m_centralWidget->setVisible(false);
    update();

    QMimeData *mime = new QMimeData;
    mime->setData(DOCK_PLUGIN_MIME, m_itemKey.toStdString().c_str());

    QDrag *drag = new QDrag(this);
    drag->setPixmap(pixmap);
    drag->setHotSpot(pixmap.rect().center() / pixmap.devicePixelRatioF());
    drag->setMimeData(mime);

    emit dragStarted();
    const Qt::DropAction result = drag->exec(Qt::MoveAction);
    Q_UNUSED(result);
    emit itemDropped(drag->target(), QCursor::pos());

    m_dragging = false;
    m_hover = false;
    m_centralWidget->setVisible(true);
    setVisible(true);
    update();
}

void PluginsItem::mouseClicked()
{
    const QString command = m_pluginInter->itemCommand(m_itemKey);
    if (!command.isEmpty()) {
        QProcess *proc = new QProcess(this);

        connect(proc, static_cast<void (QProcess::*)(int)>(&QProcess::finished), proc, &QProcess::deleteLater);

        proc->startDetached(command);
        return;
    }

    // request popup applet
    QWidget *w = m_pluginInter->itemPopupApplet(m_itemKey);
    if (w)
        showPopupApplet(w);
}

bool PluginsItem::checkGSettingsControl() const
{
    return m_gsettings ? m_gsettings->keys().contains("control") &&
                             m_gsettings->get("control").toBool()
                       : false;
}
