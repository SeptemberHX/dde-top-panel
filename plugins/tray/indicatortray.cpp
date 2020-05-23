#include "indicatortray.h"

#include <QLabel>
#include <QDBusConnection>
#include <QJsonObject>
#include <QDBusInterface>
#include <QDBusReply>
#include <QDebug>
#include <QApplication>
#include <QJsonDocument>
#include <QFile>
#include <QTimer>
#include <QDBusMessage>
#include <thread>

class IndicatorTrayPrivate
{
public:
    IndicatorTrayPrivate(IndicatorTray *parent) : q_ptr(parent) {}

    void init();

    void updateContent();

    void initDBus(const QString &indicatorName);

    template<typename Func>
    void featData(const QString &key,
                  const QJsonObject &data,
                  const char *propertyChangedSlot,
                  Func const &callback)
    {
        Q_Q(IndicatorTray);
        auto dataConfig = data.value(key).toObject();
        auto dbusService = dataConfig.value("dbus_service").toString();
        auto dbusPath = dataConfig.value("dbus_path").toString();
        auto dbusInterface = dataConfig.value("dbus_interface").toString();
        auto isSystemBus = dataConfig.value("system_dbus").toBool(false);
        auto bus = isSystemBus ? QDBusConnection::systemBus() : QDBusConnection::sessionBus();

        QDBusInterface interface(dbusService, dbusPath, dbusInterface, bus, q);

        if (dataConfig.contains("dbus_method")) {
            QString methodName = dataConfig.value("dbus_method").toString();
            auto ratio = qApp->devicePixelRatio();
            QDBusReply<QByteArray> reply = interface.call(methodName.toStdString().c_str(), ratio);
            callback(reply.value());
        }

        if (dataConfig.contains("dbus_properties")) {
            auto propertyName = dataConfig.value("dbus_properties").toString();
            auto propertyNameCStr = propertyName.toStdString();
            propertyInterfaceNames.insert(key, dbusInterface);
            propertyNames.insert(key, QString::fromStdString(propertyNameCStr));
            QDBusConnection::sessionBus().connect(dbusService,
                                                  dbusPath,
                                                  "org.freedesktop.DBus.Properties",
                                                  "PropertiesChanged",
                                                  "sa{sv}as",
                                                  q,
                                                  propertyChangedSlot);

            // FIXME(sbw): hack for qt dbus property changed signal.
            // see: https://bugreports.qt.io/browse/QTBUG-48008
            QDBusConnection::sessionBus().connect(dbusService,
                                                  dbusPath,
                                                  dbusInterface,
                                                  QString("%1Changed").arg(propertyName),
                                                  "s",
                                                  q,
                                                  propertyChangedSlot);

            callback(interface.property(propertyNameCStr.c_str()));
        }
    }

    template<typename Func>
    void propertyChanged(const QString &key, const QDBusMessage &msg, Func const &callback)
    {
        QList<QVariant> arguments = msg.arguments();
        if (1 == arguments.count())
        {
            const QString &v = msg.arguments().at(0).toString();
            callback(v);
            return;
        } else if (3 != arguments.count()) {
            qWarning() << "arguments count must be 3";
            return;
        }

        QString interfaceName = msg.arguments().at(0).toString();
        if (interfaceName != propertyInterfaceNames.value(key)) {
            qWarning() << "interfaceName mismatch" << interfaceName << propertyInterfaceNames.value(key) << key;
            return;
        }
        QVariantMap changedProps = qdbus_cast<QVariantMap>(arguments.at(1).value<QDBusArgument>());
        if (changedProps.contains(propertyNames.value(key))) {
            callback(changedProps.value(propertyNames.value(key)));
        }
    }

    IndicatorTrayWidget*    indicatorTrayWidget = Q_NULLPTR;
    QString                 indicatorName;
    QMap<QString, QString>  propertyNames;
    QMap<QString, QString>  propertyInterfaceNames;

    IndicatorTray *q_ptr;
    Q_DECLARE_PUBLIC(IndicatorTray)
};

void IndicatorTrayPrivate::init()
{
    //Q_Q(IndicatorTray);

    indicatorTrayWidget = new IndicatorTrayWidget(indicatorName);

    initDBus(indicatorName);
    updateContent();
}

void IndicatorTrayPrivate::updateContent()
{
    indicatorTrayWidget->update();

    Q_EMIT indicatorTrayWidget->iconChanged();
}

