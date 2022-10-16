//
// Created by septemberhx on 2020/5/22.
//

#include <DApplication>
#include <DGuiApplicationHelper>
#include <unistd.h>
#include <iostream>
#include "window/MainWindow.h"

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

    QString locale = QLocale::system().name();

    QTranslator translator;
    translator.load("/usr/share/dde-top-panel/translations/dde-top-panel_"+ locale +".qm");
    app.installTranslator(&translator);

    app.setOrganizationName("septemberhx");
    app.setApplicationName("dde-top-panel");
    app.setApplicationDisplayName("DDE Top Panel");
    app.setApplicationVersion("0.6.8");
    app.loadTranslator();
    app.setAttribute(Qt::AA_EnableHighDpiScaling, true);
    app.setAttribute(Qt::AA_UseHighDpiPixmaps, false);

//    MainWindow mw(qApp->primaryScreen());
//    mw.loadPlugins();

    TopPanelLauncher launcher;

    return app.exec();
}
