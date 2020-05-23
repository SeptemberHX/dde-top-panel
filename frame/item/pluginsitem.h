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

#ifndef PLUGINSITEM_H
#define PLUGINSITEM_H

#include "dockitem.h"
#include "pluginsiteminterface.h"

class QGSettings;
class PluginsItem : public DockItem
{
    Q_OBJECT

public:
    explicit PluginsItem(PluginsItemInterface *const pluginInter, const QString &itemKey, QWidget *parent = nullptr);
    ~PluginsItem() override;

    int itemSortKey() const;
    void setItemSortKey(const int order) const;
    void detachPluginWidget();

    void setInContainer(const bool container);

    QString pluginName() const;

    using DockItem::showContextMenu;
    using DockItem::hidePopup;

    ItemType itemType() const override;
    QSize sizeHint() const override;

    QWidget *centralWidget() const;

    virtual void setDraging(bool bDrag) override;

public slots:
    void refershIcon() override;
    void onGSettingsChanged(const QString &key);

protected:
    void mousePressEvent(QMouseEvent *e) override;
    void mouseMoveEvent(QMouseEvent *e) override;
    void mouseReleaseEvent(QMouseEvent *e) override;
    void enterEvent(QEvent *event) Q_DECL_OVERRIDE;
    void leaveEvent(QEvent *event) Q_DECL_OVERRIDE;
    bool eventFilter(QObject *watched, QEvent *event) Q_DECL_OVERRIDE;
    void showEvent(QShowEvent *event) override;

    void invokedMenuItem(const QString &itemId, const bool checked) override;
    void showPopupWindow(QWidget *const content, const bool model = false) override;
    const QString contextMenu() const override;
    QWidget *popupTips() override;
    void resizeEvent(QResizeEvent *event) override;

private:
    void startDrag();
    void mouseClicked();
    bool checkGSettingsControl() const;

private:
    PluginsItemInterface *const m_pluginInter;
    QWidget *m_centralWidget;

    const QString m_itemKey;
    bool m_dragging;

    static QPoint MousePressPoint;
    QGSettings *m_gsettings;
};

#endif // PLUGINSITEM_H
