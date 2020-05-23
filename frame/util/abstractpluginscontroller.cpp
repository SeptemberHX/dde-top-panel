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

#include "abstractpluginscontroller.h"
#include "pluginsiteminterface.h"
#include "DNotifySender"

#include <DSysInfo>
#include <QDebug>
#include <QDir>
#include <QGSettings>

static const QStringList CompatiblePluginApiList {
    "1.1.1",
    "1.2",
    "1.2.1",
    DOCK_PLUGIN_API_VERSION
};

AbstractPluginsController::AbstractPluginsController(QObject *parent)
    : QObject(parent)
    , m_dbusDaemonInterface(QDBusConnection::sessionBus().interface())
    , m_dockDaemonInter(new DockDaemonInter("com.deepin.dde.daemon.Dock", "/com/deepin/dde/daemon/Dock", QDBusConnection::sessionBus(), this))
{
    qApp->installEventFilter(this);

    refreshPluginSettings();

    connect(m_dockDaemonInter, &DockDaemonInter::PluginSettingsSynced, this, &AbstractPluginsController::refreshPluginSettings, Qt::QueuedConnection);
}

void AbstractPluginsController::saveValue(PluginsItemInterface *const itemInter, const QString &key, const QVariant &value)
{
    // is it necessary?
//    refreshPluginSettings();

    // save to local cache
    QJsonObject localObject = m_pluginSettingsObject.value(itemInter->pluginName()).toObject();
    localObject.insert(key, QJsonValue::fromVariant(value)); //Note: QVariant::toJsonValue() not work in Qt 5.7
    m_pluginSettingsObject.insert(itemInter->pluginName(), localObject);

    // save to daemon
    QJsonObject remoteObject, remoteObjectInter;
    remoteObjectInter.insert(key, QJsonValue::fromVariant(value)); //Note: QVariant::toJsonValue() not work in Qt 5.7
    remoteObject.insert(itemInter->pluginName(), remoteObjectInter);
    m_dockDaemonInter->MergePluginSettings(QJsonDocument(remoteObject).toJson(QJsonDocument::JsonFormat::Compact));
}

const QVariant AbstractPluginsController::getValue(PluginsItemInterface *const itemInter, const QString &key, const QVariant &fallback)
{
    // load from local cache
    QVariant v = m_pluginSettingsObject.value(itemInter->pluginName()).toObject().value(key).toVariant();
    if (v.isNull() || !v.isValid()) {
        v = fallback;
    }

    return v;
}

void AbstractPluginsController::removeValue(PluginsItemInterface *const itemInter, const QStringList &keyList)
{
    if (keyList.isEmpty()) {
        m_pluginSettingsObject.remove(itemInter->pluginName());
    } else {
        QJsonObject localObject = m_pluginSettingsObject.value(itemInter->pluginName()).toObject();
        for (auto key : keyList) {
            localObject.remove(key);
        }
        m_pluginSettingsObject.insert(itemInter->pluginName(), localObject);
    }

    m_dockDaemonInter->RemovePluginSettings(itemInter->pluginName(), keyList);
}

QMap<PluginsItemInterface *, QMap<QString, QObject *> > &AbstractPluginsController::pluginsMap()
{
    return m_pluginsMap;
}

QObject *AbstractPluginsController::pluginItemAt(PluginsItemInterface *const itemInter, const QString &itemKey) const
{
    if (!m_pluginsMap.contains(itemInter))
        return nullptr;

    return m_pluginsMap[itemInter][itemKey];
}

PluginsItemInterface *AbstractPluginsController::pluginInterAt(const QString &itemKey)
{
    for (auto it = m_pluginsMap.constBegin(); it != m_pluginsMap.constEnd(); ++it) {
        for (auto key : it.value().keys()) {
            if (key == itemKey) {
                return it.key();
            }
        }
    }

    return nullptr;
}

PluginsItemInterface *AbstractPluginsController::pluginInterAt(QObject *destItem)
{
    for (auto it = m_pluginsMap.constBegin(); it != m_pluginsMap.constEnd(); ++it) {
        for (auto item : it.value().values()) {
            if (item == destItem) {
                return it.key();
            }
        }
    }

    return nullptr;
}

void AbstractPluginsController::startLoader(PluginLoader *loader)
{
    connect(loader, &PluginLoader::finished, loader, &PluginLoader::deleteLater, Qt::QueuedConnection);
    connect(loader, &PluginLoader::pluginFounded, this, &AbstractPluginsController::loadPlugin, Qt::QueuedConnection);

    QGSettings gsetting("com.deepin.dde.dock", "/com/deepin/dde/dock/");

    QTimer::singleShot(gsetting.get("delay-plugins-time").toUInt(),
                       loader, [ = ] { loader->start(QThread::LowestPriority); });
}

void AbstractPluginsController::displayModeChanged()
{
    const Dock::DisplayMode displayMode = qApp->property(PROP_DISPLAY_MODE).value<Dock::DisplayMode>();
    const auto inters = m_pluginsMap.keys();

    for (auto inter : inters)
        inter->displayModeChanged(displayMode);
}

void AbstractPluginsController::positionChanged()
{
    const Dock::Position position = qApp->property(PROP_POSITION).value<Dock::Position>();
    const auto inters = m_pluginsMap.keys();

    for (auto inter : inters)
        inter->positionChanged(position);
}

