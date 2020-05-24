//
// Created by septemberhx on 2020/5/23.
//

#include "MainPanelControl.h"

MainPanelControl::MainPanelControl(QWidget *parent)
    : QWidget(parent)
    , m_mainPanelLayout(new QBoxLayout(QBoxLayout::LeftToRight, this))
    , m_trayAreaWidget(new QWidget(this))
    , m_trayAreaLayout(new QBoxLayout(QBoxLayout::LeftToRight))
    , m_pluginAreaWidget(new QWidget(this))
    , m_pluginLayout(new QBoxLayout(QBoxLayout::LeftToRight))
    , m_position(Position::Top)
{
    this->init();
}

void MainPanelControl::init() {
    this->m_mainPanelLayout->addStretch();
    this->m_mainPanelLayout->addWidget(this->m_trayAreaWidget);
    this->m_mainPanelLayout->addWidget(this->m_pluginAreaWidget);

    m_mainPanelLayout->setMargin(0);
    m_mainPanelLayout->setContentsMargins(0, 0, 0, 0);
    m_mainPanelLayout->setSpacing(0);

    // 托盘
    m_trayAreaWidget->setLayout(m_trayAreaLayout);
    m_trayAreaWidget->setAccessibleName("trayarea");
    m_trayAreaLayout->setMargin(0);
    m_trayAreaLayout->setSpacing(0);
    m_trayAreaLayout->setContentsMargins(0, 10, 0, 10);

    // 插件
    m_pluginAreaWidget->setLayout(m_pluginLayout);
    m_pluginAreaWidget->setAccessibleName("pluginarea");
    m_pluginLayout->setMargin(0);
    m_pluginLayout->setSpacing(0);
    m_pluginLayout->setContentsMargins(10, 0, 10, 0);

    m_pluginAreaWidget->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Expanding);
    m_trayAreaWidget->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Expanding);
}

// dockitemmanager is responsible for loading all the necessary items on the dock
// I don't want to change the code in dockitemmanager, so I abandon some items that I don't want
void MainPanelControl::insertItem(int index, DockItem *item)
{
    item->installEventFilter(this);
    qDebug() << item->itemType();

    switch (item->itemType()) {
        case DockItem::Launcher:
        case DockItem::FixedPlugin:
        case DockItem::App:
        case DockItem::Placeholder:
            qDebug() << "Abandon the plugin" << item->objectName() << "due to the unnecessary plugin type";
            break;
        case DockItem::TrayPlugin:
            qDebug() << "Add tray plugin item";
            addTrayAreaItem(index, item);
            break;
        case DockItem::Plugins:
            // todo
            addPluginAreaItem(index, item);
            break;
        default:
            break;
    }
}

void MainPanelControl::removeItem(DockItem *item)
{
    switch (item->itemType()) {
        case DockItem::Launcher:
        case DockItem::FixedPlugin:
        case DockItem::App:
        case DockItem::Placeholder:
            break;
        case DockItem::TrayPlugin:
            removeTrayAreaItem(item);
            break;
        case DockItem::Plugins:
            removePluginAreaItem(item);
            break;
        default:
            break;
    }
}

void MainPanelControl::removePluginAreaItem(QWidget *wdg)
{
    m_pluginLayout->removeWidget(wdg);
}

void MainPanelControl::itemUpdated(DockItem *item)
{
    item->parentWidget()->adjustSize();
    resizeDockIcon();
}

void MainPanelControl::resizeDockIcon()
{
    if (!m_tray)
        return;
    // 插件有点特殊，因为会引入第三方的插件，并不会受dock的缩放影响，我们只能限制我们自己的插件，否则会导致显示错误。
    // 以下是受控制的插件
    PluginsItem *trashPlugin = nullptr;
    PluginsItem *shutdownPlugin = nullptr;
    PluginsItem *keyboardPlugin = nullptr;
    for (int i = 0; i < m_pluginLayout->count(); ++ i) {
        PluginsItem *w = static_cast<PluginsItem *>(m_pluginLayout->itemAt(i)->widget());
        if (w->pluginName() == "trash") {
            trashPlugin = w;
        } else if (w->pluginName() == "shutdown") {
            shutdownPlugin = w;
        } else if (w->pluginName() == "onboard") {
            keyboardPlugin = w;
        }
    }

    // 总宽度
    int totalLength = ((m_position == Position::Top) || (m_position == Position::Bottom)) ? width() : height();
    // 减去托盘间隔区域
    totalLength -= (m_tray->trayVisableItemCount() + 1) * 10;
    // 减去插件间隔
    totalLength -= (m_pluginLayout->count() + 1) * 10;
    // 减去3个分割线的宽度
//    totalLength -= 3 * SPLITER_SIZE;

    // 减去所有插件宽度，加上参与计算的3个插件宽度
    if ((m_position == Position::Top) || (m_position == Position::Bottom)) {
        totalLength -= m_pluginAreaWidget->width();
        if (trashPlugin) totalLength += trashPlugin->width();
        if (shutdownPlugin) totalLength += shutdownPlugin->width();
        if (keyboardPlugin) totalLength += keyboardPlugin->width();
    } else {
        totalLength -= m_pluginAreaWidget->height();
        if (trashPlugin) totalLength += trashPlugin->height();
        if (shutdownPlugin) totalLength += shutdownPlugin->height();
        if (keyboardPlugin) totalLength += keyboardPlugin->height();
    }

    if (totalLength < 0)
        return;

    // 参与计算的插件的个数（包含托盘和插件，垃圾桶，关机，屏幕键盘）
    int pluginCount = m_tray->trayVisableItemCount() + (trashPlugin ? 1 : 0) + (shutdownPlugin ? 1 : 0) + (keyboardPlugin ? 1 : 0);

    // icon个数
    int iconCount = pluginCount;

    int iconSize = 0;

    // 余数
    int yu = (totalLength % iconCount);
    // icon宽度 = (总宽度-余数)/icon个数
    iconSize = (totalLength - yu) / iconCount;

    if (iconSize < 20 || iconSize > 40) {

        // 减去插件和托盘的宽度
        if (iconSize < 20)
            totalLength -= 20 * pluginCount;
        else
            totalLength -= 40 * pluginCount;

        iconCount -= pluginCount;

        // 余数
//        int yu = (totalLength % iconCount);
        // icon宽度 = (总宽度-余数)/icon个数
//        iconSize = (totalLength - yu) / iconCount;
        iconSize = 0;
    }

    // only considering top mode
    if (iconSize >= height()) {
        calcuDockIconSize(height(), height(), trashPlugin, shutdownPlugin, keyboardPlugin);
    } else {
        calcuDockIconSize(iconSize, height(), trashPlugin, shutdownPlugin, keyboardPlugin);
    }
}


