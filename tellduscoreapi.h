#ifndef TELLDUSCOREAPI_H
#define TELLDUSCOREAPI_H

#include <QObject>
#include <QTime>
#include "telldus-core.h"
#include "device.h"
#include "singleton.h"

class TelldusCoreAPI : public QObject
{
    Q_OBJECT
public:
    explicit TelldusCoreAPI(QObject *parent = 0);
    ~TelldusCoreAPI();

    enum CommandTypes
    {
        TurnOn = 1,
        TurnOff = 2,
        Bell = 4,
        Toggle = 8,
        Dim = 16,
        Learn = 32,
        Execute = 64,
        Up = 128,
        Down = 256,
        Stop = 512
    };

    QString ClearAllDevicesInCore();
    QString RegisterNewDevice(Devices &dev);
    QList<Devices> LookForSensors(void);

    QString RegisterNewSensor(Devices &dev);
    QString DeviceCommand(int id, int command, int value);



signals:
    void RawDataEvent(QStringList param);
    void ControllerEvent(QStringList param);
    void SensorEvent(const char *protocol, const char *model, int id, int dataType, const char *value);
    void DeviceEvent(int id, int command, const char *data);
    void DeviceChangeEvent(QStringList param);


private slots:

public slots:



private:

    QString ErrorCode(int id, int code);

    static void WINAPI RawDataEventCallback(const char *data, int controllerId, int, void *);
    static void WINAPI ControllerEventCallback(int controllerId, int changeEvent, int changeType, const char *newValue, int callbackId, void *);
    static void WINAPI SensorEventCallback(const char *protocol, const char *model, int sensorId, int dataType, const char *value, int, int, void *);
    static void WINAPI DeviceEventCallback(int deviceId, int method, const char *data, int, void *);
    static void WINAPI DeviceChangeEventCallback(int deviceId, int changeEvent, int changeType, int, void *);

    int rawDataEventId, sensorEventId, deviceEventId, deviceChangedEventId, controllerEventId;
};

//Global variable
typedef Singleton<TelldusCoreAPI> TelldusCore;

#endif // TELLDUSCOREAPI_H
