//
// Created by septemberhx on 2020/5/26.
//

#include <QDebug>
#include <QWindow>
#include "ActiveWindowControlWidget.h"
#include "util/XUtils.h"
#include <QMouseEvent>
#include <NETWM>
#include <QtX11Extras/QX11Info>
#include <QApplication>
#include <QScreen>
#include <QEvent>
#include <QDesktopWidget>

ActiveWindowControlWidget::ActiveWindowControlWidget(QWidget *parent)
    : QWidget(parent)
    , m_appInter(new DBusDock("com.deepin.dde.daemon.Dock", "/com/deepin/dde/daemon/Dock", QDBusConnection::sessionBus(), this))
    , m_wmInter(new DBusWM("com.deepin.wm", "/com/deepin/wm", QDBusConnection::sessionBus(), this))
    , mouseClicked(false)
    , isMenuShown(false)
    , m_currentIndex(-1)
{
    QPalette palette1 = this->palette();
    palette1.setColor(QPalette::Background, Qt::transparent);
    this->setPalette(palette1);

    this->m_layout = new QHBoxLayout(this);
    this->m_layout->setSpacing(12);
    this->m_layout->setContentsMargins(10, 5, 0, 5);
    this->setLayout(this->m_layout);

    this->m_iconLabel = new QLabel(this);
    this->m_iconLabel->setFixedSize(22, 22);
    this->m_iconLabel->setScaledContents(true);
    this->m_layout->addWidget(this->m_iconLabel);

    int buttonSize = 22;
    this->m_buttonWidget = new QWidget(this);
    this->m_buttonLayout = new QHBoxLayout(this->m_buttonWidget);
    this->m_buttonLayout->setContentsMargins(0, 0, 0, 0);
    this->m_buttonLayout->setSpacing(12);
    this->m_buttonLayout->setMargin(0);

    this->closeButton = new QToolButton(this->m_buttonWidget);
    this->closeButton->setFixedSize(buttonSize, buttonSize);
    this->closeButton->setIcon(QIcon(":/icons/close.svg"));
    this->closeButton->setIconSize(QSize(buttonSize - 8, buttonSize - 8));
    this->m_buttonLayout->addWidget(this->closeButton);

    this->maxButton = new QToolButton(this->m_buttonWidget);
    this->maxButton->setFixedSize(buttonSize, buttonSize);
    this->maxButton->setIcon(QIcon(":/icons/maximum.svg"));
    this->maxButton->setIconSize(QSize(buttonSize - 8, buttonSize - 8));
    this->m_buttonLayout->addWidget(this->maxButton);

    this->minButton = new QToolButton(this->m_buttonWidget);
    this->minButton->setFixedSize(buttonSize, buttonSize);
    this->minButton->setIcon(QIcon(":/icons/minimum.svg"));
    this->minButton->setIconSize(QSize(buttonSize - 8, buttonSize - 8));
    this->m_buttonLayout->addWidget(this->minButton);
    this->m_layout->addWidget(this->m_buttonWidget);

    this->m_menuWidget = new QWidget(this);
    this->m_layout->addWidget(this->m_menuWidget);
    this->m_menuLayout = new QHBoxLayout(this->m_menuWidget);
    this->m_menuLayout->setContentsMargins(0, 0, 0, 0);
    this->m_menuLayout->setSpacing(2);

    this->m_appMenuModel = new AppMenuModel(this);
    connect(this->m_appMenuModel, &AppMenuModel::modelNeedsUpdate, this, &ActiveWindowControlWidget::updateMenu);

    this->m_winTitleLabel = new QLabel(this);
    this->m_winTitleLabel->setAlignment(Qt::AlignLeft | Qt::AlignVCenter);
    this->m_layout->addWidget(this->m_winTitleLabel);

    this->m_layout->addStretch();

    this->m_buttonShowAnimation = new QPropertyAnimation(this->m_buttonWidget, "maximumWidth");
    this->m_buttonShowAnimation->setEndValue(this->m_buttonWidget->width());
    this->m_buttonShowAnimation->setDuration(150);

    this->m_buttonHideAnimation = new QPropertyAnimation(this->m_buttonWidget, "maximumWidth");
    this->m_buttonHideAnimation->setEndValue(0);
    this->m_buttonHideAnimation->setDuration(150);

    this->setButtonsVisible(false);
    this->setMouseTracking(true);

    connect(this->maxButton, &QToolButton::clicked, this, &ActiveWindowControlWidget::maxButtonClicked);
    connect(this->minButton, &QToolButton::clicked, this, &ActiveWindowControlWidget::minButtonClicked);
    connect(this->closeButton, &QToolButton::clicked, this, &ActiveWindowControlWidget::closeButtonClicked);

    // detect whether active window maximized signal
    connect(KWindowSystem::self(), qOverload<WId, NET::Properties, NET::Properties2>(&KWindowSystem::windowChanged), this, &ActiveWindowControlWidget::windowChanged);

    this->m_fixTimer = new QTimer(this);
    this->m_fixTimer->setSingleShot(true);
    this->m_fixTimer->setInterval(100);
    connect(this->m_fixTimer, &QTimer::timeout, this, &ActiveWindowControlWidget::activeWindowInfoChanged);
    // some applications like Gtk based and electron based seems still holds the focus after clicking the close button for a little while
    // Thus, we need to check the active window when some windows are closed.
    // However, we will use the dock dbus signal instead of other X operations.
    connect(this->m_appInter, &DBusDock::EntryRemoved, this->m_fixTimer, qOverload<>(&QTimer::start));
    connect(this->m_appInter, &DBusDock::EntryAdded, this->m_fixTimer, qOverload<>(&QTimer::start));

    applyCustomSettings(*CustomSettings::instance());
}

