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


private:
    QList<Device> deviceList;
    TelldusCoreAPI *tDCore;
    Server *tcpServer;

    QString SaveConfig(void);
    QString LoadConfig(void);

};

#endif // MANAGER_H
