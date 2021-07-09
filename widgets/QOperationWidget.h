//
// Created by septemberhx on 2020/8/14.
//

#ifndef DDE_TOP_PANEL_QOPERATIONWIDGET_H
#define DDE_TOP_PANEL_QOPERATIONWIDGET_H

#include <QWidget>
#include <QHBoxLayout>
#include <QToolButton>
#include <QPropertyAnimation>
#include <QLabel>
#include "util/CustomSettings.h"

class QOperationWidget : public QWidget {
    Q_OBJECT

public:
    QOperationWidget(bool leftSide, QWidget *parent = nullptr);
    void applyCustomSettings(const CustomSettings& settings);
    void hideWithAnimation();
    void showWithAnimation();

signals:
    void closeButtonClicked();
    void minButtonClicked();
    void maxButtonClicked();
    void animationFinished();

private:
    QHBoxLayout *m_layout;
    QToolButton *closeButton;
    QToolButton *minButton;
    QToolButton *maxButton;

    QLabel *wrapLabel;
    QHBoxLayout *wrapLayout;

    QPropertyAnimation *m_buttonShowAnimation;
    QPropertyAnimation *m_buttonHideAnimation;
};


#endif //DDE_TOP_PANEL_QOPERATIONWIDGET_H
