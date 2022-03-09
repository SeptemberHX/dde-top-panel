//
// Created by septemberhx on 2020/5/26.
//

#include <iostream>
#include <QImage>
#include <QDebug>
#include <QApplication>
#include <QScreen>
#include "XUtils.h"
#include "xdo.h"
#include <X11/Xlib.h>
#include <X11/Xw32defs.h>
#include <iostream>
#include <X11/Xutil.h>
#include <X11/Xatom.h>
#include <KWindowInfo>
#include <KWindowSystem>
#include "util/CustomSettings.h"

xdo_t *XUtils::m_xdo = nullptr;
Display *XUtils::m_display = nullptr;

void XUtils::openXdo() {
    if (m_xdo == nullptr) {
        m_xdo = xdo_new(NULL);
    }
}

int XUtils::getFocusWindowId() {
    openXdo();

    Window window = 0;
    int ret = xdo_get_active_window(m_xdo, &window);

    if (ret == XDO_SUCCESS) {
        return window;
    } else {
        return KWindowSystem::activeWindow();
    }
//    context_t context;
//    context.xdo = m_xdo;
//
//    return cmd_getActiveWindow(&context);
}

QString XUtils::getWindowNameX11(int winId) {
    int nameLen = -1;
    int nameType;
    unsigned char *windowName;
    int ret = xdo_get_window_name(m_xdo, winId, &windowName, &nameLen, &nameType);
    if (ret != XDO_SUCCESS || nameLen < 0 || nameLen > 256) {
        return "";
    }

    if (windowName == nullptr)
        return "";
    QString result;
    char name[nameLen + 1];
    for (int i = 0; i < nameLen; ++i) {
        name[i] = windowName[i];
    }
    name[nameLen] = '\0';
    XFree(windowName);
    return QString(name);
}

QString XUtils::getWindowName(int winId) {
    KWindowInfo info(winId, NET::WMVisibleName);
    if (info.valid()) {
        return info.visibleName();
    }
    return QString();
}

bool XUtils::checkIfBadWindow(int winId) {
    KWindowInfo info(winId, NET::WMGeometry);
    return !info.valid();
}

void XUtils::openDisplay() {
    if (m_display == nullptr) {
        m_display = XOpenDisplay(NULL);
    }
}

int XUtils::getFocusWindowIdByX() {
    openDisplay();

    Window w;
    int revert_to;
    XGetInputFocus(m_display, &w, &revert_to);
}

bool XUtils::checkIfWinMaximum(int winId) {
    openXdo();

    unsigned char *value;
    long n;

    int ret = xdo_get_window_property(m_xdo, winId, "_NET_WM_STATE", &value, &n, NULL, NULL);
    if (ret != XDO_SUCCESS) {
        return false;
    }

    Atom *result = (Atom*) value;
    Atom hMax = XInternAtom(m_xdo->xdpy, "_NET_WM_STATE_MAXIMIZED_HORZ", False);
    Atom vMax = XInternAtom(m_xdo->xdpy, "_NET_WM_STATE_MAXIMIZED_VERT", False);
    bool hMaxFlag = false;
    bool vMaxFlag = false;
    for (int i = 0; i < n; ++i) {
        if (result[i] == hMax) {
            hMaxFlag = true;
        } else if (result[i] == vMax) {
            vMaxFlag = true;
        }
    }
    XFree(value);
    return hMaxFlag && vMaxFlag;
}

bool XUtils::checkIfWinMinimunX11(int winId) {
    openXdo();

    unsigned char *value;
    long n;

    int ret = xdo_get_window_property(m_xdo, winId, "_NET_WM_STATE", &value, &n, NULL, NULL);
    if (ret != XDO_SUCCESS) {
        return false;
    }

    Atom *result = (Atom*) value;
    Atom minAtom = XInternAtom(m_xdo->xdpy, "_NET_WM_STATE_HIDDEN", False);
    bool maxFlag = false;
    for (int i = 0; i < n; ++i) {
        if (result[i] == minAtom) {
            maxFlag = true;
            break;
        }
    }
    XFree(value);
    return maxFlag;
}

void XUtils::unmaximizeWindow(int winId) {
    openXdo();

    XEvent xev;
    Atom wm_state  =  XInternAtom(m_xdo->xdpy, "_NET_WM_STATE", False);
    Atom max_horz  =  XInternAtom(m_xdo->xdpy, "_NET_WM_STATE_MAXIMIZED_HORZ", False);
    Atom max_vert  =  XInternAtom(m_xdo->xdpy, "_NET_WM_STATE_MAXIMIZED_VERT", False);

    memset(&xev, 0, sizeof(xev));
    xev.type = ClientMessage;
    xev.xclient.window = winId;
    xev.xclient.message_type = wm_state;
    xev.xclient.format = 32;
    xev.xclient.data.l[0] = 0; // _NET_WM_STATE_REMOVE
    xev.xclient.data.l[1] = max_horz;
    xev.xclient.data.l[2] = max_vert;

    XSendEvent(m_xdo->xdpy, DefaultRootWindow(m_xdo->xdpy), True, SubstructureNotifyMask, &xev);
}

