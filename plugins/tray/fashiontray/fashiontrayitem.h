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

#ifndef FASHIONTRAYITEM_H
#define FASHIONTRAYITEM_H

#include "constants.h"
#include "../trayplugin.h"
#include "fashiontraywidgetwrapper.h"
#include "fashiontraycontrolwidget.h"
#include "containers/normalcontainer.h"
#include "containers/attentioncontainer.h"
#include "containers/holdcontainer.h"

#include <QWidget>
#include <QPointer>
#include <QBoxLayout>
#include <QLabel>

#include "../abstracttraywidget.h"

#define FASHION_MODE_ITEM_KEY   "fashion-mode-item"

class FashionTrayItem : public QWidget
{
    Q_OBJECT

public:
    explicit FashionTrayItem(TrayPlugin *trayPlugin, QWidget *parent = 0);

    void setTrayWidgets(const QMap<QString, AbstractTrayWidget *> &itemTrayMap);
    void trayWidgetAdded(const QString &itemKey, AbstractTrayWidget *trayWidget);
    void trayWidgetRemoved(AbstractTrayWidget *trayWidget);
    void clearTrayWidgets();

    void setDockPosition(Dock::Position pos);

    inline static int trayWidgetWidth() {return TrayWidgetWidth;}
    inline static int trayWidgetHeight() {return TrayWidgetHeight;}

public slots:
    void onExpandChanged(const bool expand);
    void setRightSplitVisible(const bool visible);
    void onPluginSettingsChanged();

protected:
    void showEvent(QShowEvent *event) Q_DECL_OVERRIDE;
    void hideEvent(QHideEvent *event) Q_DECL_OVERRIDE;
    void resizeEvent(QResizeEvent *event) Q_DECL_OVERRIDE;
    void dragEnterEvent(QDragEnterEvent *event) Q_DECL_OVERRIDE;
    bool event(QEvent *event) override;

private:
    void init();
    void resizeTray();

private Q_SLOTS:
    void onWrapperAttentionChanged(FashionTrayWidgetWrapper *wrapper, const bool attention);
    void attentionWrapperToNormalWrapper();
    void normalWrapperToAttentionWrapper(FashionTrayWidgetWrapper *wrapper);
    void requestResize();
    void refreshHoldContainerPosition();
    void onRequireDraggingWrapper();

private:
    QBoxLayout *m_mainBoxLayout;
    QTimer *m_attentionDelayTimer;

    TrayPlugin *m_trayPlugin;
    FashionTrayControlWidget *m_controlWidget;
    FashionTrayWidgetWrapper *m_currentDraggingTray;

    NormalContainer *m_normalContainer;
    AttentionContainer *m_attentionContainer;
    HoldContainer *m_holdContainer;

    static int TrayWidgetWidth;
    static int TrayWidgetHeight;
    QWidget *m_leftSpace;
    Dock::Position m_dockpos;
    int m_iconSize  = 40;
};

#endif // FASHIONTRAYITEM_H
