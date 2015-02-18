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

    //connect(TelldusCore::Instance(),SIGNAL(SensorEvent(const char*,const char*,int,int,const char*)),
    //        this,SLOT(SensorEvent(const char*,const char*,int,int,const char*)));
    connect(TelldusCore::Instance(),SIGNAL(DeviceEvent(int,int,const char*)),
            this,SLOT(DeviceEvent(int,int,const char*)));

    connect(tcpServer,SIGNAL(TelegramReceived(QStringList)),this,SLOT(ProcessIncomingTelegram(QStringList)));


    // TEST
    QString name = "Stikk";
    QString protocol = "arctech";
    QString model = "selflearning-switch:nexa";
    QString house = "123456";
    QString unit = "1";
    QString type = "switch";

    Devices Stikk(name,protocol,model,house,unit,type);

    name = "Dimmer";
    protocol = "arctech";
    model = "selflearning-dimmer:nexa";
    house = "123457";
    unit = "1";
    type = "dimmer";

    Devices Dimmer(name,protocol,model,house,unit,type);

    name = "Switch_1.1";
    protocol = "arctech";
    model = "selflearning-switch:nexa";
    house = "15327318";
    unit = "11";
    type = "switch";

    Devices Switch(name,protocol,model,house,unit,type);

    name = "Switch_1.2";
    protocol = "arctech";
    model = "selflearning-switch:nexa";
    house = "15327318";
    unit = "12";
    type = "switch";

    Devices Switch2(name,protocol,model,house,unit,type);

    deviceList.append(Stikk);
    deviceList.append(Dimmer);
    deviceList.append(Switch);
    deviceList.append(Switch2);

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
    QList<Devices>::iterator i;
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
    LoadSchedule();
}


Manager::~Manager()
{

}

void Manager::RawEvent(QStringList eventList)
{
    // Log if enabled

    // Forward raw events if client is looking for new switches and sensors
    //qDebug() << eventList;

}

void Manager::DeviceEvent(int eventId, int eventCommand, const char *eventData)
{
    int eventValue = atoi(eventData);

    foreach(Devices dev, deviceList)
    {
        int id = dev.GetId();

        if(id == eventId)
        {
            dev.SetLastCommand(eventCommand);
            dev.SetLastValue(eventValue);

            foreach(DataBaseItem *item,schedulerList)
            {
                int triggerId = item->getMasterId();
                if(triggerId == eventId)
                {
                    item->CompareToTrigger(eventCommand,eventValue);
                }
            }

            qDebug() << "Device " << dev.GetName() << ", Id " << QString::number(dev.GetId())
                     << ", type " << dev.GetType() << " is now " << (QString)dev.GetLastCommand() << "value " << dev.GetLastValue();
            return;
        }
    }
}


void Manager::ProcessIncomingTelegram(QStringList telegram)
{
    qDebug() << telegram;
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

    QList<Devices>::iterator i;
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
        Devices tempDev;
        in >> tempDev;
        deviceList.append(tempDev);
    }

    file.close();
    return ("Success");
}


QString Manager::SaveSchedule()
{

}

QString Manager::LoadSchedule()
{
    QString testStringOn = "master,3,1,0,=,slave,1,1,0,slave,2,16,255";
    QString testStringOff = "master,3,2,0,=,slave,1,2,0,slave,2,16,0";
    QStringList list;
    list << testStringOn << testStringOff;

    foreach(QString string, list)
    {
        DataBaseItem *newItem = new DataBaseItem;
        newItem->ParseListString(string);
        connect(newItem,SIGNAL(SlaveSignal(int,int,int)),tDCore,SLOT(DeviceCommand(int,int,int)));
        schedulerList.append(newItem);
    }
    return "Success";

}
