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

#include "dockitemmanager.h"
#include "item/appitem.h"
#include "item/pluginsitem.h"
#include "item/traypluginitem.h"
#include "util/docksettings.h"

#include <QDebug>
#include <QGSettings>

DockItemManager *DockItemManager::INSTANCE = nullptr;

DockItemManager::DockItemManager(QObject *parent, bool enableBlacklist)
    : QObject(parent)
    , m_updatePluginsOrderTimer(new QTimer(this))
    , m_appInter(new DBusDock("com.deepin.dde.daemon.Dock", "/com/deepin/dde/daemon/Dock", QDBusConnection::sessionBus(), this))
    , m_pluginsInter(new DockPluginsController(enableBlacklist, this))
{
    // 应用区域
    for (auto entry : m_appInter->entries()) {
        AppItem *it = new AppItem(entry);
        manageItem(it);

        connect(it, &AppItem::requestActivateWindow, m_appInter, &DBusDock::ActivateWindow, Qt::QueuedConnection);
        connect(it, &AppItem::requestPreviewWindow, m_appInter, &DBusDock::PreviewWindow);
        connect(it, &AppItem::requestCancelPreview, m_appInter, &DBusDock::CancelPreviewWindow);
        connect(it, &AppItem::windowInfoChanged, this, &DockItemManager::windowInfoChanged);

        m_itemList.append(it);
    }

    // 托盘区域和插件区域 由DockPluginsController获取

    // 更新插件顺序
    m_updatePluginsOrderTimer->setSingleShot(true);
    m_updatePluginsOrderTimer->setInterval(1000);
    connect(m_updatePluginsOrderTimer, &QTimer::timeout, this, &DockItemManager::updatePluginsItemOrderKey);

    // 应用信号
    connect(m_appInter, &DBusDock::EntryAdded, this, &DockItemManager::appItemAdded);
    connect(m_appInter, &DBusDock::EntryRemoved, this, static_cast<void (DockItemManager::*)(const QString &)>(&DockItemManager::appItemRemoved), Qt::QueuedConnection);
    connect(m_appInter, &DBusDock::ServiceRestarted, this, &DockItemManager::reloadAppItems);

    // 插件信号
    connect(m_pluginsInter, &DockPluginsController::pluginItemInserted, this, &DockItemManager::pluginItemInserted, Qt::QueuedConnection);
    connect(m_pluginsInter, &DockPluginsController::pluginItemRemoved, this, &DockItemManager::pluginItemRemoved, Qt::QueuedConnection);
    connect(m_pluginsInter, &DockPluginsController::pluginItemUpdated, this, &DockItemManager::itemUpdated, Qt::QueuedConnection);
    connect(m_pluginsInter, &DockPluginsController::trayVisableCountChanged, this, &DockItemManager::trayVisableCountChanged, Qt::QueuedConnection);

    // 刷新图标
    QMetaObject::invokeMethod(this, "refershItemsIcon", Qt::QueuedConnection);
}


DockItemManager *DockItemManager::instance(QObject *parent)
{
    if (!INSTANCE)
        INSTANCE = new DockItemManager(parent);

    return INSTANCE;
}

const QList<QPointer<DockItem>> DockItemManager::itemList() const
{
    return m_itemList;
}

const QList<PluginsItemInterface *> DockItemManager::pluginList() const
{
    return m_pluginsInter->pluginsMap().keys();
}

bool DockItemManager::appIsOnDock(const QString &appDesktop) const
{
    return m_appInter->IsOnDock(appDesktop);
}

void DockItemManager::startLoadPlugins() const
{
    QGSettings gsetting("com.deepin.dde.dock", "/com/deepin/dde/dock/");

    QTimer::singleShot(gsetting.get("delay-plugins-time").toUInt(), m_pluginsInter, &DockPluginsController::startLoader);
}

