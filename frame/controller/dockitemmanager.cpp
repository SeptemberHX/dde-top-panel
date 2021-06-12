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
#include "pluginsitem.h"
#include "traypluginitem.h"
#include "utils.h"

#include <QDebug>
#include <QGSettings>

#include <DApplication>

DockItemManager *DockItemManager::INSTANCE = nullptr;

DockItemManager::DockItemManager(QObject *parent)
        : QObject(parent)
        , m_appInter(new DBusDock("com.deepin.dde.daemon.Dock", "/com/deepin/dde/daemon/Dock", QDBusConnection::sessionBus(), this))
        , m_pluginsInter(new DockPluginsController(this))
        , m_loadFinished(false)
{
    // 托盘区域和插件区域 由DockPluginsController获取

    // 插件信号
    connect(m_pluginsInter, &DockPluginsController::pluginItemInserted, this, &DockItemManager::pluginItemInserted, Qt::QueuedConnection);
    connect(m_pluginsInter, &DockPluginsController::pluginItemRemoved, this, &DockItemManager::pluginItemRemoved, Qt::QueuedConnection);
    connect(m_pluginsInter, &DockPluginsController::pluginItemUpdated, this, &DockItemManager::itemUpdated, Qt::QueuedConnection);
    connect(m_pluginsInter, &DockPluginsController::trayVisableCountChanged, this, &DockItemManager::trayVisableCountChanged, Qt::QueuedConnection);
    connect(m_pluginsInter, &DockPluginsController::pluginLoaderFinished, this, &DockItemManager::onPluginLoadFinished, Qt::QueuedConnection);

    DApplication *app = qobject_cast<DApplication *>(qApp);
    if (app) {
        connect(app, &DApplication::iconThemeChanged, this, &DockItemManager::refreshItemsIcon);
    }

    connect(qApp, &QApplication::aboutToQuit, this, &QObject::deleteLater);

    // 刷新图标
    QMetaObject::invokeMethod(this, "refreshItemsIcon", Qt::QueuedConnection);
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
    int delay = Utils::SettingValue("com.deepin.dde.dock", "/com/deepin/dde/dock/", "delay-plugins-time", 0).toInt();
    QTimer::singleShot(delay, m_pluginsInter, &DockPluginsController::startLoader);
}

void DockItemManager::refreshItemsIcon()
{
    for (auto item : m_itemList) {
        item->refreshIcon();
        item->update();
    }
}

/**
 * @brief 将插件的参数(Order, Visible, etc)写入gsettings
 * 自动化测试需要通过dbus(GetPluginSettings)获取这些参数
 */
void DockItemManager::updatePluginsItemOrderKey()
{
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
        || moveType == DockItem::FixedPlugin || replaceType == DockItem::FixedPlugin) {
        updatePluginsItemOrderKey();
    }

    // for app move, index 0 is launcher item, need to pass it.
    if (moveType == DockItem::App && replaceType == DockItem::App)
        m_appInter->MoveEntry(moveIndex - 1, replaceIndex - 1);
}

void DockItemManager::itemAdded(const QString &appDesktop, int idx)
{
    m_appInter->RequestDock(appDesktop, idx);
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
    }

    emit itemInserted(insertIndex - firstPluginPosition, item);
}

void DockItemManager::pluginItemRemoved(PluginsItem *item)
{
    item->hidePopup();

    emit itemRemoved(item);

    m_itemList.removeOne(item);

    if (m_loadFinished) {
        updatePluginsItemOrderKey();
    }
}

void DockItemManager::manageItem(DockItem *item)
{
    connect(item, &DockItem::requestRefreshWindowVisible, this, &DockItemManager::requestRefershWindowVisible, Qt::UniqueConnection);
    connect(item, &DockItem::requestWindowAutoHide, this, &DockItemManager::requestWindowAutoHide, Qt::UniqueConnection);
}

void DockItemManager::onPluginLoadFinished()
{
    updatePluginsItemOrderKey();
    m_loadFinished = true;
}