void ActiveWindowControlWidget::activeWindowInfoChanged() {
    int activeWinId = XUtils::getFocusWindowId();
    if (activeWinId < 0) {
        qDebug() << "Failed to get active window id !";
        return;
    }

    // fix strange focus losing when pressing alt in some applications like chrome
    if (activeWinId == 0) {
        return;
    }

    // fix focus moving to top panel bug with aurorae
    if (activeWinId == this->winId()) {
        return;
    }

    int currScreenNum = this->currScreenNum();
    int activeWinScreenNum = XUtils::getWindowScreenNum(activeWinId);
    if (activeWinScreenNum >= 0 && activeWinScreenNum != currScreenNum) {
        if (XUtils::checkIfBadWindow(this->currActiveWinId) || this->currActiveWinId == activeWinId || XUtils::checkIfWinMinimun(this->currActiveWinId)) {
//            qDebug() << "Screen" << this->currScreenNum()
//                     << XUtils::checkIfBadWindow(this->currActiveWinId)
//                     << (this->currActiveWinId == activeWinId)
//                     << XUtils::checkIfWinMinimun(this->currActiveWinId);
            this->currActiveWinId = -1;
            this->m_winTitleLabel->setText(tr("桌面"));
            this->m_iconLabel->setPixmap(QPixmap(CustomSettings::instance()->getActiveDefaultAppIconPath()));
        }
        return;
    }

    if (activeWinId != this->currActiveWinId) {
        this->currActiveWinId = activeWinId;
        // todo
    }

    this->setButtonsVisible(XUtils::checkIfWinMaximum(this->currActiveWinId));

    QString activeWinTitle = XUtils::getWindowName(this->currActiveWinId);
    this->currActiveWinTitle = activeWinTitle;
    this->m_winTitleLabel->setText(this->currActiveWinTitle);

    if (!activeWinTitle.isEmpty()) {
        this->m_iconLabel->setPixmap(XUtils::getWindowIconNameX11(this->currActiveWinId));
    }

    // KWindowSystem will not update menu for desktop when focusing on the desktop
    // It is not a good idea to do the filter here instead of the AppmenuModel.
    // However, it works, and works pretty well.
    if (activeWinTitle == tr("桌面")) {
        // hide buttons
        this->setButtonsVisible(false);

        // clear menu
        this->m_menuWidget->hide();
        for (auto m_label : this->buttonLabelList) {
            this->m_menuLayout->removeWidget(m_label);
            delete m_label;
        }
        this->buttonLabelList.clear();
        this->m_menuWidget->show();

        // show title
        this->m_winTitleLabel->show();
    }

    // some applications like KWrite will expose its global menu with an invalid dbus path
    //   thus we need to recheck it again :(
    this->m_appMenuModel->setWinId(this->currActiveWinId);
}

