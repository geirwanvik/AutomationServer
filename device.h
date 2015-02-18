#ifndef DEVICES
#define DEVICES

#include <QtCore>

class Devices
{
public:

    Devices();
    Devices(int id, QString &name, QString &protocol, QString &model, QString &house, QString &unit,
            QString &type, int supportedCommands,int lastCommand, int lastValue);
    Devices(QString &name, QString &protocol, QString &model, QString &house, QString &unit,QString &type);
    Devices(const Devices &other);
    ~Devices();

    void SetId(const int &id) {this->id = id;}
    int GetId() const {return id;}

    void SetName(const QString &name) {this->name = name;}
    QString GetName() const {return name;}

    void SetProtocol(const QString &protocol) {this->protocol = protocol;}
    QString GetProtocol() const {return protocol;}

    void SetModel(const QString &model) {this->model = model;}
    QString GetModel() const {return model;}

    void SetHouse(const QString &house) {this->house = house;}
    QString GetHouse() const {return house;}

    void SetUnit(const QString &unit) {this->unit = unit;}
    QString GetUnit() const {return unit;}

    void SetType(const QString &type) {this->type = type;}
    QString GetType() const {return type;}

    void SetSupportedCommands(const int &methodsSupported) {this->supportedCommands = methodsSupported;}
    int GetSupportedCommands() const {return supportedCommands;}

    void SetLastCommand(const int &lastSentCommand) {this->lastCommand = lastSentCommand;}
    int GetLastCommand() const {return lastCommand;}
    QString GetLastCommandText();

    void SetLastValue(const int &value) {this->lastValue = value;}
    int GetLastValue() const {return lastValue;}


private:


    int id;
    QString name;
    QString protocol;
    QString model;
    QString house;
    QString unit;
    QString type;
    int supportedCommands;
    int lastCommand;
    int lastValue;
};


QDataStream &operator<<(QDataStream &out, const Devices &device);
QDataStream &operator>>(QDataStream &in, Devices &device);
#endif // DEVICE

