//
// Created by septemberhx on 2020/5/23.
//

#ifndef DDE_TOP_PANEL_TOPPANELSETTINGS_H
#define DDE_TOP_PANEL_TOPPANELSETTINGS_H

#include "constants.h"
#include <QObject>
#include <QWidget>
#include "dbus/dbusdisplay.h"

using namespace Dock;

class TopPanelSettings : public QObject
{
    Q_OBJECT

public:
    inline DisplayMode displayMode() const { return m_displayMode; }
    inline Position position() const { return m_position; }
    inline int screenRawHeight() const { return m_screenRawHeight; }
    inline int screenRawWidth() const { return m_screenRawWidth; }
    inline const QRect primaryRawRect() const { return m_primaryRawRect; }
    inline const QSize windowSize() const { return m_mainWindowSize; }

    const QRect windowRect(const Position position, const bool hide) const;

    const int dockMargin() const;

    static TopPanelSettings &Instance();
    const QRect primaryRect() const;
    QSize m_mainWindowSize;

private:
    void calculateWindowConfig();

    explicit TopPanelSettings(QWidget *parent = 0);
    TopPanelSettings(TopPanelSettings const &) = delete;
    TopPanelSettings operator =(TopPanelSettings const &) = delete;

    void resetFrontendGeometry();
    qreal dockRatio() const;

private:
    int m_dockWindowSize;
    Position m_position;

    DisplayMode m_displayMode;
    DBusDisplay *m_displayInter;
    QRect m_primaryRawRect;
    QRect m_frontendRect;
    int m_screenRawHeight;
    int m_screenRawWidth;
};


#endif //DDE_TOP_PANEL_TOPPANELSETTINGS_H
