//
// Created by septemberhx on 2020/5/23.
//

#ifndef DDE_TOP_PANEL_MAINPANELCONTROL_H
#define DDE_TOP_PANEL_MAINPANELCONTROL_H

#include <QWidget>
#include <QBoxLayout>
#include "item/traypluginitem.h"
#include <com_deepin_dde_daemon_dock_entry.h>
#include <controller/dockitemmanager.h>
#include "../widgets/ActiveWindowControlWidget.h"

using DockEntryInter = com::deepin::dde::daemon::dock::Entry;

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
    void removePluginAreaItem(QWidget *wdg);
    void addPluginAreaItem(int index, QWidget *wdg);

    void setDisplayMode(DisplayMode mode);

public slots:
    void insertItem(const int index, DockItem *item);
    void removeItem(DockItem *item);
    void itemUpdated(DockItem *item);

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
    DockItemManager *m_itemManager;
    Position m_position;

    int m_trayIconCount;

    TrayPluginItem *m_tray = nullptr;
    ActiveWindowControlWidget *activeWindowControlWidget;
};


#endif //DDE_TOP_PANEL_MAINPANELCONTROL_H
