# DDE Top Panel

【中文请看】：[Deepin 论坛](https://bbs.deepin.org/forum.php?mod=viewthread&tid=195128&extra=)

Top panel for deepin desktop environment v20.

This is a modification of dde-dock for top panel. Comparing to dde-dock, it:
* remove the launcher, show desktop, multi tasking, and application icons
* full support for dde-dock plugins and tray
* remove lots of unnecessary things so it is just a top panel. It is fixed to the top of the screen, and there is no way to change that.
* separate plugin config gsetting path `/com/deepin/dde/toppanel`
* different plugin directories: `/usr/lib/dde-top-panel/plugins` and `~/.local/lib/dde-top-panel/plugins`

> Still in development.

## Features

* show window title on top panel
* show title buttons on top panel when current window maximized
* double click on empty area on top panel will maximize current window
* coping with `~/.config/kwinrc` can remove the window title bar when maximized


Know issues:
* it seems impossible to set the minimum height of the top panel less than 40, or there will be abnormal left and right spacing/margin around the plugin widgets. `setMargins` and `setSpacing` seem not work.
* The position of the top panel somethings will not be the top when there are some windows block it.

## Screenshot

![](./screenshots/toppanel1.png)
![](./screenshots/toppanel2.png)
![](./screenshots/demo.gif)

## How to run

1. download zip from release page and unzip it
1. cp `*.xml` to `/usr/share/glib-2.0/schemas`, and run `sudo glib-compile-schemas /usr/share/glib-2.0/schemas`
2. run `./dde-top-panel`
3. for removing the title bar of maximized windows, make sure your `~/.config/kwinrc` contains items below, then logout.
```shell script
[Windows]
BorderlessMaximizedWindows=true
```
