//
// Created by septemberhx on 2020/5/23.
//

#ifndef DDE_TOP_PANEL_MAINWINDOW_H
#define DDE_TOP_PANEL_MAINWINDOW_H

#include <DBlurEffectWidget>
#include <DPlatformWindowHandle>
#include "../panel/MainPanelControl.h"

DWIDGET_USE_NAMESPACE

class MainWindow : public DBlurEffectWidget
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = 0);

signals:
    void panelGeometryChanged();

private:
    DPlatformWindowHandle m_platformWindowHandle;
    MainPanelControl *m_mainPanel;
};


#endif //DDE_TOP_PANEL_MAINWINDOW_H