void IndicatorTrayPrivate::initDBus(const QString &indicatorName)
{
    Q_Q(IndicatorTray);

    QString filepath = QString("/etc/dde-dock/indicator/%1.json").arg(indicatorName);
    QFile confFile(filepath);
    if (!confFile.open(QIODevice::ReadOnly)) {
        qCritical() << "read indicator config Error";
    }

    QJsonDocument doc = QJsonDocument::fromJson(confFile.readAll());
    confFile.close();
    auto config = doc.object();

    auto delay = config.value("delay").toInt(0);

    qDebug() << "delay load" << delay << indicatorName << q;

    QTimer::singleShot(delay, [ = ]() {
        auto data = config.value("data").toObject();

        if (data.contains("text")) {
            featData("text", data, SLOT(textPropertyChanged(QDBusMessage)), [ = ](QVariant v) {
                if (v.toString().isEmpty()) {
                    Q_EMIT q->removed();
                    return;
                }
                else {
                    Q_EMIT q->delayLoaded();
                }
                indicatorTrayWidget->setText(v.toString());
                updateContent();
            });
        }

        if (data.contains("icon")) {
            featData("icon", data, SLOT(iconPropertyChanged(QDBusMessage)), [ = ](QVariant v) {
                if (v.toByteArray().isEmpty()) {
                    Q_EMIT q->removed();
                    return;
                }
                else {
                    Q_EMIT q->delayLoaded();
                }
                indicatorTrayWidget->setPixmapData(v.toByteArray());
                updateContent();
            });
        }

        const QJsonObject action = config.value("action").toObject();
        if (!action.isEmpty())
            q->connect(indicatorTrayWidget, &IndicatorTrayWidget::clicked, q, [ = ](uint8_t button_index, int x, int y) {
                std::thread t([=]() -> void {
                    auto triggerConfig = action.value("trigger").toObject();
                    auto dbusService = triggerConfig.value("dbus_service").toString();
                    auto dbusPath = triggerConfig.value("dbus_path").toString();
                    auto dbusInterface = triggerConfig.value("dbus_interface").toString();
                    auto methodName = triggerConfig.value("dbus_method").toString();
                    auto isSystemBus = triggerConfig.value("system_dbus").toBool(false);
                    auto bus = isSystemBus ? QDBusConnection::systemBus() : QDBusConnection::sessionBus();

                    QDBusInterface interface(dbusService, dbusPath, dbusInterface, bus);
                    QDBusReply<void> reply = interface.call(methodName, button_index, x, y);
                    if (!reply.isValid()) {
                        qDebug() << interface.call(methodName);
                    }
                    else {
                        qDebug() << reply.error();
                    }
                });
                t.detach();
            });
    });
}

IndicatorTray::IndicatorTray(const QString &indicatorName, QObject *parent)
    : QObject(parent)
    , d_ptr(new IndicatorTrayPrivate(this))
{
    Q_D(IndicatorTray);

    d->indicatorName = indicatorName;
    d->init();
}

IndicatorTray::~IndicatorTray()
{

}

IndicatorTrayWidget *IndicatorTray::widget()
{
    Q_D(IndicatorTray);

    if (!d->indicatorTrayWidget) {
        d->init();
    }

    return d->indicatorTrayWidget;
}

void IndicatorTray::removeWidget()
{
    Q_D(IndicatorTray);

    d->indicatorTrayWidget = nullptr;
}

void IndicatorTray::textPropertyChanged(const QDBusMessage &message)
{
    Q_D(IndicatorTray);

    d->propertyChanged("text", message, [=] (const QVariant &value) {
        if (value.toString().isEmpty()) {
            Q_EMIT removed();
            return;
        }

        if (!d->indicatorTrayWidget) {
            d->init();
        }

        d->indicatorTrayWidget->setText(value.toByteArray());
    });
}

void IndicatorTray::iconPropertyChanged(const QDBusMessage &message)
{
    Q_D(IndicatorTray);

    d->propertyChanged("icon", message, [=] (const QVariant &value) {
        if (value.toByteArray().isEmpty()) {
            Q_EMIT removed();
            return;
        }

        if (!d->indicatorTrayWidget) {
            d->init();
        }

        d->indicatorTrayWidget->setPixmapData(value.toByteArray());
    });
}
