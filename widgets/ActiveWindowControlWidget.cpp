//
// Created by septemberhx on 2020/5/26.
//

#include <QDebug>
#include <QWidget>
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
#include <iostream>

ActiveWindowControlWidget::ActiveWindowControlWidget(QWidget *parent)
    : QWidget(parent)
    , m_appInter(new DBusDock("com.deepin.dde.daemon.Dock", "/com/deepin/dde/daemon/Dock", QDBusConnection::sessionBus(), this))
    , mouseClicked(false)
    , m_currentIndex(-1)
    , m_currentMenu(nullptr)
    , m_moreMenu(new QMenu())
    , organizeFlag(false)
    , m_launcherInter(new LauncherInter("com.deepin.dde.Launcher", "/com/deepin/dde/Launcher", QDBusConnection::sessionBus(), this))
{
    m_launcherInter->setSync(true, false);

    QPalette palette1 = this->palette();
    palette1.setColor(QPalette::Background, Qt::transparent);
    this->setPalette(palette1);

    this->m_layout = new QHBoxLayout(this);
    this->m_layout->setSpacing(16);
    this->m_layout->setContentsMargins(20, 0, 0, 0);
    this->setLayout(this->m_layout);

    this->m_iconLabel = new QLabel(this);
    this->m_iconLabel->setFixedSize(18, 18);
    this->m_iconLabel->setScaledContents(true);
    this->m_layout->addWidget(this->m_iconLabel);

    int buttonSize = 22;
    this->m_buttonWidget = new QOperationWidget(true, this);
    this->m_layout->addWidget(this->m_buttonWidget);

    this->m_appNameLabel = new QLabel(this);
    this->m_appNameLabel->setFixedHeight(22);
    this->m_layout->addWidget(this->m_appNameLabel);

    this->m_menuWidget = new QWidget(this);
    this->installEventFilter(this);

    this->m_moreLabel = new QClickableLabel(this->m_menuWidget);
    this->m_moreLabel->setText(tr("   ▸   "));
    this->m_moreLabel->setAlignment(Qt::AlignCenter);
    connect(this->m_moreLabel, &QClickableLabel::clicked, this, &ActiveWindowControlWidget::menuLabelClicked);
    this->m_moreLabel->hide();

    this->m_layout->addWidget(this->m_menuWidget);
    this->m_menuLayout = new QHBoxLayout(this->m_menuWidget);
    this->m_menuLayout->setContentsMargins(0, 0, 0, 0);
    this->m_menuLayout->setSpacing(2);
    this->m_appMenuModel = new AppMenuModel(this);
    connect(this->m_appMenuModel, &AppMenuModel::modelNeedsUpdate, this, &ActiveWindowControlWidget::updateMenu);

    this->m_winTitleLabel = new QLabel(this);
    this->m_winTitleLabel->setAlignment(Qt::AlignLeft | Qt::AlignVCenter);
    this->m_winTitleLabel->setContentsMargins(0, 4, 0, 4);
    this->m_layout->addWidget(this->m_winTitleLabel);

    this->m_layout->addStretch();

    this->setButtonsVisible(false);
    this->setMouseTracking(true);

    connect(this->m_buttonWidget, &QOperationWidget::maxButtonClicked, this, &ActiveWindowControlWidget::maxButtonClicked);
    connect(this->m_buttonWidget, &QOperationWidget::minButtonClicked, this, &ActiveWindowControlWidget::minButtonClicked);
    connect(this->m_buttonWidget, &QOperationWidget::closeButtonClicked, this, &ActiveWindowControlWidget::closeButtonClicked);

    connect(this, &ActiveWindowControlWidget::showOperationButtons, this, [this] {
        if (CustomSettings::instance()->isButtonOnLeft()) {
            this->m_buttonWidget->showWithAnimation();
        }
    });
    connect(this, &ActiveWindowControlWidget::hideOperationButtons, this, [this] {
        if (CustomSettings::instance()->isButtonOnLeft()) {
            this->m_buttonWidget->hideWithAnimation();
        }
    });
    connect(this->m_buttonWidget, &QOperationWidget::animationFinished, this, &ActiveWindowControlWidget::organizeMenu);

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
    this->m_buttonWidget->hide();
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
        bool ifFoundPrevActiveWinId = false;
        if (XUtils::checkIfBadWindow(this->currActiveWinId) || this->currActiveWinId == activeWinId || XUtils::checkIfWinMinimun(this->currActiveWinId)) {
            int newCurActiveWinId = -1;
            while (!this->activeIdStack.isEmpty()) {
                int prevActiveId = this->activeIdStack.pop();
                if (XUtils::checkIfBadWindow(prevActiveId) || this->currActiveWinId == prevActiveId || XUtils::checkIfWinMinimun(prevActiveId)) {
                    continue;
                }

                int sNum = XUtils::getWindowScreenNum(prevActiveId);
                if (sNum != currScreenNum) {
                    continue;
                }

                newCurActiveWinId = prevActiveId;
                break;
            }

            if (newCurActiveWinId < 0) {
                this->currActiveWinId = -1;
                if (!CustomSettings::instance()->isShowAppNameInsteadIcon()) {
                    this->m_iconLabel->setPixmap(QPixmap(CustomSettings::instance()->getActiveDefaultAppIconPath()));
                    this->m_winTitleLabel->show();
                    this->m_winTitleLabel->setText(tr("桌面"));
                } else {
                    this->m_winTitleLabel->hide();
                }
                this->m_appNameLabel->setText(tr("桌面"));
            } else {
                activeWinId = newCurActiveWinId;
                ifFoundPrevActiveWinId = true;
            }
        }
        if (!ifFoundPrevActiveWinId) {
            return;
        }
    }

    if (activeWinId != this->currActiveWinId) {
        this->currActiveWinId = activeWinId;
        this->activeIdStack.push(this->currActiveWinId);
    }

    this->setButtonsVisible(XUtils::checkIfWinMaximum(this->currActiveWinId));

    QString activeWinTitle = XUtils::getWindowName(this->currActiveWinId);
    this->currActiveWinTitle = activeWinTitle;
    this->m_winTitleLabel->setText(this->currActiveWinTitle);

    if (!activeWinTitle.isEmpty()) {
        if (!CustomSettings::instance()->isShowAppNameInsteadIcon()) {
            this->m_iconLabel->setPixmap(XUtils::getWindowIconNameX11(this->currActiveWinId));
        }
        this->m_appNameLabel->setText(XUtils::getWindowAppName(this->currActiveWinId));
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
        }
        for (auto m_label : this->buttonLabelListBak) {
            delete m_label;
        }
        this->buttonLabelListBak.clear();
        this->buttonLabelList.clear();
        this->setMenuVisible(false);
    }

    // some applications like KWrite will expose its global menu with an invalid dbus path
    //   thus we need to recheck it again :(
    this->m_appMenuModel->setWinId(this->currActiveWinId);
}

