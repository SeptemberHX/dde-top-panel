//
// Created by septemberhx on 2020/5/26.
//

#include "XUtils.h"
#include "xdo.h"
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
    int nameLen;
    int nameType;
    unsigned char *windowName;
    xdo_get_window_name(m_xdo, winId, &windowName, &nameLen, &nameType);
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

//int XUtils::cmd_getActiveWindow(context_t *context) {
//    Window window = 0;
//
//    xdo_get_active_window(context->xdo, &window);
//    printf("%ld\n", window);
//    return window;
//}
//
//