void DockItemManager::refershItemsIcon()
{
    for (auto item : m_itemList) {
        item->refershIcon();
        item->update();
    }
}

void DockItemManager::updatePluginsItemOrderKey()
{
    Q_ASSERT(sender() == m_updatePluginsOrderTimer);

    int index = 0;
    for (auto item : m_itemList) {
        if (item.isNull() || item->itemType() != DockItem::Plugins)
            continue;
        static_cast<PluginsItem *>(item.data())->setItemSortKey(++index);
    }

    // 固定区域插件排序
    index = 0;
    for (auto item : m_itemList) {
        if (item.isNull() || item->itemType() != DockItem::FixedPlugin)
            continue;
        static_cast<PluginsItem *>(item.data())->setItemSortKey(++index);
    }
}

void DockItemManager::itemMoved(DockItem *const sourceItem, DockItem *const targetItem)
{
    Q_ASSERT(sourceItem != targetItem);

    const DockItem::ItemType moveType = sourceItem->itemType();
    const DockItem::ItemType replaceType = targetItem->itemType();

    // app move
    if (moveType == DockItem::App || moveType == DockItem::Placeholder)
        if (replaceType != DockItem::App)
            return;

    // plugins move
    if (moveType == DockItem::Plugins || moveType == DockItem::TrayPlugin)
        if (replaceType != DockItem::Plugins && replaceType != DockItem::TrayPlugin)
            return;

    const int moveIndex = m_itemList.indexOf(sourceItem);
    const int replaceIndex = m_itemList.indexOf(targetItem);

    m_itemList.removeAt(moveIndex);
    m_itemList.insert(replaceIndex, sourceItem);

    // update plugins sort key if order changed
    if (moveType == DockItem::Plugins || replaceType == DockItem::Plugins
            || moveType == DockItem::TrayPlugin || replaceType == DockItem::TrayPlugin
            || moveType == DockItem::FixedPlugin || replaceType == DockItem::FixedPlugin)
        m_updatePluginsOrderTimer->start();

    // for app move, index 0 is launcher item, need to pass it.
    if (moveType == DockItem::App && replaceType == DockItem::App)
        m_appInter->MoveEntry(moveIndex - 1, replaceIndex - 1);
}

void DockItemManager::itemAdded(const QString &appDesktop, int idx)
{
    m_appInter->RequestDock(appDesktop, idx);
}

void DockItemManager::appItemAdded(const QDBusObjectPath &path, const int index)
{
    // 第一个是启动器
    int insertIndex = 1;

    // -1 for append to app list end
    if (index != -1) {
        insertIndex += index;
    } else {
        for (auto item : m_itemList)
            if (item->itemType() == DockItem::App)
                ++insertIndex;
    }

    AppItem *item = new AppItem(path);
    manageItem(item);

    connect(item, &AppItem::requestActivateWindow, m_appInter, &DBusDock::ActivateWindow, Qt::QueuedConnection);
    connect(item, &AppItem::requestPreviewWindow, m_appInter, &DBusDock::PreviewWindow);
    connect(item, &AppItem::requestCancelPreview, m_appInter, &DBusDock::CancelPreviewWindow);
    connect(item, &AppItem::windowInfoChanged, this, &DockItemManager::windowInfoChanged);

    m_itemList.insert(insertIndex, item);

    if (index != -1) {
        emit itemInserted(insertIndex - 1, item);
        return;
    }

    emit itemInserted(insertIndex, item);
}

void DockItemManager::appItemRemoved(const QString &appId)
{
    for (int i(0); i != m_itemList.size(); ++i) {
        if (m_itemList[i]->itemType() != DockItem::App)
            continue;

        AppItem *app = static_cast<AppItem *>(m_itemList[i].data());
        if (!app) {
            continue;
        }
        if (!app->isValid() || app->appId() == appId) {
            appItemRemoved(app);
        }
    }
}

