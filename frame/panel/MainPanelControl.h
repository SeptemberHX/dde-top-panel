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
    void addFixedAreaItem(int index, QWidget *wdg);

    void addPluginAreaItem(int index, QWidget *wdg);

    void setDisplayMode(DisplayMode mode);

public slots:
    void insertItem(const int index, DockItem *item);

private:
    void init();
    void resizeDockIcon();
    void calcuDockIconSize(const int w, const int h, PluginsItem *trashPlugin, PluginsItem *shutdownPlugin, PluginsItem *keyboardPlugin);

private:
    QBoxLayout *m_mainPanelLayout;
    QWidget *m_trayAreaWidget;
    QBoxLayout *m_trayAreaLayout;
    QWidget *m_pluginAreaWidget;
    QBoxLayout *m_pluginLayout;
    Position m_position;

    int m_trayIconCount;
    TrayPluginItem *m_tray = nullptr;

    QLabel *m_label;
};


#endif //DDE_TOP_PANEL_MAINPANELCONTROL_H