void ActiveWindowControlWidget::setButtonsVisible(bool visible) {
    if (visible) {
        this->m_buttonShowAnimation->setStartValue(this->m_buttonWidget->width());
        this->m_buttonShowAnimation->start();
    } else {
        this->m_buttonHideAnimation->setStartValue(this->m_buttonWidget->width());
        this->m_buttonHideAnimation->start();
    }
}

void ActiveWindowControlWidget::enterEvent(QEvent *event) {
    if (CustomSettings::instance()->isShowGlobalMenuOnHover() && !this->buttonLabelList.isEmpty()
        && XUtils::checkIfWinMaximum(this->currActiveWinId)) {
        this->m_menuWidget->show();
        this->m_winTitleLabel->hide();
    }

    QWidget::enterEvent(event);
}

void ActiveWindowControlWidget::leaveEvent(QEvent *event) {
    this->leaveTopPanel();
    QWidget::leaveEvent(event);
}

void ActiveWindowControlWidget::maxButtonClicked() {
    if (XUtils::checkIfWinMaximum(this->currActiveWinId)) {
        XUtils::unmaximizeWindow(this->currActiveWinId);

        // checkIfWinMaximum is MUST needed here.
        //   I don't know whether XSendEvent is designed like this,
        //   my test shows unmaximizeWindow by XSendEvent will work when others try to fetch its properties.
        //   i.e., checkIfWinMaximum
        XUtils::checkIfWinMaximum(this->currActiveWinId);
    } else {
        // sadly the dbus maximizeWindow cannot unmaximize window :(
        this->m_appInter->MaximizeWindow(this->currActiveWinId);
    }

//    this->activeWindowInfoChanged();
}

void ActiveWindowControlWidget::minButtonClicked() {
    this->m_appInter->MinimizeWindow(this->currActiveWinId);
}

void ActiveWindowControlWidget::closeButtonClicked() {
    this->m_appInter->CloseWindow(this->currActiveWinId);
}

void ActiveWindowControlWidget::maximizeWindow() {
    this->maxButtonClicked();
}

void ActiveWindowControlWidget::mouseDoubleClickEvent(QMouseEvent *event) {
    if (this->childAt(event->pos()) != this->m_menuWidget) {
        this->maximizeWindow();
    }
    QWidget::mouseDoubleClickEvent(event);
}

