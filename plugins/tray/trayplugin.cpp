/*
 * Copyright (C) 2011 ~ 2018 Deepin Technology Co., Ltd.
 *
 * Author:     sbw <sbw@sbw.so>
 *
 * Maintainer: sbw <sbw@sbw.so>
 *             listenerri <listenerri@gmail.com>
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

#include "trayplugin.h"
#include "fashiontray/fashiontrayitem.h"
#include "snitraywidget.h"

#include <QDir>
#include <QWindow>
#include <QWidget>
#include <QX11Info>
#include <QGSettings>

#include "../widgets/tipswidget.h"
#include "xcb/xcb_icccm.h"

#define PLUGIN_ENABLED_KEY "enable"
#define FASHION_MODE_TRAYS_SORTED   "fashion-mode-trays-sorted"

#define SNI_WATCHER_SERVICE "org.kde.StatusNotifierWatcher"
#define SNI_WATCHER_PATH "/StatusNotifierWatcher"

using org::kde::StatusNotifierWatcher;

TrayPlugin::TrayPlugin(QObject *parent)
    : QObject(parent)
    , m_pluginLoaded(false)
{
}

const QString TrayPlugin::pluginName() const
{
    return "tray";
}

void TrayPlugin::init(PluginProxyInterface *proxyInter)
{
    // transfex config
    QSettings settings("deepin", "dde-dock-shutdown");
    if (QFile::exists(settings.fileName())) {
        proxyInter->saveValue(this, "enable", settings.value("enable", true));

        QFile::remove(settings.fileName());
    }

    m_proxyInter = proxyInter;

    if (pluginIsDisable()) {
        qDebug() << "hide tray from config disable!!";
        return;
    }

    if (m_pluginLoaded) {
        return;
    }

    m_pluginLoaded = true;

    m_trayInter = new DBusTrayManager(this);
    m_sniWatcher = new StatusNotifierWatcher(SNI_WATCHER_SERVICE, SNI_WATCHER_PATH, QDBusConnection::sessionBus(), this);
    m_fashionItem = new FashionTrayItem(this);
    m_systemTraysController = new SystemTraysController(this);
    m_refreshXEmbedItemsTimer = new QTimer(this);
    m_refreshSNIItemsTimer = new QTimer(this);
    m_tipsLabel = new TipsWidget;

    m_refreshXEmbedItemsTimer->setInterval(0);
    m_refreshXEmbedItemsTimer->setSingleShot(true);

    m_refreshSNIItemsTimer->setInterval(0);
    m_refreshSNIItemsTimer->setSingleShot(true);

    m_tipsLabel->setObjectName("tray");
    m_tipsLabel->setText(tr("System Tray"));
    m_tipsLabel->setVisible(false);

    connect(m_systemTraysController, &SystemTraysController::pluginItemAdded, this, &TrayPlugin::addTrayWidget);
    connect(m_systemTraysController, &SystemTraysController::pluginItemRemoved, this, [ = ](const QString & itemKey) { trayRemoved(itemKey); });

    m_trayInter->Manage();

    switchToMode(displayMode());

    QTimer::singleShot(0, this, &TrayPlugin::loadIndicator);
    QTimer::singleShot(0, m_systemTraysController, &SystemTraysController::startLoader);
    QTimer::singleShot(0, this, &TrayPlugin::initSNI);
    QTimer::singleShot(0, this, &TrayPlugin::initXEmbed);
}

bool TrayPlugin::pluginIsDisable()
{
    // NOTE(justforlxz): local config
    QSettings enableSetting("deepin", "dde-dock");
    enableSetting.beginGroup("tray");
    if (!enableSetting.value("enable", true).toBool()) {
        return true;
    }

    if (!m_proxyInter)
        return true;

    return !m_proxyInter->getValue(this, PLUGIN_ENABLED_KEY, true).toBool();
}

void TrayPlugin::displayModeChanged(const Dock::DisplayMode mode)
{
    if (pluginIsDisable()) {
        return;
    }

    switchToMode(displayMode());
}

void TrayPlugin::positionChanged(const Dock::Position position)
{
    if (pluginIsDisable()) {
        return;
    }

    m_fashionItem->setDockPosition(position);
}

QWidget *TrayPlugin::itemWidget(const QString &itemKey)
{
    if (itemKey == FASHION_MODE_ITEM_KEY) {
        return m_fashionItem;
    }

    return m_trayMap.value(itemKey);
}

QWidget *TrayPlugin::itemTipsWidget(const QString &itemKey)
{
    Q_UNUSED(itemKey);

    return nullptr;
}

QWidget *TrayPlugin::itemPopupApplet(const QString &itemKey)
{
    Q_UNUSED(itemKey);

    return nullptr;
}

int TrayPlugin::itemSortKey(const QString &itemKey)
{
    // 如果是系统托盘图标则调用内部插件的相应接口
    if (isSystemTrayItem(itemKey)) {
        return m_systemTraysController->systemTrayItemSortKey(itemKey);
    }

    const int defaultSort = displayMode() == Dock::DisplayMode::Fashion ? 0 : 0;

    AbstractTrayWidget *const trayWidget = m_trayMap.value(itemKey, nullptr);
    if (trayWidget == nullptr) {
        return defaultSort;
    }

    const QString key = QString("pos_%1_%2").arg(trayWidget->itemKeyForConfig()).arg(Dock::Efficient);

    return m_proxyInter->getValue(this, key, defaultSort).toInt();
}

void TrayPlugin::setSortKey(const QString &itemKey, const int order)
{
    if (displayMode() == Dock::DisplayMode::Fashion && !traysSortedInFashionMode()) {
        m_proxyInter->saveValue(this, FASHION_MODE_TRAYS_SORTED, true);
    }

    // 如果是系统托盘图标则调用内部插件的相应接口
    if (isSystemTrayItem(itemKey)) {
        return m_systemTraysController->setSystemTrayItemSortKey(itemKey, order);
    }

    AbstractTrayWidget *const trayWidget = m_trayMap.value(itemKey, nullptr);
    if (trayWidget == nullptr) {
        return;
    }

    const QString key = QString("pos_%1_%2").arg(trayWidget->itemKeyForConfig()).arg(Dock::Efficient);
    m_proxyInter->saveValue(this, key, order);
}

void TrayPlugin::refreshIcon(const QString &itemKey)
{
    if (itemKey == FASHION_MODE_ITEM_KEY) {
        for (auto trayWidget : m_trayMap.values()) {
            if (trayWidget) {
                trayWidget->updateIcon();
            }
        }
        return;
    }

    AbstractTrayWidget *const trayWidget = m_trayMap.value(itemKey);
    if (trayWidget) {
        trayWidget->updateIcon();
    }
}

void TrayPlugin::pluginSettingsChanged()
{
    if (pluginIsDisable()) {
        return;
    }

    if (displayMode() == Dock::DisplayMode::Fashion) {
        m_fashionItem->onPluginSettingsChanged();
        m_fashionItem->clearTrayWidgets();
        m_fashionItem->setTrayWidgets(m_trayMap);
    }
}

Dock::Position TrayPlugin::dockPosition() const
{
    return position();
}

bool TrayPlugin::traysSortedInFashionMode()
{
    return m_proxyInter->getValue(this, FASHION_MODE_TRAYS_SORTED, false).toBool();
}

void TrayPlugin::saveValue(const QString &itemKey, const QString &key, const QVariant &value)
{
    // 如果是系统托盘图标则调用内部插件的相应接口
    if (isSystemTrayItem(itemKey)) {
        return m_systemTraysController->saveValueSystemTrayItem(itemKey, key, value);
    }

    m_proxyInter->saveValue(this, key, value);
}

const QVariant TrayPlugin::getValue(const QString &itemKey, const QString &key, const QVariant &fallback)
{
    // 如果是系统托盘图标则调用内部插件的相应接口
    if (isSystemTrayItem(itemKey)) {
        return m_systemTraysController->getValueSystemTrayItem(itemKey, key, fallback);
    }

    return m_proxyInter->getValue(this, key, fallback);
}

bool TrayPlugin::isSystemTrayItem(const QString &itemKey)
{
    AbstractTrayWidget *const trayWidget = m_trayMap.value(itemKey, nullptr);

    if (trayWidget && trayWidget->trayTyep() == AbstractTrayWidget::TrayType::SystemTray) {
        return true;
    }

    return false;
}

QString TrayPlugin::itemKeyOfTrayWidget(AbstractTrayWidget *trayWidget)
{
    QString itemKey;

    if (displayMode() == Dock::DisplayMode::Fashion) {
        itemKey = FASHION_MODE_ITEM_KEY;
    } else {
        itemKey = m_trayMap.key(trayWidget);
    }

    return itemKey;
}

Dock::DisplayMode TrayPlugin::displayMode()
{
    return Dock::DisplayMode::Fashion;
}

void TrayPlugin::initXEmbed()
{
    connect(m_refreshXEmbedItemsTimer, &QTimer::timeout, this, &TrayPlugin::xembedItemsChanged);
    connect(m_trayInter, &DBusTrayManager::TrayIconsChanged, this, [ = ] {m_refreshXEmbedItemsTimer->start();});
    connect(m_trayInter, &DBusTrayManager::Changed, this, &TrayPlugin::xembedItemChanged);

    m_refreshXEmbedItemsTimer->start();
}

void TrayPlugin::initSNI()
{
    connect(m_refreshSNIItemsTimer, &QTimer::timeout, this, &TrayPlugin::sniItemsChanged);
    connect(m_sniWatcher, &StatusNotifierWatcher::StatusNotifierItemRegistered, this, [ = ] {m_refreshSNIItemsTimer->start();});
    connect(m_sniWatcher, &StatusNotifierWatcher::StatusNotifierItemUnregistered, this, [ = ] {m_refreshSNIItemsTimer->start();});

    m_refreshSNIItemsTimer->start();
}

void TrayPlugin::sniItemsChanged()
{
    const QStringList &itemServicePaths = m_sniWatcher->registeredStatusNotifierItems();
    QStringList sinTrayKeyList;

    for (auto item : itemServicePaths) {
        sinTrayKeyList << SNITrayWidget::toSNIKey(item);
    }
    for (auto itemKey : m_trayMap.keys()) {
        if (!sinTrayKeyList.contains(itemKey) && SNITrayWidget::isSNIKey(itemKey)) {
            trayRemoved(itemKey);
        }
    }
    const QList<QString> &passiveSNIKeyList = m_passiveSNITrayMap.keys();
    for (auto itemKey : passiveSNIKeyList) {
        if (!sinTrayKeyList.contains(itemKey) && SNITrayWidget::isSNIKey(itemKey)) {
            m_passiveSNITrayMap.take(itemKey)->deleteLater();
        }
    }

    for (int i = 0; i < sinTrayKeyList.size(); ++i) {
        traySNIAdded(sinTrayKeyList.at(i), itemServicePaths.at(i));
    }
}

void TrayPlugin::xembedItemsChanged()
{
    QList<quint32> winidList = m_trayInter->trayIcons();
    QStringList trayKeyList;

    for (auto winid : winidList) {
        trayKeyList << XEmbedTrayWidget::toXEmbedKey(winid);
    }

    for (auto tray : m_trayMap.keys()) {
        if (!trayKeyList.contains(tray) && XEmbedTrayWidget::isXEmbedKey(tray)) {
            trayRemoved(tray);
        }
    }

    for (int i = 0; i < trayKeyList.size(); ++i) {
        trayXEmbedAdded(trayKeyList.at(i), winidList.at(i));
    }
}

void TrayPlugin::addTrayWidget(const QString &itemKey, AbstractTrayWidget *trayWidget)
{
    if (!trayWidget) {
        return;
    }

    if (m_trayMap.contains(itemKey) || m_trayMap.values().contains(trayWidget)) {
        return;
    }

    m_trayMap.insert(itemKey, trayWidget);

    if (displayMode() == Dock::Efficient) {
        m_proxyInter->itemAdded(this, itemKey);
    } else {
        m_proxyInter->itemAdded(this, FASHION_MODE_ITEM_KEY);
        m_fashionItem->trayWidgetAdded(itemKey, trayWidget);
    }

    connect(trayWidget, &AbstractTrayWidget::requestWindowAutoHide, this, &TrayPlugin::onRequestWindowAutoHide, Qt::UniqueConnection);
    connect(trayWidget, &AbstractTrayWidget::requestRefershWindowVisible, this, &TrayPlugin::onRequestRefershWindowVisible, Qt::UniqueConnection);
}

void TrayPlugin::trayXEmbedAdded(const QString &itemKey, quint32 winId)
{
    if (m_trayMap.contains(itemKey) || !XEmbedTrayWidget::isXEmbedKey(itemKey)) {
        return;
    }

    QGSettings settings("com.deepin.dde.dock.module.systemtray");
    if (settings.keys().contains("enable") && !settings.get("enable").toBool()) {
        return;
    }

    AbstractTrayWidget *trayWidget = new XEmbedTrayWidget(winId);
    if (trayWidget->isValid())
        addTrayWidget(itemKey, trayWidget);
    else {
        qDebug() << "-- invalid tray windowid" << winId;
    }
}

void TrayPlugin::traySNIAdded(const QString &itemKey, const QString &sniServicePath)
{
    if (m_trayMap.contains(itemKey) || !SNITrayWidget::isSNIKey(itemKey) || m_passiveSNITrayMap.contains(itemKey)) {
        return;
    }

    QGSettings settings("com.deepin.dde.dock.module.systemtray");
    if (settings.keys().contains("enable") && !settings.get("enable").toBool()) {
        return;
    }

    if (sniServicePath.startsWith("/") || !sniServicePath.contains("/")) {
        qDebug() << "SNI service path invalid";
        return;
    }

    QStringList list = sniServicePath.split("/");
    QString nsiServerName = list.takeFirst();

    QProcess p;
    p.start("qdbus", {nsiServerName});
    if (!p.waitForFinished(1000)) {
        qWarning() << "sni dbus service error : " << nsiServerName;
        return;
    }

    SNITrayWidget *trayWidget = new SNITrayWidget(sniServicePath);
    if (!trayWidget->isValid())
        return;

    if (trayWidget->status() == SNITrayWidget::ItemStatus::Passive) {
        m_passiveSNITrayMap.insert(itemKey, trayWidget);
    } else {
        addTrayWidget(itemKey, trayWidget);
    }

    connect(trayWidget, &SNITrayWidget::statusChanged, this, &TrayPlugin::onSNIItemStatusChanged);
}

void TrayPlugin::trayIndicatorAdded(const QString &itemKey, const QString &indicatorName)
{
    if (m_trayMap.contains(itemKey) || !IndicatorTrayWidget::isIndicatorKey(itemKey)) {
        return;
    }

    QGSettings settings("com.deepin.dde.dock.module.systemtray");
    if (settings.keys().contains("enable") && !settings.get("enable").toBool()) {
        return;
    }

    IndicatorTray *indicatorTray = nullptr;
    if (!m_indicatorMap.keys().contains(indicatorName)) {
        indicatorTray = new IndicatorTray(indicatorName);
        m_indicatorMap[indicatorName] = indicatorTray;
    } else {
        indicatorTray = m_indicatorMap[itemKey];
    }

    connect(indicatorTray, &IndicatorTray::delayLoaded,
    indicatorTray, [ = ]() {
        addTrayWidget(itemKey, indicatorTray->widget());
    }, Qt::UniqueConnection);

    connect(indicatorTray, &IndicatorTray::removed, this, [ = ] {
        trayRemoved(itemKey);
        indicatorTray->removeWidget();
    }, Qt::UniqueConnection);
}

void TrayPlugin::trayRemoved(const QString &itemKey, const bool deleteObject)
{
    if (!m_trayMap.contains(itemKey)) {
        return;
    }

    AbstractTrayWidget *widget = m_trayMap.take(itemKey);

    if (displayMode() == Dock::Efficient) {
        m_proxyInter->itemRemoved(this, itemKey);
    } else {
        m_fashionItem->trayWidgetRemoved(widget);
    }

    // only delete tray object when it is a tray of applications
    // set the parent of the tray object to avoid be deconstructed by parent(DockItem/PluginsItem/TrayPluginsItem)
    if (widget->trayTyep() == AbstractTrayWidget::TrayType::SystemTray) {
        widget->setParent(nullptr);
    } else if (deleteObject) {
        widget->deleteLater();
    }
}

void TrayPlugin::xembedItemChanged(quint32 winId)
{
    QString itemKey = XEmbedTrayWidget::toXEmbedKey(winId);
    if (!m_trayMap.contains(itemKey)) {
        return;
    }

    m_trayMap.value(itemKey)->updateIcon();
}

void TrayPlugin::switchToMode(const Dock::DisplayMode mode)
{
    if (!m_proxyInter)
        return;

    if (mode == Dock::Fashion) {
        for (auto itemKey : m_trayMap.keys()) {
            m_proxyInter->itemRemoved(this, itemKey);
        }
        if (m_trayMap.isEmpty()) {
            m_proxyInter->itemRemoved(this, FASHION_MODE_ITEM_KEY);
        } else {
            m_fashionItem->setTrayWidgets(m_trayMap);
            m_proxyInter->itemAdded(this, FASHION_MODE_ITEM_KEY);
        }
    } else {
        m_fashionItem->clearTrayWidgets();
        m_proxyInter->itemRemoved(this, FASHION_MODE_ITEM_KEY);
        for (auto itemKey : m_trayMap.keys()) {
            m_proxyInter->itemAdded(this, itemKey);
        }
    }
}

void TrayPlugin::onRequestWindowAutoHide(const bool autoHide)
{
    const QString &itemKey = itemKeyOfTrayWidget(static_cast<AbstractTrayWidget *>(sender()));

    if (itemKey.isEmpty()) {
        return;
    }

    m_proxyInter->requestWindowAutoHide(this, itemKey, autoHide);
}

void TrayPlugin::onRequestRefershWindowVisible()
{
    const QString &itemKey = itemKeyOfTrayWidget(static_cast<AbstractTrayWidget *>(sender()));

    if (itemKey.isEmpty()) {
        return;
    }

    m_proxyInter->requestRefreshWindowVisible(this, itemKey);
}

void TrayPlugin::onSNIItemStatusChanged(SNITrayWidget::ItemStatus status)
{
    SNITrayWidget *trayWidget = static_cast<SNITrayWidget *>(sender());
    if (!trayWidget) {
        return;
    }

    QString itemKey;
    do {
        itemKey = m_trayMap.key(trayWidget);
        if (!itemKey.isEmpty()) {
            break;
        }

        itemKey = m_passiveSNITrayMap.key(trayWidget);
        if (itemKey.isEmpty()) {
            qDebug() << "Error! not found the status changed SNI tray!";
            return;
        }
    } while (false);

    switch (status) {
    case SNITrayWidget::Passive: {
        m_passiveSNITrayMap.insert(itemKey, trayWidget);
        trayRemoved(itemKey, false);
        break;
    }
    case SNITrayWidget::Active:
    case SNITrayWidget::NeedsAttention: {
        m_passiveSNITrayMap.remove(itemKey);
        addTrayWidget(itemKey, trayWidget);
        break;
    }
    default:
        break;
    }
}

void TrayPlugin::loadIndicator()
{
    QDir indicatorConfDir("/etc/dde-dock/indicator");

    for (const QFileInfo &fileInfo : indicatorConfDir.entryInfoList({"*.json"}, QDir::Files | QDir::NoDotAndDotDot)) {
        const QString &indicatorName = fileInfo.baseName();
        trayIndicatorAdded(IndicatorTrayWidget::toIndicatorKey(indicatorName), indicatorName);
    }
}
