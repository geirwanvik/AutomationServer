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
    void TestSlot(int id);

private:
    QList<Device> deviceList;
    TelldusCoreAPI *tDCore;

    QString SaveConfig(void);
    QString LoadConfig(void);

};

#endif // MANAGER_H
