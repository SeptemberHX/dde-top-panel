/*
 * Copyright (C) 2016 ~ 2018 Deepin Technology Co., Ltd.
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

#ifndef DBUSDOCKADAPTORS_H
#define DBUSDOCKADAPTORS_H

#include <QtDBus/QtDBus>
#include "window/MainWindow.h"
class MainWindow;
/*
 * Adaptor class for interface com.deepin.dde.Dock
 */

class DBusDockAdaptors: public QDBusAbstractAdaptor
{
    Q_OBJECT
    Q_CLASSINFO("D-Bus Interface", "com.deepin.dde.Dock")
    Q_CLASSINFO("D-Bus Introspection", ""
                                       "  <interface name=\"com.deepin.dde.Dock\">\n"
                                       "    <property access=\"read\" type=\"(iiii)\" name=\"geometry\"/>\n"
                                       "    <signal name=\"geometryChanged\">"
                                                "<arg name=\"geometry\" type=\"(iiii)\"/>"
                                            "</signal>"
                                       "  </interface>\n"
                                       "")

public:
    DBusDockAdaptors(MainWindow *parent);
    virtual ~DBusDockAdaptors();

    MainWindow *parent() const;

public: // PROPERTIES
    Q_PROPERTY(QRect geometry READ geometry NOTIFY geometryChanged)
    QRect geometry() const;

signals:
    void geometryChanged(QRect geometry);
};

#endif //DBUSDOCKADAPTORS
