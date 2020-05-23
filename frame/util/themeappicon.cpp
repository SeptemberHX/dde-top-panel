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

#include "themeappicon.h"

#include <QIcon>
#include <QFile>
#include <QDebug>
#include <QApplication>
#include <QPixmapCache>
#include <QCryptographicHash>

ThemeAppIcon::ThemeAppIcon(QObject *parent) : QObject(parent)
{

}

ThemeAppIcon::~ThemeAppIcon()
{

}

const QPixmap ThemeAppIcon::getIcon(const QString iconName, const int size, const qreal ratio)
{
    QPixmap pixmap;
    QString key;
    QIcon icon;
    // 把size改为小于size的最大偶数 :)
    const int s = int(size * ratio) & ~1;

    do {

        // load pixmap from our Cache
        if (iconName.startsWith("data:image/")) {
            key = QCryptographicHash::hash(iconName.toUtf8(), QCryptographicHash::Md5).toHex();

            // FIXME(hualet): The cache can reduce memory usage,
            // that is ~2M on HiDPI enabled machine with 9 icons loaded,
            // but I don't know why since QIcon has its own cache and all of the
            // icons loaded are loaded by QIcon::fromTheme, really strange here.
            if (QPixmapCache::find(key, &pixmap))
                break;
        }

        // load pixmap from Byte-Data
        if (iconName.startsWith("data:image/"))
        {
            const QStringList strs = iconName.split("base64,");
            if (strs.size() == 2)
                pixmap.loadFromData(QByteArray::fromBase64(strs.at(1).toLatin1()));

            if (!pixmap.isNull())
                break;
        }

        // load pixmap from File
        if (QFile::exists(iconName))
        {
            pixmap = QPixmap(iconName);
            if (!pixmap.isNull())
                break;
        }

        icon = QIcon::fromTheme(iconName);
        if(icon.isNull()){
            icon = QIcon::fromTheme("deepinwine-"+iconName);
        }else{
            icon = QIcon::fromTheme(iconName, QIcon::fromTheme("application-x-desktop"));
        }

        // load pixmap from Icon-Theme
        const int fakeSize = std::max(48, s); // cannot use 16x16, cause 16x16 is label icon
        pixmap = icon.pixmap(QSize(fakeSize, fakeSize));
        if (!pixmap.isNull())
            break;

        // fallback to a Default pixmap
        pixmap = QPixmap(":/icons/resources/application-x-desktop.svg");
        if (!pixmap.isNull())
            break;

        Q_UNREACHABLE();

    } while (false);

    if (!key.isEmpty()) {
        QPixmapCache::insert(key, pixmap);
    }

    if (pixmap.size().width() != s) {
        pixmap = pixmap.scaled(s, s, Qt::KeepAspectRatio, Qt::SmoothTransformation);
    }
    pixmap.setDevicePixelRatio(ratio);

    return pixmap;
}
