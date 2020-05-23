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

#include "traypluginitem.h"

#include <QEvent>
#include <QGSettings>

TrayPluginItem::TrayPluginItem(PluginsItemInterface * const pluginInter, const QString &itemKey, QWidget *parent)
    : PluginsItem(pluginInter, itemKey, parent)
{
    centralWidget()->installEventFilter(this);
}

void TrayPluginItem::setSuggestIconSize(QSize size)
{
    // invoke the method "setSuggestIconSize" of FashionTrayItem class
    QMetaObject::invokeMethod(centralWidget(), "setSuggestIconSize", Qt::QueuedConnection, Q_ARG(QSize, size));
}

void TrayPluginItem::setRightSplitVisible(const bool visible)
{
    // invoke the method "setRightSplitVisible" of FashionTrayItem class
    QMetaObject::invokeMethod(centralWidget(), "setRightSplitVisible", Qt::QueuedConnection, Q_ARG(bool, visible));
}

int TrayPluginItem::trayVisableItemCount()
{
    return m_trayVisableItemCount;
}

bool TrayPluginItem::eventFilter(QObject *watched, QEvent *e)
{
    // 时尚模式下
    // 监听插件Widget的"FashionTraySize"属性
    // 当接收到这个属性变化的事件后，重新计算和设置dock的大小

    if (watched == centralWidget()) {
        if (e->type() == QEvent::MouseButtonPress ||
                e->type() == QEvent::MouseButtonRelease) {
            QGSettings settings("com.deepin.dde.dock.module.systemtray");
            if (settings.keys().contains("control")
                    && settings.get("control").toBool()) {
                return true;
            }
        }
    }

    if (watched == centralWidget() && e->type() == QEvent::DynamicPropertyChange) {
        const QString &propertyName = static_cast<QDynamicPropertyChangeEvent *>(e)->propertyName();
        if (propertyName == "TrayVisableItemCount") {
            m_trayVisableItemCount = watched->property("TrayVisableItemCount").toInt();
            Q_EMIT trayVisableCountChanged(m_trayVisableItemCount);
        }
    }

    return PluginsItem::eventFilter(watched, e);
}
