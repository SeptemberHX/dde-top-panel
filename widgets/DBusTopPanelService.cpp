//
// Created by ragdoll on 2022/3/30.
//

#include "DBusTopPanelService.h"
#include "toppaneladaptor.h"

DBusTopPanelService::DBusTopPanelService(ActiveWindowControlWidget *parent)
    : QDBusAbstractAdaptor(parent)
{
    new GrandSearchAdaptor(this);
    QDBusConnection conn = QDBusConnection::sessionBus();
    if (!conn.registerService("com.septemberhx.dde.TopPanel") ||
        !conn.registerObject("/com/septemberhx/dde/TopPanel", this)) {
        std::cout << "=======> Error <======" << std::endl;
    }
}

DBusTopPanelService::~DBusTopPanelService() {

}

ActiveWindowControlWidget *DBusTopPanelService::parent() const {
    return static_cast<ActiveWindowControlWidget*>(QObject::parent());
}

QString DBusTopPanelService::Search(const QString json) {
    //解析输入的json值
    QJsonDocument jsonDocument = QJsonDocument::fromJson(json.toLocal8Bit().data());
    if(!jsonDocument.isNull()) {
        QJsonObject jsonObject = jsonDocument.object();

        QJsonObject jsonResults;
        QJsonArray items;
        QStringList searchResult = this->parent()->GrandSearchSearch(jsonObject.value("cont").toString());
        for (int i = 0; i < searchResult.length(); i++) {
            QJsonObject jsonObj;
            jsonObj.insert("item", searchResult[i]);
            jsonObj.insert("name", searchResult[i].remove('&'));
            jsonObj.insert("icon", "menu");
            jsonObj.insert("type", "application/x-dde-top-panel-xx");

            items.insert(i, jsonObj);
        }

        QJsonObject objCont;
        objCont.insert("group", "TopPanel");
        objCont.insert("items", items);

        QJsonArray arrConts;
        arrConts.insert(0, objCont);

        jsonResults.insert("ver", jsonObject.value("ver"));
        jsonResults.insert("mID", jsonObject.value("mID"));
        jsonResults.insert("cont", arrConts);

        QJsonDocument document;
        document.setObject(jsonResults);

        return document.toJson(QJsonDocument::Compact);
    }

    return QString();
}

bool DBusTopPanelService::Stop(const QString json) {
    return false;
}

bool DBusTopPanelService::Action(const QString json) {
    QString searchName;
    QJsonDocument jsonDocument = QJsonDocument::fromJson(json.toLocal8Bit().data());
    if(!jsonDocument.isNull()) {
        QJsonObject jsonObject = jsonDocument.object();
        if (jsonObject.value("action") == "openitem") {
            //打开item的操作
            searchName = jsonObject.value("item").toString();
            return this->parent()->GrandSearchAction(searchName);
        }
    }
    return false;
}
