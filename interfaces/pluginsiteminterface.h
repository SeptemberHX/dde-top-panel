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

#ifndef PLUGINSITEMINTERFACE_H
#define PLUGINSITEMINTERFACE_H

#include "pluginproxyinterface.h"

#include <QIcon>
#include <QtCore>

///
/// \brief The PluginsItemInterface class
/// the dock plugins item interface, all dock plugins should
/// inheirt this class and override all pure virtual function.
///
class PluginsItemInterface
{
public:
    enum PluginType {
        Normal,
        Fixed
    };

    /**
    * @brief Plugin size policy
    */
    enum PluginSizePolicy {
        System = 1 << 0, // Follow the system
        Custom = 1 << 1  // The custom
    };

    ///
    /// \brief ~PluginsItemInterface
    /// DON'T try to delete m_proxyInter.
    ///
    virtual ~PluginsItemInterface() {}

    ///
    /// \brief pluginName
    /// tell dock the unique plugin id
    /// \return
    ///
    virtual const QString pluginName() const = 0;
    virtual const QString pluginDisplayName() const { return QString(); }

    ///
    /// \brief init
    /// init your plugins, you need to save proxyInter to m_proxyInter
    /// member variable. but you shouldn't free this pointer.
    /// \param proxyInter
    /// DON'T try to delete this pointer.
    ///
    virtual void init(PluginProxyInterface *proxyInter) = 0;
    ///
    /// \brief itemWidget
    /// your plugin item widget, each item should have a unique key.
    /// \param itemKey
    /// your widget' unqiue key.
    /// \return
    ///
    virtual QWidget *itemWidget(const QString &itemKey) = 0;

    ///
    /// \brief itemTipsWidget
    /// override this function if your item want to have a tips.
    /// the tips will shown when user hover your item.
    /// nullptr will be ignored.
    /// \param itemKey
    /// \return
    ///
    virtual QWidget *itemTipsWidget(const QString &itemKey) {Q_UNUSED(itemKey); return nullptr;}
    ///
    /// \brief itemPopupApplet
    /// override this function if your item wants to have an popup applet.
    /// the popup applet will shown when user click your item.
    ///
    /// Tips:
    /// dock should receive mouse press/release event to check user mouse operate,
    /// if your item filter mouse event, this function will not be called.
    /// so if you override mouse event and want to use popup applet, you
    /// should pass event to your parent use QWidget::someEvent(e);
    /// \param itemKey
    /// \return
    ///
    virtual QWidget *itemPopupApplet(const QString &itemKey) {Q_UNUSED(itemKey); return nullptr;}
    ///
    /// \brief itemCommand
    /// execute spec command when user clicked your item.
    /// ensure your command do not get user input.
    ///
    /// empty string will be ignored.
    /// \param itemKey
    /// \return
    ///
    virtual const QString itemCommand(const QString &itemKey) {Q_UNUSED(itemKey); return QString();}

    ///
    /// \brief itemContextMenu
    /// context menu is shown when RequestPopupMenu called.
    /// \param itemKey
    /// \return
    ///
    virtual const QString itemContextMenu(const QString &itemKey) {Q_UNUSED(itemKey); return QString();}
    ///
    /// \brief invokedMenuItem
    /// call if context menu item is clicked
    /// \param itemKey
    /// \param itemId
    /// menu item id
    /// \param checked
    ///
    virtual void invokedMenuItem(const QString &itemKey, const QString &menuId, const bool checked) {Q_UNUSED(itemKey); Q_UNUSED(menuId); Q_UNUSED(checked);}

    ///
    /// \brief itemSortKey
    /// tell dock where your item wants to put on.
    ///
    /// this index is start from 1 and
    /// 0 for left side
    /// -1 for right side
    /// \param itemKey
    /// \return
    ///
    virtual int itemSortKey(const QString &itemKey) {Q_UNUSED(itemKey); return 1;}
    ///
    /// \brief setSortKey
    /// save your item new position
    /// sort key will be changed when plugins order
    /// changed(by user drag-drop)
    /// \param itemKey
    /// \param order
    ///
    virtual void setSortKey(const QString &itemKey, const int order) {Q_UNUSED(itemKey); Q_UNUSED(order);}

    ///
    /// \brief itemAllowContainer
    /// tell dock is your item allow to move into container
    ///
    /// if your item placed into container, popup tips and popup
    /// applet will be disabled.
    /// \param itemKey
    /// \return
    ///
    virtual bool itemAllowContainer(const QString &itemKey) {Q_UNUSED(itemKey); return false;}
    ///
    /// \brief itemIsInContainer
    /// tell dock your item is in container, this function
    /// called at item init and if your item enable container.
    /// \param itemKey
    /// \return
    ///
    virtual bool itemIsInContainer(const QString &itemKey) {Q_UNUSED(itemKey); return false;}
    ///
    /// \brief setItemIsInContainer
    /// save your item new state.
    /// this function called when user drag out your item from
    /// container or user drop item into container(if your item
    /// allow drop into container).
    /// \param itemKey
    /// \param container
    ///
    virtual void setItemIsInContainer(const QString &itemKey, const bool container) {Q_UNUSED(itemKey); Q_UNUSED(container);}

    virtual bool pluginIsAllowDisable() { return false; }
    virtual bool pluginIsDisable() { return false; }
    virtual void pluginStateSwitched() {}

    ///
    /// \brief displayModeChanged
    /// override this function to receive display mode changed signal
    /// \param displayMode
    ///
    virtual void displayModeChanged(const Dock::DisplayMode displayMode) {Q_UNUSED(displayMode);}
    ///
    /// \brief positionChanged
    /// override this function to receive dock position changed signal
    /// \param position
    ///
    virtual void positionChanged(const Dock::Position position) {Q_UNUSED(position);}

    ///
    /// \brief refreshIcon
    /// refresh item icon, its triggered when system icon theme changed.
    /// \param itemKey
    /// item key
    ///
    virtual void refreshIcon(const QString &itemKey) { Q_UNUSED(itemKey); }

    ///
    /// \brief displayMode
    /// get current dock display mode
    /// \return
    ///
    inline Dock::DisplayMode displayMode() const
    {
        return qApp->property(PROP_DISPLAY_MODE).value<Dock::DisplayMode>();
    }

    ///
    /// \brief position
    /// get current dock position
    /// \return
    ///
    inline Dock::Position position() const
    {
        return qApp->property(PROP_POSITION).value<Dock::Position>();
    }

    ///
    /// \brief settingsChanged
    /// override this function to receive plugin settings changed signal(DeepinSync)
    ///
    virtual void pluginSettingsChanged() {}

    ///
    /// \brief type
    /// default plugin add dock right,fixed plugin add to dock fixed area
    ///
    virtual PluginType type() { return Normal; }

    ///
    /// \brief plugin size policy
    /// default plugin size policy
    ///
    virtual PluginSizePolicy pluginSizePolicy() const { return System; }

protected:
    ///
    /// \brief m_proxyInter
    /// NEVER delete this object.
    ///
    PluginProxyInterface *m_proxyInter = nullptr;
};

QT_BEGIN_NAMESPACE

#define ModuleInterface_iid "com.deepin.dock.PluginsItemInterface"

Q_DECLARE_INTERFACE(PluginsItemInterface, ModuleInterface_iid)
QT_END_NAMESPACE

#endif // PLUGINSITEMINTERFACE_H
