/*
 * Copyright (C) 2015 ~ 2018 Deepin Technology Co., Ltd.
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

#include <QDebug>
#include <QX11Info>
#include <QApplication>

#include <xcb/xcb.h>
#include <xcb/xcb_ewmh.h>

#include "xcb_misc.h"

static XcbMisc * _xcb_misc_instance = NULL;

XcbMisc::XcbMisc()
{
    xcb_intern_atom_cookie_t * cookie = xcb_ewmh_init_atoms(QX11Info::connection(), &m_ewmh_connection);
    xcb_ewmh_init_atoms_replies(&m_ewmh_connection, cookie, NULL);
}

XcbMisc::~XcbMisc()
{

}

XcbMisc * XcbMisc::instance()
{
    if (_xcb_misc_instance == NULL) {
        _xcb_misc_instance = new XcbMisc;
    }

    return _xcb_misc_instance;
}

void XcbMisc::set_window_type(xcb_window_t winId, WindowType winType)
{
    xcb_atom_t atoms[1];

    switch (winType) {
    case WindowType::Desktop:
        atoms[0] = m_ewmh_connection._NET_WM_WINDOW_TYPE_DESKTOP;
        break;
    case WindowType::Dock:
        atoms[0] = m_ewmh_connection._NET_WM_WINDOW_TYPE_DOCK;
        break;
    default:
        break;
    }

    xcb_ewmh_set_wm_window_type(&m_ewmh_connection, winId, 1, atoms);
}

void XcbMisc::clear_strut_partial(xcb_window_t winId)
{
    xcb_ewmh_wm_strut_partial_t strutPartial;
    memset(&strutPartial, 0, sizeof(xcb_ewmh_wm_strut_partial_t));

    xcb_ewmh_set_wm_strut_partial(&m_ewmh_connection, winId, strutPartial);
}

void XcbMisc::set_strut_partial(xcb_window_t winId, Orientation orientation, uint strut, uint start, uint end)
{
    xcb_ewmh_wm_strut_partial_t strut_partial;
    memset(&strut_partial, 0, sizeof(xcb_ewmh_wm_strut_partial_t));

    switch (orientation) {
    case OrientationLeft:
        strut_partial.left = strut;
        strut_partial.left_start_y = start;
        strut_partial.left_end_y = end;
        break;
    case OrientationRight:
        strut_partial.right = strut;
        strut_partial.right_start_y = start;
        strut_partial.right_end_y = end;
        break;
    case OrientationTop:
        strut_partial.top = strut;
        strut_partial.top_start_x = start;
        strut_partial.top_end_x = end;
        break;
    case OrientationBottom:
        strut_partial.bottom = strut;
        strut_partial.bottom_start_x = start;
        strut_partial.bottom_end_x = end;
        break;
    default:
        break;
    }

//    qDebug() << "xcb_ewmh_set_wm_strut_partial" << endl
//             << "top" << strut_partial.top << strut_partial.top_start_x << strut_partial.top_end_x << endl
//             << "left" << strut_partial.left << strut_partial.left_start_y << strut_partial.left_end_y << endl
//             << "right" << strut_partial.right << strut_partial.right_start_y << strut_partial.right_end_y << endl
//             << "bottom" << strut_partial.bottom << strut_partial.bottom_start_x << strut_partial.bottom_end_x << endl;

    xcb_ewmh_set_wm_strut_partial(&m_ewmh_connection, winId, strut_partial);
}

void XcbMisc::set_window_icon_geometry(xcb_window_t winId, QRect geo)
{
//    qDebug() << Q_FUNC_INFO << winId << geo;

    const auto ratio = qApp->devicePixelRatio();

    xcb_ewmh_set_wm_icon_geometry(&m_ewmh_connection, winId, geo.x() * ratio, geo.y() * ratio, geo.width() * ratio, geo.height() * ratio);
}