void ActiveWindowControlWidget::updateMenu() {
    this->m_menuWidget->hide();
    for (auto m_label : this->buttonLabelList) {
        this->m_menuLayout->removeWidget(m_label);
        delete m_label;
    }
    this->buttonLabelList.clear();

    QList<QString> existedMenu;  // tricks for twice menu of libreoffice
    for (int r = 0; r < m_appMenuModel->rowCount(); ++r) {
        QString menuStr = m_appMenuModel->data(m_appMenuModel->index(r, 0), AppMenuModel::MenuRole).toString();

        if (existedMenu.contains(menuStr)) {
            int index = existedMenu.indexOf(menuStr);
            auto *m_label = this->buttonLabelList.at(index);
            m_label->hide();
        }

        existedMenu.append(menuStr);
        auto *m_label = new QClickableLabel(this->m_menuWidget);
        m_label->setText(QString("  %1  ").arg(menuStr.remove('&')));
        m_label->setAlignment(Qt::AlignHCenter | Qt::AlignVCenter);
        connect(m_label, &QClickableLabel::clicked, this, &ActiveWindowControlWidget::menuLabelClicked);
        this->m_menuLayout->addWidget(m_label);
        this->buttonLabelList.append(m_label);

        if (menuStr.isEmpty()) {
            m_label->hide();
        }
    }

    if (CustomSettings::instance()->isShowGlobalMenuOnHover() && XUtils::checkIfWinMaximum(this->currActiveWinId) && !this->isMenuShown) {
        if (!this->buttonLabelList.isEmpty()) {
            this->m_menuWidget->hide();
            this->m_winTitleLabel->show();
        }
    } else {
        if (this->buttonLabelList.isEmpty()) {
            this->m_menuWidget->hide();
            this->m_winTitleLabel->show();
        } else {
            this->m_menuWidget->show();
            this->m_winTitleLabel->hide();
        }
    }
}

void ActiveWindowControlWidget::menuLabelClicked() {
    auto *label = dynamic_cast<QClickableLabel*>(sender());
    this->trigger(label, this->buttonLabelList.indexOf(label));
}

QMenu *ActiveWindowControlWidget::createMenu(int idx) const {
    QMenu *menu = nullptr;
    QAction *action = nullptr;

    const QModelIndex index = m_appMenuModel->index(idx, 0);
    const QVariant data = m_appMenuModel->data(index, AppMenuModel::ActionRole);
    action = (QAction *)data.value<void *>();
    if (action) {
        menu = action->menu();
    }

    return menu;
}

void ActiveWindowControlWidget::trigger(QClickableLabel *ctx, int idx) {
    if (m_currentIndex == idx) return;

    int oldIndex = m_currentIndex;
    m_currentIndex = idx;
    QMenu *actionMenu = createMenu(idx);

    if (actionMenu) {
        actionMenu->adjustSize();
        actionMenu->winId();//create window handle
        actionMenu->windowHandle()->setTransientParent(ctx->windowHandle());
        actionMenu->popup(this->m_menuWidget->mapToGlobal(ctx->geometry().bottomLeft()) + QPoint(0, 6));
        actionMenu->installEventFilter(this);
        this->isMenuShown = true;

        QMenu *oldMenu = m_currentMenu;
        m_currentMenu = actionMenu;
        if (oldMenu && oldMenu != actionMenu) {
            //don't initialize the currentIndex when another menu is already shown
            disconnect(oldMenu, &QMenu::aboutToHide, this, &ActiveWindowControlWidget::onMenuAboutToHide);
            oldMenu->hide();
        }

        ctx->setSelectedColor();
        if (oldIndex >=0 && oldIndex < this->buttonLabelList.size()) {
            this->buttonLabelList[oldIndex]->setNormalColor();
            this->buttonLabelList[oldIndex]->resetClicked();
        }

        connect(actionMenu, &QMenu::aboutToHide, this, &ActiveWindowControlWidget::onMenuAboutToHide, Qt::UniqueConnection);
    }
}

void ActiveWindowControlWidget::windowChanged(WId id, NET::Properties properties, NET::Properties2 properties2) {
    if (properties.testFlag(NET::WMGeometry)) {
        this->activeWindowInfoChanged();
    }

    // we still don't know why active window is 0 when pressing alt in some applications like chrome.
    if (KWindowSystem::activeWindow() != this->currActiveWinId && KWindowSystem::activeWindow() != 0) {
        return;
    }

    this->setButtonsVisible(XUtils::checkIfWinMaximum(this->currActiveWinId));
}

