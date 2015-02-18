#ifndef MANAGER_H
#define MANAGER_H

#include <QObject>
#include <QList>
#include <QFile>

#include "device.h"
#include "tellduscoreapi.h"
#include "server.h"
#include "databaseitem.h"

class Manager : public QObject
{
    Q_OBJECT
public:
    explicit Manager(QObject *parent = 0);
    ~Manager();


signals:

public slots:

private slots:
    void DeviceEvent(int eventId, int eventCommand, int eventData, QString type);
    void ProcessIncomingTelegram(QStringList telegram);
    void AboutToQuit();

private:


    QList<Devices> deviceList;
    QList<DataBaseItem*> schedulerList;
    TelldusCoreAPI *telldusCore;
    Server *tcpServer;

    QString SaveConfig(void);
    QString LoadConfig(void);
    QString SaveSchedule(void);
    QString LoadSchedule(void);

};

#endif // MANAGER_H
