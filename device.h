#ifndef DEVICES
#define DEVICES

#include <QtCore>
 #include <QTranslator>

#include "common.h"

class Device
{
public:

    Device();
    Device(int id, QString &name, QString &protocol, QString &model, QString &house, QString &unit,
            QString &type,QString &subType, int supportedCommands,int lastCommand, int lastValue);
    Device(QString &name, QString &protocol, QString &model, QString &house, QString &unit,QString &type,QString &subType);
    Device(const Device &other);
    ~Device();

    void setId(const int &id) {this->id = id;}
    int getId() const {return id;}

    void setName(const QString &name) {this->name = name;}
    QString getName() const {return name;}

    void setProtocol(const QString &protocol) {this->protocol = protocol;}
    QString getProtocol() const {return protocol;}

    void setModel(const QString &model) {this->model = model;}
    QString getModel() const {return model;}

    void setHouse(const QString &house) {this->house = house;}
    QString getHouse() const {return house;}

    void setUnit(const QString &unit) {this->unit = unit;}
    QString getUnit() const {return unit;}

    void setType(const QString &type) {this->type = type;}
    QString getType() const {return type;}

    void setSupportedCommands(const int &methodsSupported) {this->supportedCommands = methodsSupported;}
    int getSupportedCommands() const {return supportedCommands;}

    void setLastCommand(const int &lastSentCommand) {this->lastCommand = lastSentCommand;}
    int getLastCommand() const {return lastCommand;}
    QString getLastCommandText();

    void setLastValue(const int &value) {this->lastValue = value;}
    int getLastValue() const {return lastValue;}

    void setSubType(const QString &subType) {this->subType = subType;}
    QString getSubType() const {return subType;}


private:


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
    int lastValue;
};


QDataStream &operator<<(QDataStream &out, const Device &device);
QDataStream &operator>>(QDataStream &in, Device &device);
#endif // DEVICE
