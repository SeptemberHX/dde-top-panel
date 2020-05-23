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

#ifndef DOCKPLUGINSCONTROLLER_H
#define DOCKPLUGINSCONTROLLER_H

#include "item/pluginsitem.h"
#include "pluginproxyinterface.h"
#include "util/abstractpluginscontroller.h"

#include <com_deepin_dde_daemon_dock.h>

#include <QPluginLoader>
#include <QList>
#include <QMap>
#include <QDBusConnectionInterface>

class PluginsItemInterface;
class DockPluginsController : public AbstractPluginsController
{
    Q_OBJECT

    friend class DockItemController;
    friend class DockItemManager;

public:
    explicit DockPluginsController(QObject *parent = 0);

    // implements PluginProxyInterface
    void itemAdded(PluginsItemInterface * const itemInter, const QString &itemKey) Q_DECL_OVERRIDE;
    void itemUpdate(PluginsItemInterface * const itemInter, const QString &itemKey) Q_DECL_OVERRIDE;
    void itemRemoved(PluginsItemInterface * const itemInter, const QString &itemKey) Q_DECL_OVERRIDE;
    void requestWindowAutoHide(PluginsItemInterface * const itemInter, const QString &itemKey, const bool autoHide) Q_DECL_OVERRIDE;
    void requestRefreshWindowVisible(PluginsItemInterface * const itemInter, const QString &itemKey) Q_DECL_OVERRIDE;
    void requestSetAppletVisible(PluginsItemInterface * const itemInter, const QString &itemKey, const bool visible) Q_DECL_OVERRIDE;

    void startLoader();

signals:
    void pluginItemInserted(PluginsItem *pluginItem) const;
    void pluginItemRemoved(PluginsItem *pluginItem) const;
    void pluginItemUpdated(PluginsItem *pluginItem) const;
    void trayVisableCountChanged(const int &count) const;

private:
    void loadLocalPlugins();
    void loadSystemPlugins();
};

#endif // DOCKPLUGINSCONTROLLER_H
