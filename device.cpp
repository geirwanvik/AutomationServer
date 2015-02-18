#include "device.h"

Devices::Devices()
{
    this->id = 0;
    this->name = "Not Set";
    this->protocol = "Not Set";
    this->model = "Not Set";
    this->house = "Not Set";
    this->unit = "Not Set";
    this->type = "Not Set";
    this->supportedCommands = 0;
    this->lastCommand = 0;
    this->lastValue = 0;
}

Devices::Devices(int id, QString &name, QString &protocol, QString &model, QString &house, QString &unit,
               QString &type, int supportedCommands, int lastCommand, int value)
{
    this->id = id;
    this->name = name;
    this->protocol = protocol;
    this->model = model;
    this->house = house;
    this->unit = unit;
    this->type = type;
    this->supportedCommands = supportedCommands;
    this->lastCommand = lastCommand;
    this->lastValue = value;
}

Devices::Devices(QString &name, QString &protocol, QString &model, QString &paramHouse, QString &paramUnit,
               QString &type)
{
    this->id = 0;
    this->name = name;
    this->protocol = protocol;
    this->model = model;
    this->house = paramHouse;
    this->unit = paramUnit;
    this->type = type;
    this->supportedCommands = 0;
    this->lastCommand = 0;
    this->lastValue = 0;
}

Devices::Devices(const Devices &other)
{
    this->id = other.id;
    this->name = other.name;
    this->protocol = other.protocol;
    this->model = other.model;
    this->house = other.house;
    this->unit = other.unit;
    this->type = other.type;
    this->supportedCommands = other.supportedCommands;
    this->lastCommand = other.lastCommand;
    this->lastValue = other.lastValue;
}

Devices::~Devices()
{

}

QString Devices::GetLastCommandText()
{
    QString msg;
    if(lastCommand == 1)
    {
        msg = "On";
    }
    else if(lastCommand == 2)
    {
        msg = "Off";
    }
    return msg;
}


QDataStream &operator<<(QDataStream &out, const Devices &device)
{
    out << device.GetId()
        << device.GetName()
        << device.GetProtocol()
        << device.GetModel()
        << device.GetHouse()
        << device.GetUnit()
        << device.GetType()
        << device.GetSupportedCommands()
        << device.GetLastCommand()
        << device.GetLastValue();

    return out;
}

QDataStream &operator>>(QDataStream &in, Devices &device)
{
    int id;
    QString name;
    QString protocol;
    QString model;
    QString house;
    QString unit;
    QString type;
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
       >> supportedCommands
       >> lastCommand
       >> value;

    device = Devices(id,name,protocol,model,house,unit,type,supportedCommands,lastCommand,value);
    return in;
}

