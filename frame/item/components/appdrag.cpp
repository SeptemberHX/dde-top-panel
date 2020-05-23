/*
 * Copyright (C) 2011 ~ 2018 Deepin Technology Co., Ltd.
 *
 * Author:     listenerri <listenerri@gmail.com>
 *
 * Maintainer: listenerri <listenerri@gmail.com>
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

#include "appdrag.h"

AppDrag::AppDrag(QObject *dragSource) : QDrag(dragSource)
{
    // delete by itself
    m_appDragWidget = new AppDragWidget;
    m_appDragWidget->setVisible(false);
}

AppDrag::~AppDrag() {
    // delete when AppDragWidget is invisible
    if (m_appDragWidget && !m_appDragWidget->isVisible()) {
        m_appDragWidget->deleteLater();
    }
}

void AppDrag::setPixmap(const QPixmap &pix)
{
    m_appDragWidget->setAppPixmap(pix);
}

QPixmap AppDrag::pixmap() const
{
    /* TODO: return pixmap */
    return QPixmap();
}

Qt::DropAction AppDrag::start(Qt::DropActions supportedActions)
{
    m_appDragWidget->show();
    return QDrag::start(supportedActions);
}

Qt::DropAction AppDrag::exec(Qt::DropActions supportedActions)
{
    m_appDragWidget->show();
    return QDrag::exec(supportedActions);
}

Qt::DropAction AppDrag::exec(Qt::DropActions supportedActions, Qt::DropAction defaultAction)
{
    m_appDragWidget->show();
    return QDrag::exec(supportedActions, defaultAction);
}

AppDragWidget *AppDrag::appDragWidget()
{
    return m_appDragWidget;
}
