//
// Created by septemberhx on 2020/6/2.
//

#include "QClickableLabel.h"
#include <QDebug>
#include <QMouseEvent>
#include <iostream>

QClickableLabel::QClickableLabel(QWidget *parent)
    : QLabel(parent)
    , isClicked(false)
{
    this->setNormalColor();
    this->setMouseTracking(true);
    this->setAutoFillBackground(true);
    this->metrics = new QFontMetrics(this->font());
}

void QClickableLabel::enterEvent(QEvent *event) {
    std::cout << "==========> Set selected color due to enterEvent" << std::endl;
    this->setSelectedColor();
    QWidget::enterEvent(event);
}

void QClickableLabel::leaveEvent(QEvent *event) {
    if (!isClicked) {
        std::cout << "==========> Set selected color due to leaveEvent" << std::endl;
        this->setNormalColor();
        isClicked = false;
    }
    QWidget::leaveEvent(event);
}

void QClickableLabel::mousePressEvent(QMouseEvent *ev) {
    if (ev->button() != Qt::LeftButton) {
        return;
    }

    isClicked = true;
    Q_EMIT clicked();
    QLabel::mousePressEvent(ev);
}

void QClickableLabel::mouseReleaseEvent(QMouseEvent *ev) {
    QLabel::mouseReleaseEvent(ev);
}

void QClickableLabel::setDefaultFontColor(const QColor &defaultFontColor) {
    this->defaultFontColor = defaultFontColor;
    this->setNormalColor();
}

void QClickableLabel::setSelectedColor() {
    this->setStyleSheet("QClickableLabel { background-color: #0081FF; border-radius: 4px; color: white; }");
}

void QClickableLabel::setNormalColor() {
    this->setStyleSheet(QString("QClickableLabel { background-color: transparent; color: %1; }").arg(this->defaultFontColor.name()));
}

void QClickableLabel::resetClicked() {
    this->isClicked = false;
}

int QClickableLabel::standardWidth() {
    int fontWidth = metrics->boundingRect(this->text()).width();
    return fontWidth + this->contentsMargins().left() + this->contentsMargins().right();
}

bool QClickableLabel::isSelected() const {
    return isClicked;
}

void QClickableLabel::setClicked(bool flag) {
    if (flag) {
        this->isClicked = true;
        this->setSelectedColor();
    } else {
        this->resetClicked();
        this->setNormalColor();
    }
}
