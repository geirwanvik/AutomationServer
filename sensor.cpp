#include "sensor.h"

Sensor::Sensor()
{
    this->id = 0;
    this->name = "Not Set";
    this->protocol = "Not Set";
    this->model = "Not Set";
    this->dataType = 0;
    this->value = "Not Set";
}

Sensor::Sensor(int id, QString &name, QString &protocol, QString &model, int dataType, QString &value)
{
    this->id = id;
    this->name = name;
    this->protocol = protocol;
    this->model = model;
    this->dataType = dataType;
    this->value = value;
}

Sensor::Sensor(int id, QString &protocol, QString &model, int dataType, QString &value)
{
    this->id = id;
    this->name = "Not Set";
    this->protocol = protocol;
    this->model = model;
    this->dataType = dataType;
    this->value = value;
}

Sensor::Sensor(const Sensor &other)
{
    this->id = other.id;
    this->name = other.name;
    this->protocol = other.protocol;
    this->model = other.model;
    this->dataType = other.dataType;
    this->value = other.value;
}

Sensor::~Sensor()
{

}

QString Sensor::GetDataTypeText()
{
    if(dataType == 1)
    {
        return "Temperature";
    }
    else if(dataType == 2)
    {
        return "Humidity";
    }
    return "Unknown";
}

QString Sensor::GetValue() const
{

    if(dataType == 1)
    {
        return (value + QString(" degC"));
    }
    else if(dataType == 2)
    {
        return value + (QString)(" %RH");
    }
    return value;
}

QDataStream &operator<<(QDataStream &out, const Sensor &sensor)
{
    out << sensor.GetId()
        << sensor.GetName()
        << sensor.GetProtocol()
        << sensor.GetModel()
        << sensor.GetDataType()
        << sensor.GetValue();

    return out;
}

QDataStream &operator>>(QDataStream &in, Sensor &sensor)
{
    int id;
    QString name;
    QString protocol;
    QString model;
    int dataType;
    QString value;

    in >> id
       >> name
       >> protocol
       >> model
       >> dataType
       >> value;

    sensor = Sensor(id,name,protocol,model,dataType,value);
    return in;
}

