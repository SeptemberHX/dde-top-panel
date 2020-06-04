# DDE Top Panel

【中文请看】：[Deepin 论坛](https://bbs.deepin.org/forum.php?mod=viewthread&tid=195128&extra=)
【全局菜单基本原理请看】：[Github WIKI](https://github.com/SeptemberHX/dde-top-panel/wiki/Linux-%E4%B8%8A%E7%9A%84%E5%85%A8%E5%B1%80%E8%8F%9C%E5%8D%95%E5%8E%9F%E7%90%86)

Top panel for deepin desktop environment v20.

This is a modification of dde-dock for top panel. Comparing to dde-dock, it:
* remove the launcher, show desktop, multi tasking, and application icons
* full support for dde-dock plugins and tray
* remove lots of unnecessary things so it is just a top panel. It is fixed to the top of the screen, and there is no way to change that.
* separate plugin config gsetting path `/com/deepin/dde/toppanel`
* different plugin directories: `/usr/lib/dde-top-panel/plugins` and `~/.local/lib/dde-top-panel/plugins`


## Features

* global menu support (with dde-globalmenu-service)
* show window title on top panel
* show title buttons on top panel when current window maximized
* double click on empty area on top panel will maximize current window
* dragging and moving current maximized window by dragging empty area of top panel
* coping with `~/.config/kwinrc` can remove the window title bar when maximized
* support multi monitors

Know issues:
* panels on **non-primary monitors** only have the window title function. The plugins can not work on them. `It is not an issue` because the `QPluginLoader` can only create one instance from one plugin file. So the panel cannot create as many plugin widgets as the panels. Still trying to add the plugin back to all panels
* Shortcuts of the global menu not work yet


## Behaviors

* Always show title buttons for maxmized window and hide for unmaxminzed window
* Show window title only when no global menu for current window

## Screenshot

![](./screenshots/toppanel1.png)
![](./screenshots/toppanel2.png)
![](./screenshots/demo.gif)

![](./screenshots/globalmenu.gif)

## How to run

1. download zip from release page and unzip it
1. cp `*.xml` to `/usr/share/glib-2.0/schemas`, and run `sudo glib-compile-schemas /usr/share/glib-2.0/schemas`
2. run `./dde-top-panel`
3. for removing the title bar of maximized windows, make sure your `~/.config/kwinrc` contains items below, then logout.
```shell script
[Windows]
BorderlessMaximizedWindows=true
```
4. If you want to use plugins on top panel, just copy the plugin files to `~/.local/lib/dde-top-panel/plugins`. For example, if you want to get tray icons on top panel, just `cp /usr/lib/dde-dock/plugins/libtray.so ~/.local/lib/dde-top-panel/plugins`
5. If you want to enable the global menu, please install !(dde-globalmenu-service)[https://github.com/SeptemberHX/dde-globalmenu-service.git]


## For tray icons of wine applications

Due to the logical of tray plugins, only one tray widget can hold the wine trays (You can click the icon and it will response to the click).
 
The main code of wine trays is in `plugins/tray/xembedtraywidget`. Generally, it wraps the raw wine trays with a new widget and embeds it to the tray, then it operates on the new widget. The problem is every tray widget will create a new container widget for each wine trays, and the tray widgets launched before can't the window id of the new container widget.

For now, I use QSharedMemory to share the window id of the container widget. However, the size of shared memory is 1MB, and it will be used up if you kill and open the panels and docks for many many time.

This part needs help.

If you want to fix it in my way, just use the `libtray.so` (DO NOT FORGET to back up the system one) 
