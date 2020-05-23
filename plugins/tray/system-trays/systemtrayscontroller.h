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

#ifndef SYSTEMTRAYSCONTROLLER_H
#define SYSTEMTRAYSCONTROLLER_H

#include "systemtrayitem.h"
#include "pluginproxyinterface.h"
#include "util/abstractpluginscontroller.h"

#include <com_deepin_dde_daemon_dock.h>

#include <QPluginLoader>
#include <QList>
#include <QMap>
#include <QDBusConnectionInterface>

class PluginsItemInterface;
class SystemTraysController : public AbstractPluginsController
{
    Q_OBJECT

public:
    explicit SystemTraysController(QObject *parent = nullptr);

    // implements PluginProxyInterface
    void itemAdded(PluginsItemInterface * const itemInter, const QString &itemKey) Q_DECL_OVERRIDE;
    void itemUpdate(PluginsItemInterface * const itemInter, const QString &itemKey) Q_DECL_OVERRIDE;
    void itemRemoved(PluginsItemInterface * const itemInter, const QString &itemKey) Q_DECL_OVERRIDE;
    void requestWindowAutoHide(PluginsItemInterface * const itemInter, const QString &itemKey, const bool autoHide) Q_DECL_OVERRIDE;
    void requestRefreshWindowVisible(PluginsItemInterface * const itemInter, const QString &itemKey) Q_DECL_OVERRIDE;
    void requestSetAppletVisible(PluginsItemInterface * const itemInter, const QString &itemKey, const bool visible) Q_DECL_OVERRIDE;

    int systemTrayItemSortKey(const QString &itemKey);
    void setSystemTrayItemSortKey(const QString &itemKey, const int order);

    const QVariant getValueSystemTrayItem(const QString &itemKey, const QString &key, const QVariant& fallback = QVariant());
    void saveValueSystemTrayItem(const QString &itemKey, const QString &key, const QVariant &value);

    void startLoader();

signals:
    void pluginItemAdded(const QString &itemKey, AbstractTrayWidget *pluginItem) const;
    void pluginItemRemoved(const QString &itemKey, AbstractTrayWidget *pluginItem) const;
    void pluginItemUpdated(const QString &itemKey, AbstractTrayWidget *pluginItem) const;
};

#endif // SYSTEMTRAYSCONTROLLER_H
