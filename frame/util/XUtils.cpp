//
// Created by septemberhx on 2020/5/26.
//

#include <QImage>
#include "XUtils.h"
#include "xdo.h"
#include <X11/Xlib.h>
#include <X11/Xw32defs.h>
#include <iostream>
#include <X11/Xutil.h>
#include <X11/Xatom.h>
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
        return -1;
    }
//    context_t context;
//    context.xdo = m_xdo;
//
//    return cmd_getActiveWindow(&context);
}

QString XUtils::getWindowName(int winId) {
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

bool XUtils::checkIfBadWindow(int winId) {
    openXdo();

    Atom atom = XInternAtom(m_xdo->xdpy, "_NET_WM_NAME", false);
    Atom actualType;
    unsigned long ntimes;
    unsigned long bytes_after;
    int actualFormat;
    unsigned char *prop;

    int status = XGetWindowProperty(m_xdo->xdpy, winId, atom, 0, (~0L), False, AnyPropertyType, &actualType, &actualFormat, &ntimes, &bytes_after, &prop);
    XFree(prop);

    return status == BadWindow;
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

QPixmap XUtils::getWindowIconName(int winId) {
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

//int XUtils::cmd_getActiveWindow(context_t *context) {
//    Window window = 0;
//
//    xdo_get_active_window(context->xdo, &window);
//    printf("%ld\n", window);
//    return window;
//}
//
//
