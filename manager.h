#ifndef MANAGER_H
#define MANAGER_H

#include <QObject>
#include <QList>
#include <QFile>

#include "device.h"
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
    void DeviceEvent(int eventId, int eventCommand, const char *eventData);
    void ProcessIncomingTelegram(QStringList telegram);

private:


    QList<Devices> deviceList;
    TelldusCoreAPI *tDCore;
    Server *tcpServer;

    QString SaveConfig(void);
    QString LoadConfig(void);
    QString SaveSchedule(void);
    QString LoadSchedule(void);

};

#endif // MANAGER_H