void AbstractPluginsController::loadPlugin(const QString &pluginFile)
{
    QPluginLoader *pluginLoader = new QPluginLoader(pluginFile);
    const QJsonObject &meta = pluginLoader->metaData().value("MetaData").toObject();
    const QString &pluginApi = meta.value("api").toString();
    bool pluginIsValid = true;
    if (pluginApi.isEmpty() || !CompatiblePluginApiList.contains(pluginApi)) {
        qWarning() << objectName()
                   << "plugin api version not matched! expect versions:" << CompatiblePluginApiList
                   << ", got version:" << pluginApi
                   << ", the plugin file is:" << pluginFile;

        pluginIsValid = false;
    }

    PluginsItemInterface *interface = qobject_cast<PluginsItemInterface *>(pluginLoader->instance());
    if (!interface) {
        qWarning() << objectName() << "load plugin failed!!!" << pluginLoader->errorString() << pluginFile;

        pluginLoader->unload();
        pluginLoader->deleteLater();

        pluginIsValid = false;
    }

    if (!pluginIsValid) {
        QString notifyMessage(tr("The plugin %1 is not compatible with the system."));
        Dtk::Core::DUtil::DNotifySender(notifyMessage.arg(QFileInfo(pluginFile).fileName())).appIcon("dialog-warning").call();
        return;
    }

    if (interface->pluginName() == "multitasking") {
        if (qEnvironmentVariable("XDG_SESSION_TYPE").contains("wayland") or Dtk::Core::DSysInfo::deepinType() == Dtk::Core::DSysInfo::DeepinServer)
            return;
    }

    m_pluginsMap.insert(interface, QMap<QString, QObject *>());

    QString dbusService = meta.value("depends-daemon-dbus-service").toString();
    if (!dbusService.isEmpty() && !m_dbusDaemonInterface->isServiceRegistered(dbusService).value()) {
        qDebug() << objectName() << dbusService << "daemon has not started, waiting for signal";
        connect(m_dbusDaemonInterface, &QDBusConnectionInterface::serviceOwnerChanged, this,
        [ = ](const QString & name, const QString & oldOwner, const QString & newOwner) {
            Q_UNUSED(oldOwner);
            if (name == dbusService && !newOwner.isEmpty()) {
                qDebug() << objectName() << dbusService << "daemon started, init plugin and disconnect";
                initPlugin(interface);
                disconnect(m_dbusDaemonInterface);
            }
        }
               );
        return;
    }

    // NOTE(justforlxz): 插件的所有初始化工作都在init函数中进行，
    // loadPlugin函数是按队列执行的，initPlugin函数会有可能导致
    // 函数执行被阻塞。
    QTimer::singleShot(1, this, [ = ] {
        initPlugin(interface);
    });
}

void AbstractPluginsController::initPlugin(PluginsItemInterface *interface)
{
    qDebug() << objectName() << "init plugin: " << interface->pluginName();
    interface->init(this);
    qDebug() << objectName() << "init plugin finished: " << interface->pluginName();
}

void AbstractPluginsController::refreshPluginSettings()
{
    const QString &pluginSettings = m_dockDaemonInter->GetPluginSettings().value();
    if (pluginSettings.isEmpty()) {
        qDebug() << "Error! get plugin settings from dbus failed!";
        return;
    }

    const QJsonObject &pluginSettingsObject = QJsonDocument::fromJson(pluginSettings.toLocal8Bit()).object();
    if (pluginSettingsObject.isEmpty()) {
        return;
    }

    // nothing changed
    if (pluginSettingsObject == m_pluginSettingsObject) {
        return;
    }

    for (auto pluginsIt = pluginSettingsObject.constBegin(); pluginsIt != pluginSettingsObject.constEnd(); ++pluginsIt) {
        const QString &pluginName = pluginsIt.key();
        const QJsonObject &settingsObject = pluginsIt.value().toObject();
        QJsonObject newSettingsObject = m_pluginSettingsObject.value(pluginName).toObject();
        for (auto settingsIt = settingsObject.constBegin(); settingsIt != settingsObject.constEnd(); ++settingsIt) {
            newSettingsObject.insert(settingsIt.key(), settingsIt.value());
        }
        // TODO: remove not exists key-values
        m_pluginSettingsObject.insert(pluginName, newSettingsObject);
    }

    // not notify plugins to refresh settings if this update is not emit by dock daemon
    if (sender() != m_dockDaemonInter) {
        return;
    }

    // notify all plugins to reload plugin settings
    for (PluginsItemInterface *pluginInter : m_pluginsMap.keys()) {
        pluginInter->pluginSettingsChanged();
    }

    // reload all plugin items for sort order or container
    QMap<PluginsItemInterface *, QMap<QString, QObject *>> pluginsMapTemp = m_pluginsMap;
    for (auto it = pluginsMapTemp.constBegin(); it != pluginsMapTemp.constEnd(); ++it) {
        const QList<QString> &itemKeyList = it.value().keys();
        for (auto key : itemKeyList) {
            itemRemoved(it.key(), key);
        }
        for (auto key : itemKeyList) {
            itemAdded(it.key(), key);
        }
    }
}

bool AbstractPluginsController::eventFilter(QObject *o, QEvent *e)
{
    if (o != qApp)
        return false;
    if (e->type() != QEvent::DynamicPropertyChange)
        return false;

    QDynamicPropertyChangeEvent *const dpce = static_cast<QDynamicPropertyChangeEvent *>(e);
    const QString propertyName = dpce->propertyName();

    if (propertyName == PROP_POSITION)
        positionChanged();
    else if (propertyName == PROP_DISPLAY_MODE)
        displayModeChanged();

    return false;
}
