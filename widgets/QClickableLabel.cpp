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
    this->setSelectedColor();
    QWidget::enterEvent(event);
}

void QClickableLabel::leaveEvent(QEvent *event) {
    if (!isClicked) {
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
    QPalette palette = this->palette();
    palette.setColor(QPalette::WindowText, this->defaultFontColor);
    this->setPalette(palette);
    this->repaint();
}

void QClickableLabel::setSelectedColor() {
    QPalette palette = this->palette();
    palette.setColor(QPalette::Background, QColor("#0081FF"));
    palette.setColor(QPalette::WindowText, Qt::white);
    this->setPalette(palette);
    this->repaint();
}

void QClickableLabel::setNormalColor() {
    QPalette palette = this->palette();
    palette.setColor(QPalette::Background, Qt::transparent);
    palette.setColor(QPalette::WindowText, this->defaultFontColor);
    this->setPalette(palette);
    this->repaint();
}

void QClickableLabel::resetClicked() {
    this->isClicked = false;
}

int QClickableLabel::standardWidth() {
    int fontWidth = metrics->boundingRect(this->text()).width();
//    std::cout << this->text().toStdString() << ": " << fontWidth << std::endl;
    return fontWidth + this->contentsMargins().left() + this->contentsMargins().right();
}
