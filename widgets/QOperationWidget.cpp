//
// Created by septemberhx on 2020/8/14.
//

#include "QOperationWidget.h"
#include "util/CustomSettings.h"

QOperationWidget::QOperationWidget(bool leftSide, QWidget *parent)
    : QWidget(parent)
{
    int buttonSize = CustomSettings::instance()->getPanelHeight() - 2;

    this->m_layout = new QHBoxLayout(this);
    this->m_layout->setContentsMargins(0, 0, 0, 0);
    this->m_layout->setContentsMargins(0, 0, 0, 0);
    this->m_layout->setSpacing(5);
    this->m_layout->setMargin(0);

    this->closeButton = new QToolButton(this);
    this->closeButton->setFixedSize(buttonSize, buttonSize);
    this->closeButton->setIcon(QIcon(CustomSettings::instance()->getActiveCloseIconPath()));
    this->closeButton->setIconSize(QSize(buttonSize - 8, buttonSize - 8));
    connect(this->closeButton, &QToolButton::clicked, this, &QOperationWidget::closeButtonClicked);

    this->maxButton = new QToolButton(this);
    this->maxButton->setFixedSize(buttonSize, buttonSize);
    this->maxButton->setIcon(QIcon(CustomSettings::instance()->getActiveUnmaximizedIconPath()));
    this->maxButton->setIconSize(QSize(buttonSize - 8, buttonSize - 8));
    connect(this->maxButton, &QToolButton::clicked, this, &QOperationWidget::maxButtonClicked);

    this->minButton = new QToolButton(this);
    this->minButton->setFixedSize(buttonSize, buttonSize);
    this->minButton->setIcon(QIcon(CustomSettings::instance()->getActiveMinimizedIconPath()));
    this->minButton->setIconSize(QSize(buttonSize - 8, buttonSize - 8));
    connect(this->minButton, &QToolButton::clicked, this, &QOperationWidget::minButtonClicked);

    if (leftSide) {
        this->m_layout->addWidget(this->closeButton);
        this->m_layout->addWidget(this->maxButton);
        this->m_layout->addWidget(this->minButton);
    } else {
        this->m_layout->addWidget(this->minButton);
        this->m_layout->addWidget(this->maxButton);
        this->m_layout->addWidget(this->closeButton);
    }

    this->m_buttonShowAnimation = new QPropertyAnimation(this, "maximumWidth");
    this->m_buttonShowAnimation->setEndValue(this->width());
    this->m_buttonShowAnimation->setDuration(150);

    this->m_buttonHideAnimation = new QPropertyAnimation(this, "maximumWidth");
    this->m_buttonHideAnimation->setEndValue(0);
    this->m_buttonHideAnimation->setDuration(150);
    connect(this->m_buttonHideAnimation, &QPropertyAnimation::finished, this, [this]() {
        this->hide();
    });
}

void QOperationWidget::hideWithAnimation() {
    this->m_buttonHideAnimation->setStartValue(this->width());
    this->m_buttonHideAnimation->start();
}

void QOperationWidget::showWithAnimation() {
    this->show();
    this->m_buttonShowAnimation->setStartValue(this->width());
    this->m_buttonShowAnimation->start();
}

void QOperationWidget::applyCustomSettings(const CustomSettings &settings) {
    this->closeButton->setIcon(QIcon(settings.getActiveCloseIconPath()));
    this->maxButton->setIcon(QIcon(settings.getActiveUnmaximizedIconPath()));
    this->minButton->setIcon(QIcon(settings.getActiveMinimizedIconPath()));
}
