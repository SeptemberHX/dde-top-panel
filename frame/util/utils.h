/*
 * Copyright (C) 2018 ~ 2028 Uniontech Technology Co., Ltd.
 *
 * Author:     fanpengcheng <fanpengcheng@uniontech.com>
 *
 * Maintainer: fanpengcheng <fanpengcheng@uniontech.com>
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
#ifndef UTILS
#define UTILS
#include <QPixmap>
#include <QImageReader>
#include <QApplication>
#include <QScreen>
#include <QGSettings>
#include <QDebug>

namespace Utils {

#define ICBC_CONF_FILE "/etc/deepin/icbc.conf"

/**
 * @brief SettingsPtr 根据给定信息返回一个QGSettings指针
 * @param schema_id The id of the schema
 * @param path If non-empty, specifies the path for a relocatable schema
 * @param parent 创建指针的付对象
 * @return
 */
    inline const QGSettings *SettingsPtr(const QString &schema_id, const QByteArray &path = QByteArray(), QObject *parent = nullptr) {
        if (QGSettings::isSchemaInstalled(schema_id.toUtf8())) {
            QGSettings *settings = new QGSettings(schema_id.toUtf8(), path, parent);
            return settings;
        }
        qDebug() << "Cannot find gsettings, schema_id:" << schema_id;
        return nullptr;
    }

/**
 * @brief SettingsPtr 根据给定信息返回一个QGSettings指针
 * @param module 传入QGSettings构造函数时，会添加"com.deepin.dde.dock.module."前缀
 * @param path If non-empty, specifies the path for a relocatable schema
 * @param parent 创建指针的付对象
 * @return
 */
    inline const QGSettings *ModuleSettingsPtr(const QString &module, const QByteArray &path = QByteArray(), QObject *parent = nullptr) {
        return SettingsPtr("com.deepin.dde.dock.module." + module, path, parent);
    }

/* convert 'some-key' to 'someKey' or 'SomeKey'.
 * the second form is needed for appending to 'set' for 'setSomeKey'
 */
    inline QString qtify_name(const char *name)
    {
        bool next_cap = false;
        QString result;

        while (*name) {
            if (*name == '-') {
                next_cap = true;
            } else if (next_cap) {
                result.append(QChar(*name).toUpper().toLatin1());
                next_cap = false;
            } else {
                result.append(*name);
            }

            name++;
        }

        return result;
    }

/**
 * @brief SettingValue 根据给定信息返回获取的值
 * @param schema_id The id of the schema
 * @param path If non-empty, specifies the path for a relocatable schema
 * @param key 对应信息的key值
 * @param fallback 如果找不到信息，返回此默认值
 * @return
 */
    inline const QVariant SettingValue(const QString &schema_id, const QByteArray &path = QByteArray(), const QString &key = QString(), const QVariant &fallback = QVariant()){
        const QGSettings *settings = SettingsPtr(schema_id, path);

        if (settings && ((settings->keys().contains(key)) || settings->keys().contains(qtify_name(key.toUtf8().data())))) {
            QVariant v = settings->get(key);
            delete settings;
            return v;
        } else{
            qDebug() << "Cannot find gsettings, schema_id:" << schema_id
                     << " path:" << path << " key:" << key
                     << "Use fallback value:" << fallback;
            return fallback;
        }
    }

    inline QPixmap renderSVG(const QString &path, const QSize &size, const qreal devicePixelRatio) {
        QImageReader reader;
        QPixmap pixmap;
        reader.setFileName(path);
        if (reader.canRead()) {
            reader.setScaledSize(size * devicePixelRatio);
            pixmap = QPixmap::fromImage(reader.read());
            pixmap.setDevicePixelRatio(devicePixelRatio);
        }
        else {
            pixmap.load(path);
        }

        return pixmap;
    }

    inline QScreen *screenAt(const QPoint &point) {
        for (QScreen *screen : qApp->screens()) {
            const QRect r { screen->geometry() };
            const QRect rect { r.topLeft(), r.size() * screen->devicePixelRatio() };
            if (rect.contains(point)) {
                return screen;
            }
        }

        return nullptr;
    }

//!!! 注意:这里传入的QPoint是未计算缩放的
    inline QScreen *screenAtByScaled(const QPoint &point) {
        for (QScreen *screen : qApp->screens()) {
            const QRect r { screen->geometry() };
            QRect rect { r.topLeft(), r.size() * screen->devicePixelRatio() };
            if (rect.contains(point)) {
                return screen;
            }
        }

        return nullptr;
    }

    inline bool isSettingConfigured(const QString& id, const QString& path, const QString& keyName) {
        if (!QGSettings::isSchemaInstalled(id.toUtf8())) {
            return false;
        }
        QGSettings setting(id.toUtf8(), path.toUtf8());
        QVariant v = setting.get(keyName);
        if (!v.isValid()) {
            return false;
        }
        return v.toBool();
    }

/**
* @brief 比较两个插件版本号的大小
* @param pluginApi1 第一个插件版本号
* @param pluginApi2 第二个插件版本号
* @return 0:两个版本号相等,1:第一个版本号大,-1:第二个版本号大
*/
    inline int comparePluginApi(const QString &pluginApi1, const QString &pluginApi2) {
        // 版本号相同
        if (pluginApi1 == pluginApi2)
            return 0;

        // 拆分版本号
        QStringList subPluginApis1 = pluginApi1.split(".", QString::SkipEmptyParts, Qt::CaseSensitive);
        QStringList subPluginApis2 = pluginApi2.split(".", QString::SkipEmptyParts, Qt::CaseSensitive);
        for (int i = 0; i < subPluginApis1.size(); ++i) {
            auto subPluginApi1 = subPluginApis1[i];
            if (subPluginApis2.size() > i) {
                auto subPluginApi2 = subPluginApis2[i];

                // 相等判断下一个子版本号
                if (subPluginApi1 == subPluginApi2)
                    continue;

                // 转成整形比较
                if (subPluginApi1.toInt() > subPluginApi2.toInt()) {
                    return 1;
                } else {
                    return -1;
                }
            }
        }

        // 循环结束但是没有返回,说明子版本号个数不同,且前面的子版本号都相同
        // 子版本号多的版本号大
        if (subPluginApis1.size() > subPluginApis2.size()) {
            return 1;
        } else {
            return -1;
        }
    }
}

#endif // UTILS
