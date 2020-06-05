//
// Created by septemberhx on 2020/6/2.
//

#include "QClickableLabel.h"
#include <QDebug>
#include <QMouseEvent>

QClickableLabel::QClickableLabel(QWidget *parent)
    : QLabel(parent)
{
    QPalette palette = this->palette();
    palette.setColor(QPalette::WindowText, Qt::black);
    palette.setColor(QPalette::Background, Qt::transparent);
    this->setPalette(palette);
    this->setMouseTracking(true);
    this->setAutoFillBackground(true);
}

void QClickableLabel::enterEvent(QEvent *event) {
    QPalette palette = this->palette();
    palette.setColor(QPalette::Background, QColor("#0081FF"));
    palette.setColor(QPalette::WindowText, Qt::white);
    this->setPalette(palette);
    this->repaint();

    QWidget::enterEvent(event);
}

void QClickableLabel::leaveEvent(QEvent *event) {
    QPalette palette = this->palette();
    palette.setColor(QPalette::Background, Qt::transparent);
    palette.setColor(QPalette::WindowText, Qt::black);
    this->setPalette(palette);
    this->repaint();

    QWidget::leaveEvent(event);
}

void QClickableLabel::mousePressEvent(QMouseEvent *ev) {
    if (ev->button() != Qt::LeftButton) {
        return;
    }

    Q_EMIT clicked();
    QLabel::mousePressEvent(ev);
}

void QClickableLabel::mouseReleaseEvent(QMouseEvent *ev) {
    QLabel::mouseReleaseEvent(ev);
}
