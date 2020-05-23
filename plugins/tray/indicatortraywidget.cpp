/*
 * Copyright (C) 2011 ~ 2018 Deepin Technology Co., Ltd.
 *
 * Author:     sbw <sbw@sbw.so>
 *
 * Maintainer: sbw <sbw@sbw.so>
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

#include "indicatortraywidget.h"

#include <QDebug>
#include <QLabel>
#include <QBoxLayout>

#include <QDBusConnection>
#include <QDBusInterface>

IndicatorTrayWidget::IndicatorTrayWidget(const QString &indicatorName, QWidget *parent, Qt::WindowFlags f)
    : AbstractTrayWidget(parent, f)
    , m_indicatorName(indicatorName)
{
    setAttribute(Qt::WA_TranslucentBackground);

    auto layout = new QVBoxLayout(this);
    layout->setContentsMargins(0, 0, 0, 0);
    m_label = new QLabel(this);

    QPalette p = palette();
    p.setColor(QPalette::Foreground, Qt::white);
    p.setColor(QPalette::Background, Qt::red);
    m_label->setPalette(p);

    m_label->setAttribute(Qt::WA_TranslucentBackground);

    layout->addWidget(m_label, 0, Qt::AlignCenter);
    setLayout(layout);

    // register dbus
    auto path = QString("/com/deepin/dde/Dock/Indicator/") + m_indicatorName;
    auto interface =  QString("com.deepin.dde.Dock.Indicator.") + m_indicatorName;
    auto sessionBus = QDBusConnection::sessionBus();
    sessionBus.registerObject(path,
                              interface,
                              this,
                              QDBusConnection::ExportScriptableSlots);
}

IndicatorTrayWidget::~IndicatorTrayWidget()
{
}

QString IndicatorTrayWidget::itemKeyForConfig()
{
    return toIndicatorKey(m_indicatorName);
}

void IndicatorTrayWidget::setActive(const bool)
{

}

void IndicatorTrayWidget::updateIcon()
{

}

const QImage IndicatorTrayWidget::trayImage()
{
    return m_label->grab().toImage();
}

void IndicatorTrayWidget::sendClick(uint8_t buttonIndex, int x, int y)
{
    Q_EMIT clicked(buttonIndex, x, y);
}

void IndicatorTrayWidget::setPixmapData(const QByteArray &data)
{
    auto rawPixmap = QPixmap::fromImage(QImage::fromData(data));
    rawPixmap.setDevicePixelRatio(devicePixelRatioF());
    m_label->setPixmap(rawPixmap);
}

void IndicatorTrayWidget::setPixmapPath(const QString &text)
{
    m_label->setPixmap(QPixmap(text));
}

void IndicatorTrayWidget::setText(const QString &text)
{
    m_label->setText(text);
}

