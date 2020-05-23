#pragma once

#include "indicatortraywidget.h"

#include <QObject>
#include <QScopedPointer>

class IndicatorTrayPrivate;
class IndicatorTray : public QObject
{
    Q_OBJECT
public:
    explicit IndicatorTray(const QString &indicatorName, QObject *parent = nullptr);
    ~IndicatorTray();

    IndicatorTrayWidget *widget();

    void removeWidget();

signals:
    void delayLoaded();
    void removed();

private slots:
    void textPropertyChanged(const QDBusMessage &message);
    void iconPropertyChanged(const QDBusMessage &message);

private:
    QScopedPointer<IndicatorTrayPrivate> d_ptr;
    Q_DECLARE_PRIVATE_D(qGetPtrHelper(d_ptr), IndicatorTray)
};
