#include "normalcontainer.h"
#include "../fashiontrayconstants.h"

NormalContainer::NormalContainer(TrayPlugin *trayPlugin, QWidget *parent)
    : AbstractContainer(trayPlugin, parent)
    , m_sizeAnimation(new QVariantAnimation(this))
{
    m_sizeAnimation->setEasingCurve(QEasingCurve::InOutCubic);

    connect(m_sizeAnimation, &QVariantAnimation::valueChanged, [ = ](const QVariant & value) {

        if (m_sizeAnimation->state() != QVariantAnimation::Running)
            return;

        adjustMaxSize(value.toSize());
    });

    connect(m_sizeAnimation, &QVariantAnimation::finished, [ = ]() {
        this->setVisible(expand());
    });

    connect(DWindowManagerHelper::instance(), &DWindowManagerHelper::hasCompositeChanged, this, &NormalContainer::compositeChanged, Qt::QueuedConnection);

    QTimer::singleShot(1, this, &NormalContainer::compositeChanged);
}

void NormalContainer::compositeChanged()
{
    int duration = DWindowManagerHelper::instance()->hasComposite() ? 300 : 1;
    if (isEmpty())
        duration = 0;

    m_sizeAnimation->setDuration(duration);
}

QSize NormalContainer::sizeHint() const
{
    return totalSize();
}

void NormalContainer::resizeEvent(QResizeEvent *event)
{
    if (QPropertyAnimation::Stopped == m_sizeAnimation->state()) {
        if (dockPosition() == Dock::Top || dockPosition() == Dock::Bottom)
            setMaximumWidth(totalSize().width());
        else {
            setMaximumHeight(totalSize().height());
        }
    }

    AbstractContainer::resizeEvent(event);
}

bool NormalContainer::acceptWrapper(FashionTrayWidgetWrapper *wrapper)
{
    Q_UNUSED(wrapper);

    return true;
}

void NormalContainer::addWrapper(FashionTrayWidgetWrapper *wrapper)
{
    AbstractContainer::addWrapper(wrapper);

    if (containsWrapper(wrapper)) {
        const QString &key = HoldKeyPrefix + wrapper->absTrayWidget()->itemKeyForConfig();
        trayPlugin()->saveValue(wrapper->itemKey(), key, false);
    }
}

void NormalContainer::refreshVisible()
{
    AbstractContainer::refreshVisible();

    for (auto w : wrapperList()) {
        if (dockPosition() == Dock::Top || dockPosition() == Dock::Bottom)
            w->setFixedSize(itemSize(), QWIDGETSIZE_MAX);
        else
            w->setFixedSize(QWIDGETSIZE_MAX, itemSize());
    }

    if (isEmpty()) {
        // set the minimum size to 1 to avoid can not drag back wrappers when
        // all wrappers has been drag out
        setMinimumSize(TraySpace, TraySpace);
    } else {
        // set the minimum size back to 0 in order to make the fold animation works correctly
        setMinimumSize(0, 0);
    }

    compositeChanged();
    QSize endSize = expand() ? totalSize() : QSize(0, 0);

    const QPropertyAnimation::State state = m_sizeAnimation->state();

    if (state == QPropertyAnimation::Stopped && size() == endSize) {
        setVisible(expand());
        return;
    }

    if (state == QPropertyAnimation::Running)
        return m_sizeAnimation->setEndValue(endSize);

    m_sizeAnimation->setStartValue(size());
    m_sizeAnimation->setEndValue(endSize);

    if (isVisible() == expand()) {
        // 非x86平台，第一次启动setEndValue时，不进QVariantAnimation::valueChanged
        adjustMaxSize(endSize);
        return;
    }

    if (expand()) {
        setVisible(true);
    }

    m_sizeAnimation->start();
}

void NormalContainer::adjustMaxSize(const QSize size)
{
    if (dockPosition() == Dock::Top || dockPosition() == Dock::Bottom) {
        this->setMaximumWidth(size.width());
        this->setMaximumHeight(DOCK_MAX_SIZE);
    } else {
        this->setMaximumWidth(DOCK_MAX_SIZE);
        this->setMaximumHeight(size.height());
    }
}

int NormalContainer::itemCount()
{
    if (expand())
        return AbstractContainer::itemCount();
    else
        return 0;
}

void NormalContainer::setExpand(const bool expand)
{
    int itemSize;

    if (dockPosition() == Dock::Position::Top || dockPosition() == Dock::Position::Bottom)
        itemSize = std::min(parentWidget()->height(), PLUGIN_BACKGROUND_MAX_SIZE);
    else
        itemSize = std::min(parentWidget()->width(), PLUGIN_BACKGROUND_MAX_SIZE);

    for (auto w : wrapperList()) {
        w->setAttention(false);

//        w->setFixedSize(itemSize, itemSize);
    }

    AbstractContainer::setExpand(expand);
}

