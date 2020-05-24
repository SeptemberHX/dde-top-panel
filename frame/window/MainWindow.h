//
// Created by septemberhx on 2020/5/23.
//

#ifndef DDE_TOP_PANEL_MAINWINDOW_H
#define DDE_TOP_PANEL_MAINWINDOW_H

#include <DBlurEffectWidget>
#include <DPlatformWindowHandle>
#include "../panel/MainPanelControl.h"
#include "util/TopPanelSettings.h"
#include "xcb/xcb_misc.h"

DWIDGET_USE_NAMESPACE

class MainWindow : public DBlurEffectWidget
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow() override;

signals:
    void panelGeometryChanged();

private:
    void resizeMainPanelWindow();
    void clearStrutPartial();
    void setStrutPartial();
    void initConnections();

private:
    MainPanelControl *m_mainPanel;
    TopPanelSettings *m_settings;
    XcbMisc *m_xcbMisc;
    Position m_curDockPos;
    DPlatformWindowHandle m_platformWindowHandle;
};


#endif //DDE_TOP_PANEL_MAINWINDOW_H
