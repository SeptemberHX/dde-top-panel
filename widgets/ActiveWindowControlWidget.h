//
// Created by septemberhx on 2020/5/26.
//

#ifndef DDE_TOP_PANEL_ACTIVEWINDOWCONTROLWIDGET_H
#define DDE_TOP_PANEL_ACTIVEWINDOWCONTROLWIDGET_H

#include <QWidget>
#include <QLabel>
#include <QHBoxLayout>
#include <QToolButton>
#include <QMutex>
#include <com_deepin_dde_daemon_dock.h>
#include <QMenuBar>
#include "../appmenu/appmenumodel.h"
#include "QClickableLabel.h"
#include <com_deepin_wm.h>
#include "../frame/util/CustomSettings.h"
#include <com_deepin_dde_launcher.h>
#include "QOperationWidget.h"


using LauncherInter = com::deepin::dde::Launcher;

using DBusDock = com::deepin::dde::daemon::Dock;
using DBusWM = com::deepin::wm;

class ActiveWindowControlWidget : public QWidget {

    Q_OBJECT

public:
    explicit ActiveWindowControlWidget(QWidget *parent = 0);
    bool eventFilter(QObject *watched, QEvent *event) override;

signals:
    void hideOperationButtons();
    void showOperationButtons();

public slots:
    void activeWindowInfoChanged();
    void maximizeWindow();
    void applyCustomSettings(const CustomSettings& settings);
    void maxButtonClicked();
    void minButtonClicked();
    void closeButtonClicked();

protected:
    void mouseDoubleClickEvent(QMouseEvent *event) override;
    void enterEvent(QEvent *event) override;
    void leaveEvent(QEvent *event) override;
    void mouseReleaseEvent(QMouseEvent *event) override;
    void mouseMoveEvent(QMouseEvent *event) override;
    void mousePressEvent(QMouseEvent *event) override;

private:
    void setButtonsVisible(bool visible);
    QAction *createAction(int idx) const;
    void trigger(QClickableLabel *ctx, int idx);
    int currScreenNum();
    void requestActivateIndex(int buttonIndex);
    bool isMenuShown();
    void setMenuVisible(bool visible);
    void leaveTopPanel();
    int menuAvailableWidth();

private slots:
    void updateMenu();
    void menuLabelClicked();
    void windowChanged(WId, NET::Properties, NET::Properties2);
    void onMenuAboutToHide();
    void organizeMenu();

private:
    QMutex organizeMenuMutex;

    QHBoxLayout *m_layout;
    QLabel *m_winTitleLabel;

    QStack<int> activeIdStack;
    int currActiveWinId;
    QString currActiveWinTitle;

    QOperationWidget *m_buttonWidget;

    QLabel *m_iconLabel;
    QLabel *m_appNameLabel;
    QWidget *m_menuWidget;
    QLayout *m_menuLayout;
    AppMenuModel *m_appMenuModel;
    QList<QClickableLabel*> buttonLabelList;
    QList<QClickableLabel*> buttonLabelListBak;

    bool mouseClicked;
    int m_currentIndex;
    QMenu *m_currentMenu;

    DBusDock *m_appInter;
    LauncherInter *m_launcherInter;

    QTimer *m_fixTimer;

    QMenu *m_moreMenu;
    QClickableLabel *m_moreLabel;
    bool organizeFlag;

    int prevAvailableWidth;
};


#endif //DDE_TOP_PANEL_ACTIVEWINDOWCONTROLWIDGET_H
