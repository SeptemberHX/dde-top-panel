/*
 * Copyright (C) 2019 ~ 2019 Deepin Technology Co., Ltd.
 *
 * Author:     wangshaojun <wangshaojun_cm@deepin.com>
 *
 * Maintainer: wangshaojun <wangshaojun_cm@deepin.com>
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

#ifndef DOCKITEMMANAGER_H
#define DOCKITEMMANAGER_H

#include "dockpluginscontroller.h"
#include "pluginsiteminterface.h"
#include "dockitem.h"
#include "placeholderitem.h"

#include <com_deepin_dde_daemon_dock.h>

#include <QObject>

using DBusDock = com::deepin::dde::daemon::Dock;
/**
 * @brief The DockItemManager class
 * 管理类，管理所有的应用数据，插件数据
 */
class DockItemManager : public QObject
{
Q_OBJECT

public:
    static DockItemManager *instance(QObject *parent = nullptr);
    explicit DockItemManager(QObject *parent = nullptr);

    const QList<QPointer<DockItem> > itemList() const;
    const QList<PluginsItemInterface *> pluginList() const;
    bool appIsOnDock(const QString &appDesktop) const;
    void startLoadPlugins() const;

signals:
    void itemInserted(const int index, DockItem *item) const;
    void itemRemoved(DockItem *item) const;
    void itemUpdated(DockItem *item) const;
    void trayVisableCountChanged(const int &count) const;
    void requestWindowAutoHide(const bool autoHide) const;
    void requestRefershWindowVisible() const;

    void requestUpdateDockItem() const;

public slots:
    void refreshItemsIcon();
    void itemMoved(DockItem *const sourceItem, DockItem *const targetItem);
    void itemAdded(const QString &appDesktop, int idx);

private Q_SLOTS:
    void onPluginLoadFinished();

private:
    void pluginItemInserted(PluginsItem *item);
    void pluginItemRemoved(PluginsItem *item);
    void updatePluginsItemOrderKey();
    void manageItem(DockItem *item);

private:
    DBusDock *m_appInter;
    DockPluginsController *m_pluginsInter;

    static DockItemManager *INSTANCE;

    QList<QPointer<DockItem>> m_itemList;
    QList<QString> m_appIDist;

    bool m_loadFinished; // 记录所有插件是否加载完成

    static const QGSettings *m_appSettings;
    static const QGSettings *m_activeSettings;
    static const QGSettings *m_dockedSettings;

    bool enableBlacklist;
};

#endif // DOCKITEMMANAGER_H