void ActiveWindowControlWidget::mousePressEvent(QMouseEvent *event) {
    if (event->button() == Qt::LeftButton) {
        QWidget *pressedWidget = childAt(event->pos());
        if (pressedWidget == nullptr || pressedWidget == m_winTitleLabel) {
            this->mouseClicked = !this->mouseClicked;
        }
    }
    QWidget::mousePressEvent(event);
}

void ActiveWindowControlWidget::mouseReleaseEvent(QMouseEvent *event) {
    this->mouseClicked = false;
    QWidget::mouseReleaseEvent(event);
}

void ActiveWindowControlWidget::mouseMoveEvent(QMouseEvent *event) {
    if (this->mouseClicked) {
        if (XUtils::checkIfWinMaximum(this->currActiveWinId)) {
            NETRootInfo ri(QX11Info::connection(), NET::WMMoveResize);
            ri.moveResizeRequest(
                    this->currActiveWinId,
                    event->globalX() * this->devicePixelRatio(),
                    (this->height() + event->globalY()) * this->devicePixelRatio(),
                    NET::Move
            );
            this->mouseClicked = false;
        }
    }
    QWidget::mouseMoveEvent(event);
}

void ActiveWindowControlWidget::applyCustomSettings(const CustomSettings& settings) {
    // title
    QPalette palette = this->m_winTitleLabel->palette();
    palette.setColor(QPalette::WindowText, settings.getActiveFontColor());
    this->m_winTitleLabel->setPalette(palette);
    this->m_winTitleLabel->setFont(settings.getActiveFont());

    // menu
    for (auto menuItem : this->buttonLabelList) {
        menuItem->setFont(settings.getActiveFont());
        menuItem->setDefaultFontColor(settings.getActiveFontColor());
    }

    // buttons
    this->closeButton->setIcon(QIcon(settings.getActiveCloseIconPath()));
    this->maxButton->setIcon(QIcon(settings.getActiveUnmaximizedIconPath()));
    this->minButton->setIcon(QIcon(settings.getActiveMinimizedIconPath()));

    // todo: default app icon
}

bool ActiveWindowControlWidget::eventFilter(QObject *watched, QEvent *event) {
    auto *menu = qobject_cast<QMenu *>(watched);
    if (!menu) {
        return false;
    }

    if (event->type() == QEvent::MouseMove) {
        auto *e = dynamic_cast<QMouseEvent *>(event);

        if (!this->m_menuLayout || !this->m_menuWidget) {
            return false;
        }

        const QPointF &windowLocalPos = m_menuWidget->mapFromGlobal(e->globalPos());

        auto *item = dynamic_cast<QClickableLabel*>(m_menuWidget->childAt(windowLocalPos.x(), windowLocalPos.y()));
        if (!item) {
            return false;
        }

        const int buttonIndex = this->buttonLabelList.indexOf(item);
        if (buttonIndex < 0) {
            return false;
        }

        requestActivateIndex(buttonIndex);
    }

    return false;
}

void ActiveWindowControlWidget::leaveTopPanel() {
    if (!isMenuShown && CustomSettings::instance()->isShowGlobalMenuOnHover() && !this->buttonLabelList.isEmpty()
        && XUtils::checkIfWinMaximum(this->currActiveWinId)) {
        this->m_menuWidget->hide();
        this->m_winTitleLabel->show();
    }
}

int ActiveWindowControlWidget::currScreenNum() {
    return QApplication::desktop()->screenNumber(this);
}

void ActiveWindowControlWidget::requestActivateIndex(int buttonIndex) {
    this->trigger(this->buttonLabelList[buttonIndex], buttonIndex);
}

void ActiveWindowControlWidget::onMenuAboutToHide() {
    this->buttonLabelList[this->m_currentIndex]->resetClicked();
    this->buttonLabelList[this->m_currentIndex]->setNormalColor();
    this->m_currentIndex = -1;
    this->m_currentMenu = nullptr;
    this->isMenuShown = false;
    this->leaveTopPanel();
}
