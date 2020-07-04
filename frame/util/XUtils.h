//
// Created by septemberhx on 2020/5/26.
//

#ifndef DDE_TOP_PANEL_XUTILS_H
#define DDE_TOP_PANEL_XUTILS_H

#include <QPixmap>
#include <QString>
extern "C" {
    #include <xdo.h>
}

// something strange happens...

//struct context_t {
//    xdo_t *xdo;
//};


/**
 * This class is implemented before I realized that deepin depends on kwin now.
 *   So they can be replaced by the KWindowSystem::XXX
 *   However, those functions work well, so I do not plan to change them unless bugs happen
 */
class XUtils {

public:
    static int getFocusWindowId();

    static QString getWindowNameX11(int winId);
    static QString getWindowName(int winId);


    static bool checkIfWinMaximum(int winId);
    static bool checkIfWinMinimunX11(int winId);

    static bool checkIfWinMinimun(int winId);

    static void unmaximizeWindow(int winId);

    static QPixmap getWindowIconNameX11(int winId);
    static QPixmap getWindowIconName(int winId);

    static QString getWindowAppName(int winId);

    static bool checkIfBadWindow(int winid);
    static int getWindowScreenNum(int windid);
    static QRect getWindowGeometry(int winId);
//    static int cmd_getActiveWindow(context_t *context);
    static xdo_t *m_xdo;

    static int getFocusWindowIdByX();
    static Display *m_display;
private:
    static void openXdo();
    static void openDisplay();
};


#endif //DDE_TOP_PANEL_XUTILS_H
