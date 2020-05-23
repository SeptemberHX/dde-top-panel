/*
 * Copyright (C) 2011 ~ 2018 Deepin Technology Co., Ltd.
 *
 * Author:     listenerri <listenerri@gmail.com>
 *
 * Maintainer: listenerri <listenerri@gmail.com>
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

#ifndef TRAYPLUGINITEM_H
#define TRAYPLUGINITEM_H

#include "pluginsitem.h"

class TrayPluginItem : public PluginsItem
{
    Q_OBJECT

public:
    TrayPluginItem(PluginsItemInterface* const pluginInter, const QString &itemKey, QWidget *parent = 0);

    inline ItemType itemType() const Q_DECL_OVERRIDE {return ItemType::TrayPlugin;}

    void setSuggestIconSize(QSize size);
    void setRightSplitVisible(const bool visible);
    int trayVisableItemCount();

Q_SIGNALS:
    void trayVisableCountChanged(const int &count) const;

private:
    bool eventFilter(QObject *watched, QEvent *e) Q_DECL_OVERRIDE;

private:
    int m_trayVisableItemCount = 0;
};

#endif // TRAYPLUGINITEM_H