void DockItemManager::appItemRemoved(AppItem *appItem)
{
    emit itemRemoved(appItem);
    m_itemList.removeOne(appItem);

    if (appItem->isDragging()) {
        QDrag::cancel();
    }
    appItem->deleteLater();
}

void DockItemManager::pluginItemInserted(PluginsItem *item)
{
    manageItem(item);

    DockItem::ItemType pluginType = item->itemType();

    // find first plugins item position
    int firstPluginPosition = -1;
    for (int i(0); i != m_itemList.size(); ++i) {
        DockItem::ItemType type = m_itemList[i]->itemType();
        if (type != pluginType)
            continue;

        firstPluginPosition = i;
        break;
    }

    if (firstPluginPosition == -1)
        firstPluginPosition = m_itemList.size();

    // find insert position
    int insertIndex = 0;
    const int itemSortKey = item->itemSortKey();
    if (itemSortKey == -1 || firstPluginPosition == -1) {
        insertIndex = m_itemList.size();
    } else if (itemSortKey == 0) {
        insertIndex = firstPluginPosition;
    } else {
        insertIndex = m_itemList.size();
        for (int i(firstPluginPosition + 1); i != m_itemList.size() + 1; ++i) {
            PluginsItem *pItem = static_cast<PluginsItem *>(m_itemList[i - 1].data());
            Q_ASSERT(pItem);

            const int sortKey = pItem->itemSortKey();
            if (pluginType == DockItem::FixedPlugin) {
                if (sortKey != -1 && itemSortKey > sortKey)
                    continue;
                insertIndex = i - 1;
                break;
            }
            if (sortKey != -1 && itemSortKey > sortKey && pItem->itemType() != DockItem::FixedPlugin)
                continue;
            insertIndex = i - 1;
            break;
        }
    }

    m_itemList.insert(insertIndex, item);
    if(pluginType == DockItem::FixedPlugin)
    {
        insertIndex ++;
        item->setAccessibleName("plugins");
    }

    emit itemInserted(insertIndex - firstPluginPosition, item);
}

void DockItemManager::pluginItemRemoved(PluginsItem *item)
{
    item->hidePopup();

    emit itemRemoved(item);

    m_itemList.removeOne(item);
}

void DockItemManager::reloadAppItems()
{
    // remove old item
    for (auto item : m_itemList)
        if (item->itemType() == DockItem::App)
            appItemRemoved(static_cast<AppItem *>(item.data()));

    // append new item
    for (auto path : m_appInter->entries())
        appItemAdded(path, -1);
}

// 不同的模式下，插件顺序不一样
void DockItemManager::sortPluginItems()
{
    int firstPluginIndex = -1;
    for (int i(0); i != m_itemList.size(); ++i) {
        if (m_itemList[i]->itemType() == DockItem::Plugins) {
            firstPluginIndex = i;
            break;
        }
    }

    if (firstPluginIndex == -1)
        return;

    std::sort(m_itemList.begin() + firstPluginIndex, m_itemList.end(), [](DockItem * a, DockItem * b) -> bool {
        PluginsItem *pa = static_cast<PluginsItem *>(a);
        PluginsItem *pb = static_cast<PluginsItem *>(b);

        const int aKey = pa->itemSortKey();
        const int bKey = pb->itemSortKey();

        if (bKey == -1)
            return true;
        if (aKey == -1)
            return false;

        return aKey < bKey;
    });

    // reset order
    for (int i(firstPluginIndex); i != m_itemList.size(); ++i) {
        emit itemRemoved(m_itemList[i]);
        emit itemInserted(-1, m_itemList[i]);
    }
}

void DockItemManager::manageItem(DockItem *item)
{
    connect(item, &DockItem::requestRefreshWindowVisible, this, &DockItemManager::requestRefershWindowVisible, Qt::UniqueConnection);
    connect(item, &DockItem::requestWindowAutoHide, this, &DockItemManager::requestWindowAutoHide, Qt::UniqueConnection);
}
