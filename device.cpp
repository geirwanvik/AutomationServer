#include "device.h"

Device::Device()
{
    this->id = 0;
    this->name = "Not Set";
    this->protocol = "Not Set";
    this->model = "Not Set";
    this->house = "Not Set";
    this->unit = "Not Set";
    this->type = "Not Set";
    this->subType = "Not Set";
    this->supportedCommands = 0;
    this->lastCommand = 0;
    this->lastValue = 0;
}

Device::Device(int id, QString &name, QString &protocol, QString &model, QString &house, QString &unit,
               QString &type,QString &subType, int supportedCommands, int lastCommand, int value)
{
    this->id = id;
    this->name = name;
    this->protocol = protocol;
    this->model = model;
    this->house = house;
    this->unit = unit;
    this->type = type;
    this->subType = subType;
    this->supportedCommands = supportedCommands;
    this->lastCommand = lastCommand;
    this->lastValue = value;
}

Device::Device(QString &name, QString &protocol, QString &model, QString &paramHouse, QString &paramUnit,
               QString &type, QString &subType)
{
    this->id = 0;
    this->name = name;
    this->protocol = protocol;
    this->model = model;
    this->house = paramHouse;
    this->unit = paramUnit;
    this->type = type;
    this->subType = subType;
    this->supportedCommands = 0;
    this->lastCommand = 0;
    this->lastValue = 0;
}

Device::Device(const Device &other)
{
    this->id = other.id;
    this->name = other.name;
    this->protocol = other.protocol;
    this->model = other.model;
    this->house = other.house;
    this->unit = other.unit;
    this->type = other.type;
    this->subType = other.subType;
    this->supportedCommands = other.supportedCommands;
    this->lastCommand = other.lastCommand;
    this->lastValue = other.lastValue;
}

Device::~Device()
{

}

QString Device::getLastCommandText()
{
    if(type == "sensor")
    {
        switch(lastCommand)
        {
        case Temperature:
            return (QObject::tr("temperature:%1degC").arg(lastValue));
        case Humidity:
            return (QObject::tr("humidity:%1%RH").arg(lastValue));
        }
    }
    if(type == "switch")
    {
        switch(lastCommand)
        {
        case TurnOn:
            return (QObject::tr("on"));
        case TurnOff:
            return (QObject::tr("off"));
        case Dim:
            return (QObject::tr("dim:%1%").arg(lastValue));
        case Learn:
            return (QObject::tr("learn"));
        }
    }
    return (QObject::tr("Command not found"));
}

QDataStream &operator<<(QDataStream &out, const Device &device)
{
    out << device.getId()
        << device.getName()
        << device.getProtocol()
        << device.getModel()
        << device.getHouse()
        << device.getUnit()
        << device.getType()
        << device.getSubType()
        << device.getSupportedCommands()
        << device.getLastCommand()
        << device.getLastValue();

    return out;
}

QDataStream &operator>>(QDataStream &in, Device &device)
{
    int id;
    QString name;
    QString protocol;
    QString model;
    QString house;
    QString unit;
    QString type;
    QString subType;
    int supportedCommands;
    int lastCommand;
    int value;

    in >> id
       >> name
       >> protocol
       >> model
       >> house
       >> unit
       >> type
       >> subType
       >> supportedCommands
       >> lastCommand
       >> value;

    device = Device(id,name,protocol,model,house,unit,type,subType,supportedCommands,lastCommand,value);
    return in;
}