int NormalContainer::whereToInsert(FashionTrayWidgetWrapper *wrapper)
{
    // 如果已经对图标进行过排序则完全按照从配置文件中获取的顺序来插入图标(即父类的实现)
    if (trayPlugin()->traysSortedInFashionMode()) {
        return AbstractContainer::whereToInsert(wrapper);
    }

    // 如果没有对图标进行过排序则使用下面的默认排序算法:
    // 所有应用图标在系统图标的左侧
    // 新的应用图标在最左侧的应用图标处插入
    // 新的系统图标在最左侧的系统图标处插入
    return whereToInsertByDefault(wrapper);
}

int NormalContainer::whereToInsertByDefault(FashionTrayWidgetWrapper *wrapper) const
{
    int index = 0;
    switch (wrapper->absTrayWidget()->trayTyep()) {
    case AbstractTrayWidget::TrayType::ApplicationTray:
        index = whereToInsertAppTrayByDefault(wrapper);
        break;
    case AbstractTrayWidget::TrayType::SystemTray:
        index = whereToInsertSystemTrayByDefault(wrapper);
        break;
    default:
        Q_UNREACHABLE();
        break;
    }
    return index;
}

int NormalContainer::whereToInsertAppTrayByDefault(FashionTrayWidgetWrapper *wrapper) const
{
    if (wrapperList().isEmpty() || wrapper->absTrayWidget()->trayTyep() != AbstractTrayWidget::TrayType::ApplicationTray) {
        return 0;
    }

    int lastAppTrayIndex = -1;
    for (int i = 0; i < wrapperList().size(); ++i) {
        if (wrapperList().at(i)->absTrayWidget()->trayTyep() == AbstractTrayWidget::TrayType::ApplicationTray) {
            lastAppTrayIndex = i;
            continue;
        }
        break;
    }
    // there is no AppTray
    if (lastAppTrayIndex == -1) {
        return 0;
    }
    // the inserting tray is not a AppTray
    if (wrapper->absTrayWidget()->trayTyep() != AbstractTrayWidget::TrayType::ApplicationTray) {
        return lastAppTrayIndex + 1;
    }

    int insertIndex = trayPlugin()->itemSortKey(wrapper->itemKey());
    // invalid index
    if (insertIndex < -1) {
        return 0;
    }
    for (int i = 0; i < wrapperList().size(); ++i) {
        if (wrapperList().at(i)->absTrayWidget()->trayTyep() != AbstractTrayWidget::TrayType::ApplicationTray) {
            insertIndex = i;
            break;
        }
        if (insertIndex > trayPlugin()->itemSortKey(wrapperList().at(i)->itemKey())) {
            continue;
        }
        insertIndex = i;
        break;
    }
    if (insertIndex > lastAppTrayIndex + 1) {
        insertIndex = lastAppTrayIndex + 1;
    }

    return insertIndex;
}

int NormalContainer::whereToInsertSystemTrayByDefault(FashionTrayWidgetWrapper *wrapper) const
{
    if (wrapperList().isEmpty()) {
        return 0;
    }

    int firstSystemTrayIndex = -1;
    for (int i = 0; i < wrapperList().size(); ++i) {
        if (wrapperList().at(i)->absTrayWidget()->trayTyep() == AbstractTrayWidget::TrayType::SystemTray) {
            firstSystemTrayIndex = i;
            break;
        }
    }
    // there is no SystemTray
    if (firstSystemTrayIndex == -1) {
        return wrapperList().size();
    }
    // the inserting tray is not a SystemTray
    if (wrapper->absTrayWidget()->trayTyep() != AbstractTrayWidget::TrayType::SystemTray) {
        return firstSystemTrayIndex;
    }

    int insertIndex = trayPlugin()->itemSortKey(wrapper->itemKey());
    // invalid index
    if (insertIndex < -1) {
        return firstSystemTrayIndex;
    }
    for (int i = 0; i < wrapperList().size(); ++i) {
        if (wrapperList().at(i)->absTrayWidget()->trayTyep() != AbstractTrayWidget::TrayType::SystemTray) {
            continue;
        }
        if (insertIndex > trayPlugin()->itemSortKey(wrapperList().at(i)->itemKey())) {
            continue;
        }
        insertIndex = i;
        break;
    }
    if (insertIndex < firstSystemTrayIndex) {
        return firstSystemTrayIndex;
    }

    return insertIndex;
}
void NormalContainer::updateSize()
{
    if (QPropertyAnimation::Stopped == m_sizeAnimation->state()) {
        if (dockPosition() == Dock::Top || dockPosition() == Dock::Bottom)
            setMaximumWidth(totalSize().width());
        else {
            setMaximumHeight(totalSize().height());
        }
    }
}
