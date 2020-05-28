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

#ifndef ABSTRACTPLUGINSCONTROLLER_H
#define ABSTRACTPLUGINSCONTROLLER_H

#include "pluginproxyinterface.h"
#include "util/pluginloader.h"

#include <QPluginLoader>
#include <QList>
#include <QMap>
#include <QDBusConnectionInterface>
#include <QGSettings>

class PluginsItemInterface;
class AbstractPluginsController : public QObject, PluginProxyInterface
{
    Q_OBJECT

public:
    explicit AbstractPluginsController(QObject *parent = 0);

    // implements PluginProxyInterface
    void saveValue(PluginsItemInterface *const itemInter, const QString &key, const QVariant &value) Q_DECL_OVERRIDE;
    const QVariant getValue(PluginsItemInterface *const itemInter, const QString &key, const QVariant& fallback = QVariant()) Q_DECL_OVERRIDE;
    void removeValue(PluginsItemInterface * const itemInter, const QStringList &keyList) override;

protected:
    QMap<PluginsItemInterface *, QMap<QString, QObject *>> &pluginsMap();
    QObject *pluginItemAt(PluginsItemInterface * const itemInter, const QString &itemKey) const;
    PluginsItemInterface *pluginInterAt(const QString &itemKey);
    PluginsItemInterface *pluginInterAt(QObject *destItem);
    bool enableBlacklist;

protected Q_SLOTS:
    void startLoader(PluginLoader *loader);

private slots:
    void displayModeChanged();
    void positionChanged();
    void loadPlugin(const QString &pluginFile);
    void initPlugin(PluginsItemInterface *interface);
    void refreshPluginSettings();

private:
    bool eventFilter(QObject *o, QEvent *e) Q_DECL_OVERRIDE;

private:
    QDBusConnectionInterface *m_dbusDaemonInterface;
    QGSettings *m_gsettings;

    QMap<PluginsItemInterface *, QMap<QString, QObject *>> m_pluginsMap;

    QJsonObject m_pluginSettingsObject;
};

#endif // ABSTRACTPLUGINSCONTROLLER_H
