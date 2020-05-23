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

#ifndef FASHIONTRAYWIDGETWRAPPER_H
#define FASHIONTRAYWIDGETWRAPPER_H

#include "../abstracttraywidget.h"

#include <QWidget>
#include <QVBoxLayout>
#include <QPointer>

#define TRAY_ITEM_DRAG_MIMEDATA "TrayItemDragDrop"

class FashionTrayWidgetWrapper : public QWidget
{
    Q_OBJECT
public:
    FashionTrayWidgetWrapper(const QString &itemKey, AbstractTrayWidget *absTrayWidget, QWidget *parent = nullptr);

    QPointer<AbstractTrayWidget> absTrayWidget() const;
    QString itemKey() const;

    bool attention() const;
    void setAttention(bool attention);
    bool isDragging();
    void cancelDragging();

Q_SIGNALS:
    void attentionChanged(const bool attention);
    void dragStart();
    void dragStop();
    void requestSwapWithDragging();

protected:
    void paintEvent(QPaintEvent *event) Q_DECL_OVERRIDE;
    void mousePressEvent(QMouseEvent *event) Q_DECL_OVERRIDE;
    void mouseMoveEvent(QMouseEvent *event) Q_DECL_OVERRIDE;
    void mouseReleaseEvent(QMouseEvent *event) Q_DECL_OVERRIDE;
    void dragEnterEvent(QDragEnterEvent *event) Q_DECL_OVERRIDE;
    void enterEvent(QEvent *event) Q_DECL_OVERRIDE;
    void leaveEvent(QEvent *event) Q_DECL_OVERRIDE;
    void resizeEvent(QResizeEvent *event) override;

private:
    void handleMouseMove(QMouseEvent *event);
    void onTrayWidgetNeedAttention();
    void onTrayWidgetClicked();

private:
    QPointer<AbstractTrayWidget> m_absTrayWidget;
    QVBoxLayout *m_layout;

    bool m_attention;
    bool m_dragging;
    bool m_hover;
    bool m_pressed;
    QString m_itemKey;
    QPoint MousePressPoint;
};

#endif //FASHIONTRAYWIDGETWRAPPER_H