QPixmap XUtils::getWindowIconNameX11(int winId) {
    openXdo();

    Atom prop = XInternAtom(m_xdo->xdpy, "_NET_WM_ICON", False);
    Atom actualType;
    unsigned char *data;
    int actualFormat;
    ulong nitem;
    ulong bytes;
    XGetWindowProperty(m_xdo->xdpy, winId, prop, 0, 2, False, XA_CARDINAL, &actualType, &actualFormat, &nitem, &bytes, &data);

    unsigned long *cur_set = (unsigned long *)data;
    if (cur_set == NULL) {  // return default image
        return QPixmap(CustomSettings::instance()->getActiveDefaultAppIconPath());
    }

    int width = cur_set[0];
    int height = cur_set[1];
    XFree(data);

    XGetWindowProperty(m_xdo->xdpy, winId, prop, 2, width * height, False, XA_CARDINAL, &actualType, &actualFormat, &nitem, &bytes, &data);
    unsigned int* imgData = new unsigned int[width * height];
    unsigned long* ul = (unsigned long*) data;
    for (int i=0; i < nitem; ++i)
    {
        imgData[i] = (unsigned int)ul[i];
    }
    XFree(data);

    QImage image(width, height, QImage::Format_ARGB32);
    unsigned char* argb = (unsigned char*)imgData;
    for(int y = 0; y < height; y++)
    {
        for(int x = 0; x < width; x++)
        {
            unsigned char a = argb[3];
            unsigned char r = argb[2] * a / 255;
            unsigned char g = argb[1] * a / 255;
            unsigned char b = argb[0] * a / 255;
            QRgb value = qRgba(r, g, b, a);
            image.setPixel(x, y, value);
            argb += 4;
        }
    }

    delete[] imgData;
    return QPixmap::fromImage(image);
}

// Abandoned because it always returns an icon even there is no icon for current window: it provides default icon.
QPixmap XUtils::getWindowIconName(int winId) {
    QPixmap iconPixmap = KWindowSystem::self()->icon(winId);
    if (iconPixmap.isNull()) {
        return QPixmap(CustomSettings::instance()->getActiveDefaultAppIconPath());
    }
    return iconPixmap;
}


int XUtils::getWindowScreenNum(int winId) {
    KWindowInfo info = KWindowSystem::self()->windowInfo(winId, NET::WMGeometry);
    if (info.valid()) {
        QRect r = info.geometry();
        int maxPartScreenNum = 0;
        int maxPartArea = 0;
        int i = 0;
        for (const QScreen* screen : QApplication::screens()) {
            QRect intR = screen->geometry().intersected(r);
            auto area = intR.height() * intR.width();
            if (area > maxPartArea) {
                maxPartScreenNum = i;
                maxPartArea = area;
            }
            ++i;
        }
        return maxPartScreenNum;
    }

    return -1;
}

QRect XUtils::getWindowGeometry(int winId) {
    openXdo();

    QRect r;
    XWindowAttributes attr;
    int ret = XGetWindowAttributes(m_xdo->xdpy, winId, &attr);
    if (ret != 0) {
        r.setLeft(attr.x);
        r.setTop(attr.y);
        r.setWidth(attr.width);
        r.setHeight(attr.height);
        qDebug() << attr.x << attr.y << attr.width << attr.height;
    }

    return r;
}

bool XUtils::checkIfWinMinimun(int winId) {
    KWindowInfo info(winId, NET::WMState);
    if (info.valid()) {
        return info.isMinimized();
    }
    return false;
}

QString XUtils::getWindowAppName(int winId) {
    KWindowInfo info(winId, NET::WMName);
    if (info.valid()) {
        qDebug() << info.name() << info.desktopFileName() << info.windowClassName() << info.visibleIconName() << info.iconName() << info.visibleName();
        QString name = info.name();
        if (name.contains(QRegExp("[–—-]"))) {
            QString tmpName = name.split(QRegExp("[–—-]")).last();
            return tmpName.trimmed();
        } else {
            return name;
        }
    }
    return "";
}

//int XUtils::cmd_getActiveWindow(context_t *context) {
//    Window window = 0;
//
//    xdo_get_active_window(context->xdo, &window);
//    printf("%ld\n", window);
//    return window;
//}
//
//
