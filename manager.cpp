#include "manager.h"

Manager::Manager(QObject *parent) : QObject(parent)
{
    tellstick = new Tellstick(this);
    tcpServer = new Server(this);

    // Singleton instance of TelldusCoreAPI to forward events from the static callback functions
    connect(TellstickSingleton::Instance(),SIGNAL(deviceEvent(int,int,int,QString)),
            this,SLOT(deviceEvent(int,int,int,QString)));

    // Logging
    connect(TellstickSingleton::Instance(),SIGNAL(logEvent(QString)), this, SLOT(logEvent(QString)));
    connect(tcpServer,SIGNAL(logEvent(QString)),this,SLOT(logEvent(QString)));

    // Signals from tcp server
    connect(tcpServer,SIGNAL(toggleLookForDevices(QString)),this,SLOT(toggleLookForNewDevices(QString)));
    connect(tcpServer,SIGNAL(readAllDevices()),this,SLOT(readAllDevices()));
    connect(tcpServer,SIGNAL(writeAllDevices(QDataStream&,int)),this,SLOT(writeAllDevices(QDataStream&,int)));


    lookForNewDevices = false;
/*
    // TEST
    QString name = "Stikk";
    QString protocol = "arctech";
    QString model = "selflearning-switch:nexa";
    QString house = "123456";
    QString unit = "1";
    QString type = "switch";
    QString subType = "addon";

    Device Stikk(name,protocol,model,house,unit,type,subType);

    name = "Dimmer";
    protocol = "arctech";
    model = "selflearning-dimmer:nexa";
    house = "123457";
    unit = "1";
    type = "switch";
    subType = "something";

    Device Dimmer(name,protocol,model,house,unit,type,subType);

    name = "Switch_1.1";
    protocol = "arctech";
    model = "selflearning-switch:nexa";
    house = "15327318";
    unit = "11";
    type = "switch";
    subType = "vimana";

    Device Switch(name,protocol,model,house,unit,type,subType);

    name = "Switch_1.2";
    protocol = "arctech";
    model = "selflearning-switch:nexa";
    house = "15327318";
    unit = "12";
    type = "switch";
    subType = "zwei";

    Device Switch2(name,protocol,model,house,unit,type,subType);

    deviceList.append(Stikk);
    deviceList.append(Dimmer);
    deviceList.append(Switch);
    deviceList.append(Switch2);

    saveConfig();

    // TEST END
*/


    // Check if config file exists, else create it empty.
    bool success = loadConfig();
    if(!success)
    {
        emit logEvent(tr("Config file not found, creating new"));
        success = saveConfig();
    }
    if(!success) // Save also failed
    {
        emit logEvent(tr("Could not create config file on disk, changes will not be saved"));
    }

    loadSchedule();
}


Manager::~Manager()
{

}

void Manager::logEvent(QString event)
{
    // Log if enabled

    // Forward raw events if client is looking for new switches and sensors
    //qDebug() << event;

}

void Manager::deviceEvent(int eventId, int eventCommand, int eventValue, QString deviceType)
{
    foreach(Device dev, deviceList)
    {
        int id = dev.getId();

        if(id == eventId)
        {
            dev.setLastCommand(eventCommand);
            dev.setLastValue(eventValue);
            qDebug() << "Device " << deviceType << " id " << eventId << " command " << eventCommand << " value " << eventValue;
            return;
        }
    }
    // A new sensor will not exist in list before registered in client
    if(lookForNewDevices)
    {
        newDeviceFound(eventId, eventCommand, eventValue, deviceType);
    }
}

void Manager::toggleLookForNewDevices(QString cmd)
{
    if(cmd == "on")
    {
        lookForNewDevices = true;
    }
    if(cmd == "off")
    {
        lookForNewDevices = false;
    }
}

void Manager::readAllDevices()
{
    QStringList list;
    int elements = 0;
    foreach (Device dev, deviceList)
    {
        list << dev.getName();
        list << dev.getType();
        list << dev.getSubType();
        list << dev.getLastCommandText();
        elements++;
    }
    tcpServer->sendData(list,elements,'A','W');
}

void Manager::writeAllDevices(QDataStream &in, int elements)
{

}

void Manager::newDeviceFound(int eventId, int eventCommand, int eventValue, QString deviceType)
{

}


bool Manager::saveConfig()
{
    QString path = QCoreApplication::applicationDirPath();
    path.append("/config.dat");
    QFile file(path);

    if(!file.open(QIODevice::WriteOnly))
    {
        emit logEvent(tr("Cannot open file for writing: %1").arg(file.errorString()));
        return false;
    }

    QDataStream out(&file);
    out.setVersion(QDataStream::Qt_5_3);

    QList<Device>::iterator i;
    for(i = deviceList.begin(); i != deviceList.end(); i++)
    {
        out << (*i);
    }

    file.close();
    return true;
}

bool Manager::loadConfig()
{
    QString path = QCoreApplication::applicationDirPath();
    path.append("/config.dat");
    QFile file(path);

    if(!file.open(QIODevice::ReadOnly))
    {
        emit logEvent(tr("Cannot open file for reading: %1").arg(file.errorString()));
        return false;
    }

    QDataStream in(&file);
    in.setVersion(QDataStream::Qt_5_3);

    deviceList.clear();

    while(!in.atEnd()) // Create the device list and register devices in telldus core
    {
        Device tempDev;
        in >> tempDev;
        tellstick->registerNewDevice(tempDev);
        deviceList.append(tempDev);
    }

    file.close();
    return true;
}


bool Manager::saveSchedule()
{
    return true;
}

bool Manager::loadSchedule()
{
    /*
    QString testStringOn = "master,3,1,0,=,slave,1,1,0,slave,2,16,255";
    QString testStringOff = "master,3,2,0,=,slave,1,2,0,slave,2,16,0";
    QStringList list;
    list << testStringOn << testStringOff;

    foreach(QString string, list)
    {
        DataBaseItem *newItem = new DataBaseItem;
        newItem->ParseListString(string);
        connect(newItem,SIGNAL(SlaveSignal(int,int,int)),tDCore,SLOT(deviceCommand(int,int,int)));
        schedulerList.append(newItem);
    }
    */
    return true;

}
