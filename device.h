#ifndef DEVICE_H
#define DEVICE_H

#include <QtCore>

class Device
{
public:
    Device();
    Device(int id, QString &name, QString &protocol, QString &model, QString &paramHouse, QString &paramUnit, QString &type, int methodsSupported,int lastSentCommand);
    Device(const Device &other);

    ~Device();

    void SetId(const int &id) {this->id = id;}
    int GetId() const {return id;}

    void SetName(const QString &name) {this->name = name;}
    QString GetName() const {return name;}

    void SetProtocol(const QString &protocol) {this->protocol = protocol;}
    QString GetProtocol() const {return protocol;}

    void SetModel(const QString &model) {this->model = model;}
    QString GetModel() const {return model;}

    void SetParamHouse(const QString &paramHouse) {this->paramHouse = paramHouse;}
    QString GetParamHouse() const {return paramHouse;}

    void SetParamUnit(const QString &paramUnit) {this->paramUnit = paramUnit;}
    QString GetParamUnit() const {return paramUnit;}

    void SetType(const QString &type) {this->type = type;}
    QString GetType() const {return type;}

    void SetMethodsSupported(const int &methodsSupported) {this->methodsSupported = methodsSupported;}
    int GetMethodsSupported() const {return methodsSupported;}

    void SetLastCommandSent(const int &lastSentCommand) {this->lastSentCommand = lastSentCommand;}
    int GetLastCommandSent() const {return lastSentCommand;}

private:
    int id;
    QString name;
    QString protocol;
    QString model;
    QString paramHouse;
    QString paramUnit;
    QString type;
    int methodsSupported;
    int lastSentCommand;
};

QDataStream &operator<<(QDataStream &out, const Device &device);
QDataStream &operator>>(QDataStream &in, Device &device);

#endif // DEVICE_H