void ActiveWindowControlWidget::setButtonsVisible(bool visible) {
    if (CustomSettings::instance()->isShowControlButtons()) {
        if (visible) {
            emit showOperationButtons();
        } else {
            emit hideOperationButtons();
        }
    }
}

void ActiveWindowControlWidget::enterEvent(QEvent *event) {
    if (CustomSettings::instance()->isShowGlobalMenuOnHover() && !this->buttonLabelList.isEmpty()
        && XUtils::checkIfWinMaximum(this->currActiveWinId)) {
        this->setMenuVisible(true);
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
    KWindowSystem::self()->minimizeWindow(this->currActiveWinId);
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
    this->buttonLabelListBak.clear();

    QList<QString> existedMenu;  // tricks for twice menu of libreoffice
    for (int r = 0; r < m_appMenuModel->rowCount(); ++r) {
        QString menuStr = m_appMenuModel->data(m_appMenuModel->index(r, 0), AppMenuModel::MenuRole).toString();

        // tricks to remove useless old menus
        if (existedMenu.contains(menuStr)) {
            int index = existedMenu.indexOf(menuStr);
            auto *m_label = this->buttonLabelListBak.at(index);
            m_label->hide();
        }
        existedMenu.append(menuStr);

        // create new clickable label for the new menu item
        auto *m_label = new QClickableLabel(this->m_menuWidget);
        menuStr = menuStr.remove('&');
        int index = menuStr.lastIndexOf('(');
        if (index > 0) {
            menuStr = menuStr.mid(0, index);
        }
        m_label->setText(QString("   %1   ").arg(menuStr));
        m_label->setAlignment(Qt::AlignHCenter | Qt::AlignVCenter);
        connect(m_label, &QClickableLabel::clicked, this, &ActiveWindowControlWidget::menuLabelClicked);
        this->buttonLabelListBak.append(m_label);
        if (menuStr.isEmpty()) {
            m_label->hide();
        }
    }

    this->organizeMenu();
}

void ActiveWindowControlWidget::menuLabelClicked() {
    auto *label = dynamic_cast<QClickableLabel*>(sender());
    this->trigger(label, this->buttonLabelList.indexOf(label));
}

// for modern gtk applications, some menu has no sub menus, and only action is presented.
// thus, we obtain the action instead of the menu so we can perform the action when no submenu
QAction *ActiveWindowControlWidget::createAction(int idx) const {
    const QModelIndex index = m_appMenuModel->index(idx, 0);
    const QVariant data = m_appMenuModel->data(index, AppMenuModel::ActionRole);
    return (QAction *)data.value<void *>();
}

void ActiveWindowControlWidget::trigger(QClickableLabel *ctx, int idx) {
    if (ctx == nullptr) return;

    if (m_currentIndex == idx) return;
    int oldIndex = m_currentIndex;
    m_currentIndex = idx;

    QMenu *actionMenu = nullptr;
    QAction *action = nullptr;
    if (ctx == this->m_moreLabel) {
        actionMenu = this->m_moreMenu;
    } else {
        QAction *action = createAction(idx);
        this->m_moreMenu->hide();

        if (action == nullptr) {
            return;
        }

        actionMenu = action->menu();
    }

    if (actionMenu) {
        actionMenu->adjustSize();
        actionMenu->winId();//create window handle
        actionMenu->windowHandle()->setTransientParent(ctx->windowHandle());
        actionMenu->popup(this->m_menuWidget->mapToGlobal(ctx->geometry().bottomLeft()) + QPoint(0, 1));
        actionMenu->installEventFilter(this);

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
    } else if (action != nullptr) {
        action->trigger();
    }
}

void ActiveWindowControlWidget::windowChanged(WId id, NET::Properties properties, NET::Properties2 properties2) {
    if (properties.testFlag(NET::WMGeometry) || properties.testFlag(NET::WMName)) {
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
        } else if (qobject_cast<QLabel*>(pressedWidget) == this->m_iconLabel) {
            if (this->m_launcherInter->visible()) {
                this->m_launcherInter->Hide();
            } else {
                this->m_launcherInter->Show();
            }
        }
        KWindowSystem::activateWindow(this->currActiveWinId);
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
    this->m_buttonWidget->setVisible(CustomSettings::instance()->isButtonOnLeft() && CustomSettings::instance()->isShowControlButtons());
    this->m_buttonWidget->applyCustomSettings(settings);

    // show icons or app name
    palette = this->m_appNameLabel->palette();
    palette.setColor(QPalette::WindowText, settings.getActiveFontColor());
    this->m_appNameLabel->setPalette(palette);
    this->m_appNameLabel->setFont(QFont(settings.getActiveFont().family(), settings.getActiveFont().pointSize(), QFont::DemiBold));
    if (settings.isShowAppNameInsteadIcon()) {
        if (settings.isShowLogoWithAppName()) {
            this->m_iconLabel->show();
            this->m_iconLabel->setPixmap(QPixmap(settings.getActiveDefaultAppIconPath()));
        } else {
            this->m_iconLabel->hide();
        }
        this->m_appNameLabel->show();
    } else {
        this->m_iconLabel->show();
        this->m_appNameLabel->hide();
    }
}

bool ActiveWindowControlWidget::eventFilter(QObject *watched, QEvent *event) {
    auto *menu = qobject_cast<QMenu *>(watched);
    if (menu) {
        if (event->type() == QEvent::MouseMove) {
            auto *e = dynamic_cast<QMouseEvent *>(event);

            if (!this->m_menuLayout || !this->m_menuWidget) {
                return false;
            }

            const QPointF &windowLocalPos = m_menuWidget->mapFromGlobal(e->globalPos());

            auto *item = dynamic_cast<QClickableLabel *>(m_menuWidget->childAt(windowLocalPos.x(), windowLocalPos.y()));
            if (!item) {
                return false;
            }

            const int buttonIndex = this->buttonLabelList.indexOf(item);
            if (buttonIndex < 0) {
                return false;
            }

            requestActivateIndex(buttonIndex);
        }
    }

    if (event->type() == QEvent::Resize) {
        if (!this->organizeFlag) {
            this->organizeMenu();
        }
    }

    return false;
}

void ActiveWindowControlWidget::leaveTopPanel() {
    if (!this->isMenuShown() && CustomSettings::instance()->isShowGlobalMenuOnHover() && !this->buttonLabelList.isEmpty()
        && XUtils::checkIfWinMaximum(this->currActiveWinId)) {
        this->setMenuVisible(false);
    }
}

int ActiveWindowControlWidget::currScreenNum() {
    return QApplication::desktop()->screenNumber(this);
}

void ActiveWindowControlWidget::requestActivateIndex(int buttonIndex) {
    this->trigger(this->buttonLabelList[buttonIndex], buttonIndex);
}

void ActiveWindowControlWidget::onMenuAboutToHide() {
    if (this->m_currentIndex < this->buttonLabelList.size()) {
        this->buttonLabelList[this->m_currentIndex]->resetClicked();
        this->buttonLabelList[this->m_currentIndex]->setNormalColor();
        this->m_currentIndex = -1;
        this->m_currentMenu = nullptr;
    }
    this->leaveTopPanel();
}

bool ActiveWindowControlWidget::isMenuShown() {
    return this->m_currentMenu != nullptr;
}

void ActiveWindowControlWidget::setMenuVisible(bool visible) {
    this->m_winTitleLabel->setVisible(!visible && CustomSettings::instance()->isShowControlButtons());
    this->m_menuWidget->setVisible(visible);

    if (!this->m_winTitleLabel->isVisible()) {
        this->m_menuWidget->setVisible(true);
    }

    if (CustomSettings::instance()->isShowAppNameInsteadIcon()
        && (this->m_appNameLabel->text() == this->m_winTitleLabel->text() || !CustomSettings::instance()->isShowControlButtons() )) {
        this->m_winTitleLabel->setVisible(false);
    }
}

void ActiveWindowControlWidget::organizeMenu() {
    this->m_menuWidget->hide();

    // calculate left space for menus
    int usedWidth = this->m_layout->contentsMargins().left() + this->m_layout->contentsMargins().right();
    usedWidth += this->m_iconLabel->width();

    if (this->m_buttonWidget->isVisible()) {
        usedWidth += this->m_layout->spacing() + this->m_buttonWidget->width();
    }

    if (this->m_appNameLabel->isVisible()) {
        usedWidth += this->m_layout->spacing() + this->m_appNameLabel->width();
    }

    if (this->m_winTitleLabel->isVisible()) {
        usedWidth += this->m_layout->spacing() + this->m_winTitleLabel->width();
    }
    int availableWidth = this->width() - usedWidth - this->m_layout->spacing();

    // reset menu layout
    foreach (auto *label, this->buttonLabelList) {
        label->hide();
        this->m_menuLayout->removeWidget(label);
    }

    this->buttonLabelList.clear();
    this->m_moreMenu->clear();

    // check where we need to cut the menu
    int totalWidth = this->m_menuLayout->contentsMargins().left() + this->m_menuLayout->contentsMargins().right();
    int breakIndex = -1;
    for (int i = 0; i < this->buttonLabelListBak.size(); ++i) {
        int prevWidth = totalWidth;
        totalWidth += this->buttonLabelListBak[i]->standardWidth();
        if (i != this->buttonLabelListBak.size() - 1) {
            totalWidth += this->m_menuLayout->spacing();
        }

        if (totalWidth > availableWidth) {
            breakIndex = i;
            totalWidth = prevWidth;
            break;
        }
    }

    if (breakIndex > 0) {
        breakIndex -= 1;
    }

    if (breakIndex < 0) {
        breakIndex = this->buttonLabelListBak.size();
    }

    // show menus and hide others
    for (int i = 0; i < breakIndex; ++i) {
        this->buttonLabelListBak[i]->show();
        this->m_menuLayout->addWidget(this->buttonLabelListBak[i]);
        this->buttonLabelList.append(this->buttonLabelListBak[i]);
    }

    for (int i = breakIndex; i < this->buttonLabelListBak.size(); ++i) {
        this->buttonLabelListBak[i]->hide();
        auto *action = this->createAction(i);
        this->m_moreMenu->addAction(action);
    }

    // "More" menu item
    if (breakIndex != this->buttonLabelListBak.size()) {
        this->buttonLabelList.append(this->m_moreLabel);
        this->m_menuLayout->addWidget(this->m_moreLabel);
        this->m_moreLabel->show();
    } else {
        this->m_moreLabel->hide();
    }

    // menu visible
    if (CustomSettings::instance()->isShowGlobalMenuOnHover() && XUtils::checkIfWinMaximum(this->currActiveWinId) && !this->isMenuShown()) {
        if (!this->buttonLabelListBak.isEmpty()) {
            this->setMenuVisible(false);
        }
    } else {
        this->setMenuVisible(!this->buttonLabelListBak.isEmpty());
    }
}
