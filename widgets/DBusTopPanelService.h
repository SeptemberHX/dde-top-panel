//
// Created by ragdoll on 2022/3/30.
//

#ifndef DDE_TOP_PANEL_DBUSTOPPANELSERVICE_H
#define DDE_TOP_PANEL_DBUSTOPPANELSERVICE_H


#include <QDBusAbstractAdaptor>
#include "ActiveWindowControlWidget.h"

class DBusTopPanelService : public QDBusAbstractAdaptor {

    Q_OBJECT
    Q_CLASSINFO("D-Bus Interface", "com.septemberhx.dde.TopPanel.GrandSearch")

public:
    explicit DBusTopPanelService(ActiveWindowControlWidget *parent);
    ~DBusTopPanelService() override;

    inline ActiveWindowControlWidget *parent() const;

public slots:
    QString Search(const QString json);
    bool Stop(const QString json);
    bool Action(const QString json);
};


#endif //DDE_TOP_PANEL_DBUSTOPPANELSERVICE_H
