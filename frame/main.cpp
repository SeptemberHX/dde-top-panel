//
// Created by septemberhx on 2020/5/22.
//

#include <DApplication>
#include <DGuiApplicationHelper>
#include <controller/dockitemmanager.h>
#include "window/MainWindow.h"
#include "../appmenu/menuimporter.h"

DWIDGET_USE_NAMESPACE
#ifdef DCORE_NAMESPACE
DCORE_USE_NAMESPACE
#else
DUTIL_USE_NAMESPACE
#endif


int main(int argc, char *argv[]) {

    DGuiApplicationHelper::setUseInactiveColorGroup(false);
    DApplication::loadDXcbPlugin();
    DApplication app(argc, argv);

    app.setOrganizationName("septemberhx");
    app.setApplicationName("dde-top-panel");
    app.setApplicationDisplayName("DDE Top Panel");
    app.setApplicationVersion("0.4.1");
    app.loadTranslator();
    app.setAttribute(Qt::AA_EnableHighDpiScaling, true);
    app.setAttribute(Qt::AA_UseHighDpiPixmaps, false);

//    MainWindow mw(qApp->primaryScreen());
//    mw.loadPlugins();

    TopPanelLauncher launcher;

    return app.exec();
}