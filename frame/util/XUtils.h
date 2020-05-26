//
// Created by septemberhx on 2020/5/26.
//

#ifndef DDE_TOP_PANEL_XUTILS_H
#define DDE_TOP_PANEL_XUTILS_H

#include <QString>
extern "C" {
    #include <xdo.h>
}

// something strange happens...

//struct context_t {
//    xdo_t *xdo;
//};


class XUtils {

public:
    static int getFocusWindowId();
    static QString getWindowName(int winId);
    static bool checkIfWinMaximum(int winId);
    static void unmaximizeWindow(int winId);
//    static int cmd_getActiveWindow(context_t *context);
    static xdo_t *m_xdo;

    static int getFocusWindowIdByX();
    static Display *m_display;
private:
    static void openXdo();
    static void openDisplay();
};


#endif //DDE_TOP_PANEL_XUTILS_H
