//
// Created by septemberhx on 2020/5/23.
//

#include "MainPanelControl.h"

MainPanelControl::MainPanelControl(QWidget *parent)
    : QWidget(parent)
    , m_mainPanelLayout(new QBoxLayout(QBoxLayout::LeftToRight, this))
    , m_trayAreaWidget(new QWidget(this))
    , m_trayAreaLayout(new QBoxLayout(QBoxLayout::LeftToRight))
{
    this->init();
}

void MainPanelControl::init() {
    this->m_mainPanelLayout->addWidget(this->m_trayAreaWidget);

    m_mainPanelLayout->setMargin(0);
    m_mainPanelLayout->setContentsMargins(0, 0, 0, 0);
    m_mainPanelLayout->setSpacing(0);

    // 托盘
    m_trayAreaWidget->setLayout(m_trayAreaLayout);
    m_trayAreaWidget->setAccessibleName("trayarea");
    m_trayAreaLayout->setMargin(0);
    m_trayAreaLayout->setContentsMargins(0, 10, 0, 10);
    m_trayAreaLayout->setSpacing(0);
    m_trayAreaWidget->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Expanding);
    m_trayAreaLayout->setContentsMargins(0, 10, 0, 10);
}

// dockitemmanager is responsible for loading all the necessary items on the dock
// I don't want to change the code in dockitemmanager, so I abandon some items that I don't want
void MainPanelControl::insertItem(int index, DockItem *item)
{
    item->installEventFilter(this);

    switch (item->itemType()) {
        case DockItem::Launcher:
        case DockItem::FixedPlugin:
        case DockItem::App:
        case DockItem::Placeholder:
            qDebug() << "Abandon the plugin" << item->objectName() << "due to the unnecessary plugin type";
            break;
        case DockItem::TrayPlugin:
            addTrayAreaItem(index, item);
            break;
        case DockItem::Plugins:
            // todo
//            addPluginAreaItem(index, item);
            break;
        default:
            break;
    }
}

// ----------> For tray area begin <----------

void MainPanelControl::addTrayAreaItem(int index, QWidget *wdg)
{
    m_tray = static_cast<TrayPluginItem *>(wdg);
    m_trayAreaLayout->insertWidget(index, wdg);
//    resizeDockIcon();
}

void MainPanelControl::removeTrayAreaItem(QWidget *wdg)
{
    m_trayAreaLayout->removeWidget(wdg);
}

void MainPanelControl::getTrayVisableItemCount()
{
    if (m_trayAreaLayout->count() > 0) {
        TrayPluginItem *w = static_cast<TrayPluginItem *>(m_trayAreaLayout->itemAt(0)->widget());
        m_trayIconCount = w->trayVisableItemCount();
    } else {
        m_trayIconCount = 0;
    }

//    resizeDockIcon();

    // 模式切换时，托盘区域宽度错误，对应任务11933
    m_trayAreaWidget->adjustSize();
}

// ----------> For tray area end <----------