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
    void RawEvent(QStringList eventList);
    void DeviceEvent(QList<int> param);
    void SensorEvent(QStringList param);
    void ProcessDeviceEvents(Device &dev);
    void ProcessSensorEvents(Sensor &sens);
    void ProcessIncomingTelegram(QStringList telegram);

private:


    QList<Device> deviceList;
    QList<Sensor> sensorList;
    TelldusCoreAPI *tDCore;
    Server *tcpServer;

    QString SaveConfig(void);
    QString LoadConfig(void);
    QString SaveSchedule(void);
    QString LoadSchedule(void);

};

#endif // MANAGER_H
