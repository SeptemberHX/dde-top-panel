//
// Created by septemberhx on 2020/5/23.
//

#ifndef DDE_TOP_PANEL_MAINPANELCONTROL_H
#define DDE_TOP_PANEL_MAINPANELCONTROL_H


#include <QWidget>
#include <QBoxLayout>
#include "item/traypluginitem.h"

class MainPanelControl : public QWidget
{
    Q_OBJECT

public:
    explicit MainPanelControl(QWidget *parent = 0);

    // Tray area
    void addTrayAreaItem(int index, QWidget *wdg);
    void removeTrayAreaItem(QWidget *wdg);
    void getTrayVisableItemCount();

public slots:
    void insertItem(const int index, DockItem *item);

private:
    void init();

private:
    QBoxLayout *m_mainPanelLayout;
    QWidget *m_trayAreaWidget;
    QBoxLayout *m_trayAreaLayout;

    int m_trayIconCount;
    TrayPluginItem *m_tray = nullptr;
};


#endif //DDE_TOP_PANEL_MAINPANELCONTROL_H
