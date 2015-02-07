#include "manager.h"

Manager::Manager(QObject *parent) : QObject(parent)
{
    QString msg;
    tDCore = new TelldusCoreAPI;

/*
    // TEST
    //int id, QString &name, QString &protocol, QString &model, QString &paramHouse, QString &paramUnit, QString &type, int methodsSupported, int lastSentCommand
    QString name = "Stikk";
    QString protocol = "arctech";
    QString model = "selflearning-switch:nexa";
    QString house = "123456";
    QString unit = "1";
    QString type = "receiver";

    Device Stikk(100,name,protocol,model,house,unit,type,0,0);

    name = "Dimmer";
    protocol = "arctech";
    model = "selflearning-switch:nexa";
    house = "123457";
    unit = "1";
    type = "receiver";

    Device Dimmer(101,name,protocol,model,house,unit,type,0,0);

    deviceList.append(Stikk);
    deviceList.append(Dimmer);

    // TEST END
*/


    // Check if config file exists, else inform client.
    msg = LoadConfig();
    if(msg.compare("Success") != 0)
    {
        // Send error could not load file
        qDebug() << msg;
    }

    // Clear all devices in telldusd
    msg = tDCore->ClearAllDevicesInCore();
    if(msg.compare("Success") != 0)
    {
        // Failed to remove devices from telldusd
        qDebug() << msg;
    }

    // Add devices from file list into telldusd
    QList<Device>::iterator i;
    for(i = deviceList.begin(); i != deviceList.end(); i++)
    {
        msg = tDCore->RegisterNewDevice((*i));
        if(msg.compare("Success") != 0)
        {
            // Send error string to client
            qDebug() << msg;
        }
    }
    for(i = deviceList.begin(); i != deviceList.end(); i++)
    {
        qDebug() << (*i).GetId() << (*i).GetName();
    }
}

void Manager::TestSlot(int id)
{
    qDebug() << "Trigg from Manager with id " << id;
}

Manager::~Manager()
{

}

QString Manager::SaveConfig()
{
    QString path = QCoreApplication::applicationDirPath();
    path.append("/config.dat");
    QFile file(path);

    if(!file.open(QIODevice::WriteOnly))
    {
        return ("Cannot open file for writing: " + file.errorString());
    }

    QDataStream out(&file);
    out.setVersion(QDataStream::Qt_5_4);

    QList<Device>::iterator i;
    for(i = deviceList.begin(); i != deviceList.end(); i++)
    {
        out << (*i);
    }

    file.close();
    return ("Success");
}

QString Manager::LoadConfig()
{
    QString path = QCoreApplication::applicationDirPath();
    path.append("/config.dat");
    QFile file(path);
    if(!file.open(QIODevice::ReadOnly))
    {
        return ("Cannot open file for reading: " + file.errorString());
    }

    QDataStream in(&file);
    in.setVersion(QDataStream::Qt_5_4);
    deviceList.clear();
    while(!in.atEnd())
    {
        Device tempDev;
        in >> tempDev;
        deviceList.append(tempDev);
    }

    file.close();
    return ("Success");
}
