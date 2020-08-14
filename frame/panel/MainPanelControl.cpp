//
// Created by septemberhx on 2020/5/23.
//

#include "MainPanelControl.h"
#include <QApplication>
#include <QDrag>

MainPanelControl::MainPanelControl(QWidget *parent)
    : QWidget(parent)
    , m_mainPanelLayout(new QBoxLayout(QBoxLayout::LeftToRight, this))
    , m_trayAreaWidget(new QWidget(this))
    , m_trayAreaLayout(new QBoxLayout(QBoxLayout::LeftToRight))
    , m_pluginAreaWidget(new QWidget(this))
    , m_pluginLayout(new QBoxLayout(QBoxLayout::LeftToRight))
    , m_itemManager(DockItemManager::instance(this))
    , m_position(Position::Top)
    , activeWindowControlWidget(new ActiveWindowControlWidget(this))
    , m_buttonWidget(new QOperationWidget(false, this))
{
    this->init();

    this->setFixedHeight(CustomSettings::instance()->getPanelHeight());
    this->setMouseTracking(true);
    this->setAcceptDrops(true);

    QPalette palette = this->palette();
    palette.setColor(QPalette::Background, Qt::transparent);
    this->setPalette(palette);

    this->m_buttonWidget->hide();
}

void MainPanelControl::init() {
    this->m_mainPanelLayout->addWidget(this->activeWindowControlWidget);
    this->m_mainPanelLayout->addWidget(this->m_trayAreaWidget);
    this->m_mainPanelLayout->addWidget(this->m_pluginAreaWidget);
    this->m_mainPanelLayout->addWidget(this->m_buttonWidget);

    connect(this->activeWindowControlWidget, &ActiveWindowControlWidget::showOperationButtons, this, [this] {
        if (!CustomSettings::instance()->isButtonOnLeft()) {
            this->m_buttonWidget->showWithAnimation();
        }
    });
    connect(this->activeWindowControlWidget, &ActiveWindowControlWidget::hideOperationButtons, this, [this] {
        if (!CustomSettings::instance()->isButtonOnLeft()) {
            this->m_buttonWidget->hideWithAnimation();
        }
    });
    connect(this->m_buttonWidget, &QOperationWidget::minButtonClicked, this->activeWindowControlWidget, &ActiveWindowControlWidget::minButtonClicked);
    connect(this->m_buttonWidget, &QOperationWidget::maxButtonClicked, this->activeWindowControlWidget, &ActiveWindowControlWidget::maxButtonClicked);
    connect(this->m_buttonWidget, &QOperationWidget::closeButtonClicked, this->activeWindowControlWidget, &ActiveWindowControlWidget::closeButtonClicked);

    m_mainPanelLayout->setMargin(0);
    m_mainPanelLayout->setContentsMargins(0, 0, 5, 0);
    m_mainPanelLayout->setSpacing(0);

    // 托盘
    m_trayAreaWidget->setLayout(m_trayAreaLayout);
    m_trayAreaWidget->setAccessibleName("trayarea");
    m_trayAreaLayout->setMargin(0);
    m_trayAreaLayout->setSpacing(0);
    m_trayAreaLayout->setContentsMargins(0, 2, 0, 2);

    // 插件
    m_pluginAreaWidget->setLayout(m_pluginLayout);
    m_pluginAreaWidget->setAcceptDrops(true);
    m_pluginAreaWidget->setAccessibleName("pluginarea");
    m_pluginLayout->setMargin(0);
    m_pluginLayout->setSpacing(8);
    m_pluginLayout->setContentsMargins(10, 0, 10, 0);

    m_pluginAreaWidget->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Expanding);
    m_trayAreaWidget->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Expanding);

//    this->m_xdo = xdo_new(nullptr);
    connect(KWindowSystem::self(), &KWindowSystem::activeWindowChanged, this->activeWindowControlWidget, &ActiveWindowControlWidget::activeWindowInfoChanged);
    connect(this, &MainPanelControl::emptyAreaDoubleClicked, this->activeWindowControlWidget, &ActiveWindowControlWidget::maximizeWindow);
    this->activeWindowControlWidget->activeWindowInfoChanged();
}

