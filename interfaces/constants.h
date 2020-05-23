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

#ifndef CONSTANTS_H
#define CONSTANTS_H

#include <QtCore>

namespace Dock {

#define DOCK_PLUGIN_MIME    "dock/plugin"
#define DOCK_PLUGIN_API_VERSION    "1.2.2"

#define PROP_DISPLAY_MODE   "DisplayMode"

#define PLUGIN_BACKGROUND_MAX_SIZE 40
#define PLUGIN_BACKGROUND_MIN_SIZE 20

#define PLUGIN_ICON_MAX_SIZE 20

// 需求变更成插件图标始终保持20x20,但16x16的资源还在。所以暂时保留此宏
#define PLUGIN_ICON_MIN_SIZE 20

// 插件最小尺寸，图标采用深色
#define PLUGIN_MIN_ICON_NAME "-dark"

// dock最大尺寸
#define DOCK_MAX_SIZE 100
///
/// \brief The DisplayMode enum
/// spec dock display mode
///
enum DisplayMode {
    Fashion     = 0,
    Efficient   = 1,
    // deprecreated
//    Classic     = 2,
};

#define PROP_HIDE_MODE      "HideMode"
///
/// \brief The HideMode enum
/// spec dock hide behavior
///
enum HideMode {
    KeepShowing     = 0,
    KeepHidden      = 1,
    SmartHide       = 3,
};

#define PROP_POSITION       "Position"
///
/// \brief The Position enum
/// spec dock position, dock always placed at primary screen,
/// so all position is the primary screen edge.
///
enum Position {
    Top         = 0,
    Right       = 1,
    Bottom      = 2,
    Left        = 3,
};

#define PROP_HIDE_STATE     "HideState"
///
/// \brief The HideState enum
/// spec current dock should hide or shown.
/// this argument works only HideMode is SmartHide
///
enum HideState {
    Unknown     = 0,
    Show        = 1,
    Hide        = 2,
};

}

Q_DECLARE_METATYPE(Dock::DisplayMode)
Q_DECLARE_METATYPE(Dock::Position)

#endif // CONSTANTS_H
