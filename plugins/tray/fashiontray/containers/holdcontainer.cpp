#include "holdcontainer.h"
#include "../fashiontrayconstants.h"

HoldContainer::HoldContainer(TrayPlugin *trayPlugin, QWidget *parent)
    : AbstractContainer(trayPlugin, parent)
    , m_mainBoxLayout(new QBoxLayout(QBoxLayout::Direction::LeftToRight, this))
{
    m_mainBoxLayout->setMargin(0);
    m_mainBoxLayout->setContentsMargins(0, 0, 0, 0);
    m_mainBoxLayout->setSpacing(TraySpace);

    QBoxLayout *preLayout = wrapperLayout();
    QBoxLayout *newLayout = new QBoxLayout(QBoxLayout::Direction::LeftToRight, this);
    for (int i = 0; i < preLayout->count(); ++i) {
        newLayout->addItem(preLayout->takeAt(i));
    }
    setWrapperLayout(newLayout);

    m_mainBoxLayout->addLayout(newLayout);

//    setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
    setLayout(m_mainBoxLayout);
}

bool HoldContainer::acceptWrapper(FashionTrayWidgetWrapper *wrapper)
{
    const QString &key = HoldKeyPrefix + wrapper->absTrayWidget()->itemKeyForConfig();

    return trayPlugin()->getValue(wrapper->itemKey(), key, false).toBool();
}

void HoldContainer::addWrapper(FashionTrayWidgetWrapper *wrapper)
{
    AbstractContainer::addWrapper(wrapper);

    if (containsWrapper(wrapper)) {
        const QString &key = HoldKeyPrefix + wrapper->absTrayWidget()->itemKeyForConfig();
        trayPlugin()->saveValue(wrapper->itemKey(), key, true);
    }
}

void HoldContainer::refreshVisible()
{
    AbstractContainer::refreshVisible();

    setVisible(true);
}

void HoldContainer::setDockPosition(const Dock::Position pos)
{
    if (pos == Dock::Position::Top || pos == Dock::Position::Bottom) {
        m_mainBoxLayout->setDirection(QBoxLayout::Direction::LeftToRight);
    } else{
        m_mainBoxLayout->setDirection(QBoxLayout::Direction::TopToBottom);
    }

    AbstractContainer::setDockPosition(pos);
}

//QSize HoldContainer::totalSize() const
//{
//    QSize size = AbstractContainer::totalSize();

//    if (expand()) {
//        if (dockPosition() == Dock::Position::Top || dockPosition() == Dock::Position::Bottom) {
//            size.setHeight(height());
//        } else {
//            size.setWidth(width());
//        }
//    }

//    return size;
//}
