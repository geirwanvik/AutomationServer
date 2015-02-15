#include "manager.h"

Manager::Manager(QObject *parent) : QObject(parent)
{
    qRegisterMetaType< QList<int> >("QList<int>");
    QString msg;
    tDCore = new TelldusCoreAPI(this);
    tcpServer = new Server(this);

    // Singleton instance of TelldusCoreAPI to forward events from the static callback functions
    connect(TelldusCore::Instance(),SIGNAL(DeviceChangeEvent(QStringList)),this,SLOT(RawEvent(QStringList)));
    connect(TelldusCore::Instance(),SIGNAL(ControllerEvent(QStringList)),this,SLOT(RawEvent(QStringList)));
    connect(TelldusCore::Instance(),SIGNAL(RawDataEvent(QStringList)),this,SLOT(RawEvent(QStringList)));

    connect(TelldusCore::Instance(),SIGNAL(SensorEvent(QStringList)),this,SLOT(SensorEvent(QStringList)));
    connect(TelldusCore::Instance(),SIGNAL(DeviceEvent(QList<int>)),this,SLOT(DeviceEvent(QList<int>)));

    connect(tcpServer,SIGNAL(TelegramReceived(QStringList)),this,SLOT(ProcessIncomingTelegram(QStringList)));

    // TEST
    //int id, QString &name, QString &protocol, QString &model, QString &paramHouse, QString &paramUnit, QString &type, int methodsSupported, int lastSentCommand
    QString name = "Stikk";
    QString protocol = "arctech";
    QString model = "selflearning-switch:nexa";
    QString house = "123456";
    QString unit = "1";
    QString type = "receiver";

    Device Stikk(name,protocol,model,house,unit,type);

    name = "Dimmer";
    protocol = "arctech";
    model = "selflearning-switch:nexa";
    house = "123457";
    unit = "1";
    type = "receiver";

    Device Dimmer(name,protocol,model,house,unit,type);

    name = "Switch";
    protocol = "arctech";
    model = "selflearning-switch:nexa";
    house = "15327318";
    unit = "12";
    type = "sender";

    Device Switch(name,protocol,model,house,unit,type);

    deviceList.append(Stikk);
    deviceList.append(Dimmer);
    deviceList.append(Switch);

    // TEST END



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


Manager::~Manager()
{

}

void Manager::RawEvent(QStringList eventList)
{
    // Log if enabled

    // Forward raw events if client is looking for new switches and sensors
    qDebug() << eventList;

}

void Manager::DeviceEvent(QList<int> param)
{
    foreach(Device dev, deviceList)
    {
        int id = dev.GetId();

        if(id == param.at(0))
        {
            dev.SetLastCommandSent(param.at(1));
            if(dev.IsDimmer())
            {
                dev.SetValue(param.at(2));
            }

            qDebug() << "Device " << dev.GetName() << ", Id " << QString::number(dev.GetId())
                     << ", type " << dev.GetType() << " is now " << (QString)dev.GetLastCommandSent();
            ProcessDeviceEvents(dev);
            return;
        }
    }
}

void Manager::SensorEvent(QStringList param)
{
    // if id in ignore list, return

    foreach(Sensor sens, sensorList)
    {
        int id = sens.GetId();
        int dataType = sens.GetDataType();

        if(id == param.at(0).toInt() && dataType == param.at(1).toInt())
        {
            sens.SetValue(param.at(2));
            // Emit sensor changed
            qDebug() << "Sensor " << sens.GetName() << ", Id " << QString::number(sens.GetId())
                     << ", type " << sens.GetDataTypeText() << " is now " << sens.GetValue();
            ProcessSensorEvents(sens);
            return;
        }
    }

    // Else new sensor discovered
    int id = param.at(0).toInt();
    int dataType = param.at(1).toInt();
    QString value = param.at(2);
    QString protocol = param.at(3);
    QString model = param.at(4);

    Sensor sens(id,protocol,model,dataType,value);
    sensorList.append(sens);
    // Emit new sensor found
    qDebug() << "Found Sensor id " << QString::number(id) << "model " << model;

}

void Manager::ProcessDeviceEvents(Device &dev)
{

}

void Manager::ProcessSensorEvents(Sensor &sens)
{

}

void Manager::ProcessIncomingTelegram(QStringList telegram)
{
    Device dev = deviceList.at(0);
    tDCore->DeviceTurnOn(dev);
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
    out.setVersion(QDataStream::Qt_5_0);

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
    in.setVersion(QDataStream::Qt_5_0);
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

/*
 * master,id:,param:,value:,condition:,slave,id:,param:,value:,slave,id:,param:,value:,slave,id:,param:,value:
 * master,id:,param:,value:,condition:,slave,id:,param:,value:,slave,id:,param:,value:,slave,id:,param:,value:
 * master,id:,param:,value:,condition:,slave,id:,param:,value:,slave,id:,param:,value:,slave,id:,param:,value:
 * master,id:,param:,value:,condition:,slave,id:,param:,value:,slave,id:,param:,value:,slave,id:,param:,value:
*/

QString Manager::SaveSchedule()
{

}

QString Manager::LoadSchedule()
{
    QString testStringOn = "master,id:3,param:12,value:1,condition:=,slave,id:1,param:1,value:,slave,id:2,param:16,value:50";
    QString testStringOff = "master,id:3,param:12,value:2,condition:=,slave,id:1,param:2,value:,slave,id:2,param:16,value:0";
    QStringList list;
    list << testStringOn << testStringOff;
}
