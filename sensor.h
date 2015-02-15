#ifndef SENSOR_H
#define SENSOR_H

#include <QtCore>

class Sensor
{
public:
    Sensor();
    Sensor(int id, QString &name, QString &protocol, QString &model, int dataType, QString &value);
    Sensor(const Sensor &other);

    ~Sensor();

    void SetId(const int &id) {this->id = id;}
    int GetId() const {return id;}

    void SetName(const QString &name) {this->name = name;}
    QString GetName() const {return name;}

    void SetProtocol(const QString &protocol) {this->protocol = protocol;}
    QString GetProtocol() const {return protocol;}

    void SetModel(const QString &model) {this->model = model;}
    QString GetModel() const {return model;}

    void GetDataType(const int &dataType) {this->dataType = dataType;}
    int GetDataType() const {return dataType;}

    void SetValue(const QString &value) {this->value = value;}
    QString GetValue() const {return value;}

private:
    int id;
    QString name;
    QString protocol;
    QString model;
    int dataType;
    QString value;
};

QDataStream &operator<<(QDataStream &out, const Sensor &device);
QDataStream &operator>>(QDataStream &in, Sensor &device);

#endif // SENSOR_H
