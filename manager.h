#ifndef MANAGER_H
#define MANAGER_H

#include <QObject>
#include <QList>
#include <QFile>

#include "device.h"
#include "sensor.h"
#include "tellduscoreapi.h"
#include "server.h"

class Manager : public QObject
{
    Q_OBJECT
public:
    explicit Manager(QObject *parent = 0);
    ~Manager();

signals:

public slots:

private slots:
    void ProcessEvents(QStringList eventList);
    void UpdateDevice(QStringList deviceData);
    void UpdateSensor(QStringList sensorData);
    void ProcessIncomingTelegram(QStringList telegram);

private:
    QList<Device> deviceList;
    QList<Sensor> sensorList;
    TelldusCoreAPI *tDCore;
    Server *tcpServer;

    QString SaveConfig(void);
    QString LoadConfig(void);

};

#endif // MANAGER_H
