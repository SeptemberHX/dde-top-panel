//
// Created by septemberhx on 2020/5/26.
//

#ifndef DDE_TOP_PANEL_ACTIVEWINDOWCONTROLWIDGET_H
#define DDE_TOP_PANEL_ACTIVEWINDOWCONTROLWIDGET_H

#include <QWidget>
#include <QLabel>
#include <QHBoxLayout>
#include <QToolButton>
#include <com_deepin_dde_daemon_dock.h>
#include <QMenuBar>
#include "../appmenu/appmenumodel.h"
#include "QClickableLabel.h"
#include <com_deepin_wm.h>

using DBusDock = com::deepin::dde::daemon::Dock;
using DBusWM = com::deepin::wm;

class ActiveWindowControlWidget : public QWidget {

    Q_OBJECT

public:
    explicit ActiveWindowControlWidget(QWidget *parent = 0);

public slots:
    void activeWindowInfoChanged();
    void maximizeWindow();

protected:
    void mouseDoubleClickEvent(QMouseEvent *event) override;

protected:
    void enterEvent(QEvent *event) override;
    void leaveEvent(QEvent *event) override;

private:
    void setButtonsVisible(bool visible);
    QMenu *createMenu(int idx) const;
    void trigger(QWidget *ctx, int idx);

private slots:
    void maxButtonClicked();
    void minButtonClicked();
    void closeButtonClicked();
    void updateMenu();
    void menuLabelClicked();
    void windowChanged(WId, NET::Properties, NET::Properties2);

private:
    QHBoxLayout *m_layout;
    QLabel *m_winTitleLabel;

    int currActiveWinId;
    QString currActiveWinTitle;

    QWidget *m_buttonWidget;
    QHBoxLayout *m_buttonLayout;
    QToolButton *closeButton;
    QToolButton *minButton;
    QToolButton *maxButton;

    QLabel *m_iconLabel;
    QWidget *m_menuWidget;
    QLayout *m_menuLayout;
    AppMenuModel *m_appMenuModel;

    QList<QClickableLabel*> buttonLabelList;

    DBusDock *m_appInter;
    DBusWM  *m_wmInter;
    bool mouseClicked;

protected:
    void mouseReleaseEvent(QMouseEvent *event) override;

    void mouseMoveEvent(QMouseEvent *event) override;

    void mousePressEvent(QMouseEvent *event) override;

private:

    QPropertyAnimation *m_buttonShowAnimation;
    QPropertyAnimation *m_buttonHideAnimation;
    QTimer *m_fixTimer;
};


#endif //DDE_TOP_PANEL_ACTIVEWINDOWCONTROLWIDGET_H
