#include "device.h"

Device::Device()
{
    this->id = 0;
    this->name = "Not Set";
    this->protocol = "Not Set";
    this->model = "Not Set";
    this->paramHouse = "Not Set";
    this->paramUnit = "Not Set";
    this->type = "Not Set";
    this->methodsSupported = 0;
    this->lastSentCommand = 0;
    this->value = 0;
    this->dimmer = false;
}

Device::Device(int id, QString &name, QString &protocol, QString &model, QString &paramHouse, QString &paramUnit,
               QString &type, int methodsSupported, int lastSentCommand, int value)
{
    this->id = id;
    this->name = name;
    this->protocol = protocol;
    this->model = model;
    this->paramHouse = paramHouse;
    this->paramUnit = paramUnit;
    this->type = type;
    this->methodsSupported = methodsSupported;
    this->lastSentCommand = lastSentCommand;
    this->value = value;
    this->dimmer = dimmer;
}

Device::Device(QString &name, QString &protocol, QString &model, QString &paramHouse, QString &paramUnit,
               QString &type)
{
    this->id = 0;
    this->name = name;
    this->protocol = protocol;
    this->model = model;
    this->paramHouse = paramHouse;
    this->paramUnit = paramUnit;
    this->type = type;
    this->methodsSupported = 0;
    this->lastSentCommand = 0;
    this->value = 0;
    this->dimmer = false;
}

Device::Device(const Device &other)
{
    this->id = other.id;
    this->name = other.name;
    this->protocol = other.protocol;
    this->model = other.model;
    this->paramHouse = other.paramHouse;
    this->paramUnit = other.paramUnit;
    this->type = other.type;
    this->methodsSupported = other.methodsSupported;
    this->lastSentCommand = other.lastSentCommand;
    this->value = other.value;
    this->dimmer = other.dimmer;
}

Device::~Device()
{

}

QString Device::GetLastCommandSent()
{
    QString msg;
    if(lastSentCommand == 1)
    {
        msg = "On";
    }
    else if(lastSentCommand == 2)
    {
        msg = "Off";
    }
    return msg;
}

QDataStream &operator<<(QDataStream &out, const Device &device)
{
    out << device.GetId()
        << device.GetName()
        << device.GetProtocol()
        << device.GetModel()
        << device.GetParamHouse()
        << device.GetParamUnit()
        << device.GetType()
        << device.GetMethodsSupported()
        << device.GetLastCommandSent()
        << device.GetValue();

    return out;
}

QDataStream &operator>>(QDataStream &in, Device &device)
{
    int id;
    QString name;
    QString protocol;
    QString model;
    QString paramHouse;
    QString paramUnit;
    QString type;
    int methodsSupported;
    int lastSentCommand;
    int value;

    in >> id
       >> name
       >> protocol
       >> model
       >> paramHouse
       >> paramUnit
       >> type
       >> methodsSupported
       >> lastSentCommand
       >> value;

    device = Device(id,name,protocol,model,paramHouse,paramUnit,type,methodsSupported,lastSentCommand,value);
    return in;
}

