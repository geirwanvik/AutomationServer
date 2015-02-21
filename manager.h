#ifndef MANAGER_H
#define MANAGER_H

#include <QObject>
#include <QList>
#include <QFile>

#include "device.h"
#include "tellstick.h"
#include "server.h"
#include "databaseitem.h"
#include "common.h"

class Manager : public QObject
{
    Q_OBJECT
public:
    explicit Manager(QObject *parent = 0);
    ~Manager();


signals:

public slots:

private slots:
    void logEvent(QString event);
    void deviceEvent(int eventId, int eventCommand, int eventValue, QString deviceType);
    void toggleLookForNewDevices(QString cmd);
    void readAllDevices();
    void writeAllDevices(QDataStream &in, int elements);

private:
    void newDeviceFound(int eventId, int eventCommand, int eventValue, QString deviceType);


    QList<Device> deviceList;
    QList<DataBaseItem*> schedulerList;
    Tellstick *tellstick;
    Server *tcpServer;
    bool lookForNewDevices;

    bool saveConfig(void);
    bool loadConfig(void);
    bool saveSchedule(void);
    bool loadSchedule(void);

};

#endif // MANAGER_H
