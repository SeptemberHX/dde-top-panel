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
#include "util/CustomSettings.h"
#include "../widgets/QOperationWidget.h"

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
    void applyCustomSettings(const CustomSettings& customSettings);

public slots:
    void insertItem(const int index, DockItem *item);
    void removeItem(DockItem *item);
    void itemUpdated(DockItem *item);

signals:
    void emptyAreaDoubleClicked();
    void itemMoved(DockItem *sourceItem, DockItem *targetItem);
    void itemAdded(const QString &appDesktop, int idx);

private:
    void init();
    void resizeDockIcon();

protected:
    void mouseDoubleClickEvent(QMouseEvent *event) override;
    void dragMoveEvent(QDragMoveEvent *event) override;
    void mousePressEvent(QMouseEvent *event) override;

    void dragLeaveEvent(QDragLeaveEvent *event) override;

    void dragEnterEvent(QDragEnterEvent *event) override;

public:
    bool eventFilter(QObject *watched, QEvent *event) override;

private:
    void startDrag(DockItem *);
    void handleDragMove(QDragMoveEvent *e);
    DockItem *dropTargetItem(DockItem *sourceItem, QPoint point);
    void moveItem(DockItem *sourceItem, DockItem *targetItem);
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
    QPoint m_mousePressPos;

    QOperationWidget *m_buttonWidget;
};


#endif //DDE_TOP_PANEL_MAINPANELCONTROL_H