void MainPanelControl::calcuDockIconSize(int w, int h, PluginsItem *trashPlugin, PluginsItem *shutdownPlugin, PluginsItem *keyboardPlugin)
{
    if (m_position == Dock::Position::Top || m_position == Dock::Position::Bottom) {
//        m_traySpliter->setFixedSize(SPLITER_SIZE, int(w * 0.5));
        // 垃圾桶
        if (trashPlugin)
            trashPlugin->setFixedSize(std::min(w, h - 20), h - 20);
    }

    // 插件和托盘

    // 托盘上每个图标大小
    int tray_item_size = 20;

    if ((m_position == Position::Top) || (m_position == Position::Bottom)) {
        w = qBound(20, w, 40);
        tray_item_size = std::min(w, h - 20);
    } else {
        h = qBound(20, h, 40);
        tray_item_size = std::min(w - 20, h);
    }

    if (tray_item_size < 20)
        return;

    if ((m_position == Position::Top) || (m_position == Position::Bottom)) {
        m_tray->centralWidget()->setProperty("iconSize", tray_item_size);

        // 插件
        if (shutdownPlugin)
            shutdownPlugin->setFixedSize(tray_item_size, h - 20);
        if (keyboardPlugin)
            keyboardPlugin->setFixedSize(tray_item_size, h - 20);

    } else {
        m_tray->centralWidget()->setProperty("iconSize", tray_item_size);

        if (shutdownPlugin)
            shutdownPlugin->setFixedSize(w - 20, tray_item_size);
        if (keyboardPlugin)
            keyboardPlugin->setFixedSize(w - 20, tray_item_size);
    }

    if ((m_position == Position::Top) || (m_position == Position::Bottom)) {
        // 三方插件
        for (int i = 0; i < m_pluginLayout->count(); ++ i) {
            PluginsItem *pItem = static_cast<PluginsItem *>(m_pluginLayout->itemAt(i)->widget());
            if (pItem != trashPlugin && pItem != shutdownPlugin && pItem != keyboardPlugin) {
                if (pItem->pluginName() == "datetime"){
                    pItem->setFixedSize(pItem->sizeHint().width(), h);
                } else if (pItem->pluginName() == "AiAssistant"){
                    pItem->setFixedSize(tray_item_size, h - 20);
                } else {
                    pItem->setFixedSize(pItem->sizeHint().width(), h);
                }
            }
        }
    } else {
        // 三方插件
        for (int i = 0; i < m_pluginLayout->count(); ++ i) {
            PluginsItem *pItem = static_cast<PluginsItem *>(m_pluginLayout->itemAt(i)->widget());
            if (pItem != trashPlugin && pItem != shutdownPlugin && pItem != keyboardPlugin) {
                if (pItem->pluginName() == "datetime"){
                    pItem->setFixedSize(w, pItem->sizeHint().height());
                } else if (pItem->pluginName() == "AiAssistant"){
                    pItem->setFixedSize(w - 20, tray_item_size);
                } else {
                    pItem->setFixedSize(w, pItem->sizeHint().height());
                }
            }
        }
    }
}


// ----------> For tray area begin <----------

void MainPanelControl::addTrayAreaItem(int index, QWidget *wdg)
{
    m_tray = static_cast<TrayPluginItem *>(wdg);
    m_trayAreaLayout->insertWidget(index, wdg);
    resizeDockIcon();
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

    resizeDockIcon();

    // 模式切换时，托盘区域宽度错误，对应任务11933
    m_trayAreaWidget->adjustSize();
}

void MainPanelControl::setDisplayMode(DisplayMode mode) {
    qDebug() << "Not allowed to change display mode";
}

void MainPanelControl::addPluginAreaItem(int index, QWidget *wdg) {
    m_pluginLayout->insertWidget(index, wdg, 0, Qt::AlignCenter);
    qDebug() << "Plugin widget size: " << wdg->size();
    resizeDockIcon();
    m_pluginAreaWidget->adjustSize();
}

// ----------> For tray area end <----------