// dockitemmanager is responsible for loading all the necessary items on the dock
// I don't want to change the code in dockitemmanager, so I abandon some items that I don't want
void MainPanelControl::insertItem(int index, DockItem *item)
{
    item->installEventFilter(this);
//    qDebug() << item->itemType();

    switch (item->itemType()) {
        case DockItem::Launcher:
        case DockItem::FixedPlugin:
        case DockItem::App:
        case DockItem::Placeholder:
//            qDebug() << "Abandon the plugin" << item->objectName() << "due to the unnecessary plugin type";
            break;
        case DockItem::TrayPlugin:
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

//    // 余数
//    int yu = (totalLength % iconCount);
//    // icon宽度 = (总宽度-余数)/icon个数
//    iconSize = (totalLength - yu) / iconCount;

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
        tray_item_size = std::min(w, h);
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

// ----------> For tray area end <----------

void MainPanelControl::setDisplayMode(DisplayMode mode) {
    qDebug() << "Not allowed to cQhange display mode";
}

void MainPanelControl::addPluginAreaItem(int index, QWidget *wdg) {
    m_pluginLayout->insertWidget(index, wdg, 0, Qt::AlignCenter);
    m_pluginAreaWidget->adjustSize();
    resizeDockIcon();
}
void MainPanelControl::mouseDoubleClickEvent(QMouseEvent *event) {
    QWidget::mouseDoubleClickEvent(event);
    QWidget *clickedWidget = childAt(event->pos());
    if (clickedWidget == nullptr) {
        Q_EMIT emptyAreaDoubleClicked();
    }
}

void MainPanelControl::dragMoveEvent(QDragMoveEvent *e) {
    DockItem *sourceItem = qobject_cast<DockItem *>(e->source());
    if (sourceItem) {
        handleDragMove(e);
        return;
    }
}

void MainPanelControl::handleDragMove(QDragMoveEvent *e) {
    DockItem *sourceItem = qobject_cast<DockItem *>(e->source());

    if (!sourceItem) {
        e->ignore();
        return;
    }

    DockItem *targetItem = dropTargetItem(sourceItem, e->pos());

    if (!targetItem) {
        e->ignore();
        return;
    }

    e->accept();

    if (targetItem == sourceItem)
        return;

    moveItem(sourceItem, targetItem);
    emit itemMoved(sourceItem, targetItem);
}

DockItem *MainPanelControl::dropTargetItem(DockItem *sourceItem, QPoint point) {
    QWidget *parentWidget = m_pluginAreaWidget;

    point = parentWidget->mapFromParent(point);
    QLayout *parentLayout = parentWidget->layout();

    DockItem *targetItem = nullptr;

    for (int i = 0 ; i < parentLayout->count(); ++i) {
        QLayoutItem *layoutItem = parentLayout->itemAt(i);
        DockItem *dockItem = qobject_cast<DockItem *>(layoutItem->widget());
        if (!dockItem)
            continue;

        QRect rect;

        rect.setTopLeft(dockItem->pos());
        if (dockItem->itemType() == DockItem::Plugins) {
            if ((m_position == Position::Top) || (m_position == Position::Bottom)) {
                rect.setSize(QSize(CustomSettings::instance()->getPanelHeight(), height()));
            } else {
                rect.setSize(QSize(width(), CustomSettings::instance()->getPanelHeight()));
            }
        } else {
            rect.setSize(dockItem->size());
        }
        if (rect.contains(point)) {
            targetItem = dockItem;
            break;
        }
    }

    return targetItem;
}

void MainPanelControl::moveItem(DockItem *sourceItem, DockItem *targetItem) {
    // get target index
    int idx = -1;
    if (targetItem->itemType() == DockItem::Plugins)
        idx = m_pluginLayout->indexOf(targetItem);
    else
        return;

    // remove old item
    removeItem(sourceItem);

    // insert new position
    insertItem(idx, sourceItem);
}

bool MainPanelControl::eventFilter(QObject *watched, QEvent *event) {
    if (event->type() != QEvent::MouseMove)
        return false;

    QMouseEvent *mouseEvent = static_cast<QMouseEvent *>(event);
    if (!mouseEvent || mouseEvent->buttons() != Qt::LeftButton)
        return false;

    DockItem *item = qobject_cast<DockItem *>(watched);
    if (!item)
        return false;

    if (item->itemType() != DockItem::App && item->itemType() != DockItem::Plugins && item->itemType() != DockItem::FixedPlugin)
        return false;

    const QPoint pos = mouseEvent->globalPos();
    const QPoint distance = pos - m_mousePressPos;
    if (distance.manhattanLength() < QApplication::startDragDistance())
        return false;

    startDrag(item);

    return QWidget::eventFilter(watched, event);
}

void MainPanelControl::mousePressEvent(QMouseEvent *e) {
    if (e->button() == Qt::LeftButton) {
        m_mousePressPos = e->globalPos();
    }

    QWidget::mousePressEvent(e);
}

void MainPanelControl::startDrag(DockItem *item) {
    const QPixmap pixmap = item->grab();

    item->setDraging(true);
    item->update();

    QDrag *drag = new QDrag(item);
    drag->setPixmap(pixmap);
    drag->setHotSpot(pixmap.rect().center() / pixmap.devicePixelRatioF());
    drag->setMimeData(new QMimeData);
    drag->exec(Qt::MoveAction);

    item->setDraging(false);
    item->update();
}

void MainPanelControl::dragEnterEvent(QDragEnterEvent *e) {
    QRect rect = QRect();
    foreach (auto item, DockItemManager::instance()->itemList()) {
        DockItem *dockItem = item.data();
        PluginsItem *pluginItem = qobject_cast<PluginsItem *>(dockItem);
        if (pluginItem && pluginItem->pluginName() == "trash") {
            rect = pluginItem->geometry();
        }
    }

    if (!rect.contains(e->pos()))
        e->accept();
}

void MainPanelControl::dragLeaveEvent(QDragLeaveEvent *event) {
    QWidget::dragLeaveEvent(event);
}

void MainPanelControl::applyCustomSettings(const CustomSettings &customSettings) {
    this->activeWindowControlWidget->applyCustomSettings(customSettings);
    this->m_buttonWidget->applyCustomSettings(customSettings);
    this->m_buttonWidget->setVisible(customSettings.isShowControlButtons() && !customSettings.isButtonOnLeft());
}
