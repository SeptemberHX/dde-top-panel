//
// Created by septemberhx on 2020/5/26.
//

#include "XUtils.h"
#include "xdo.h"
#include <X11/Xlib.h>
#include <X11/Xw32defs.h>
#include <iostream>

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

    QString result;
    char name[nameLen + 1];
    for (int i = 0; i < nameLen; ++i) {
        name[i] = windowName[i];
    }
    name[nameLen] = '\0';
    free(windowName);
    return QString(name);
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
    std::cout << "==================> X: " << w << std::endl;
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
    free(value);
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

//int XUtils::cmd_getActiveWindow(context_t *context) {
//    Window window = 0;
//
//    xdo_get_active_window(context->xdo, &window);
//    printf("%ld\n", window);
//    return window;
//}
//
//
