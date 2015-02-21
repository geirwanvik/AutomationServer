#ifndef TELLSTICK_H
#define TELLSTICK_H

#include <QObject>
#include <QTime>
#include "telldus-core.h"
#include "device.h"
#include "singleton.h"
#include "common.h"

class Tellstick : public QObject
{
    Q_OBJECT
public:
    explicit Tellstick(QObject *parent = 0);
    ~Tellstick();

    void registerNewDevice(Device &dev);

signals:
    void deviceEvent(int eventId, int eventCommand, int eventValue, QString deviceType);
    void logEvent(QString msg);


private slots:

public slots:
    void deviceCommand(int id, int command, int value);

private:
    void errorCode(int id, int code);
    void removePreviousConfig();
    void registerSensor(Device &dev);
    void registerSwitch(Device &dev);
    void removeSwitch(Device &dev);

    static void WINAPI rawDataEventCallback(const char *data, int controllerId, int, void *);
    static void WINAPI controllerEventCallback(int controllerId, int changeEvent, int changeType, const char *newValue, int callbackId, void *);
    static void WINAPI sensorEventCallback(const char *protocol, const char *model, int sensorId, int dataType, const char *value, int, int, void *);
    static void WINAPI deviceEventCallback(int deviceId, int method, const char *data, int, void *);
    static void WINAPI deviceChangeEventCallback(int deviceId, int changeEvent, int changeType, int, void *);

    int rawDataEventId, sensorEventId, deviceEventId, deviceChangedEventId, controllerEventId;
};

//Global variable
typedef Singleton<Tellstick> TellstickSingleton;

#endif // TELLDUSCOREAPI_H
