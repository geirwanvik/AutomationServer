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

    int GetNumberOfDevicesInCore();
    QString ClearAllDevicesInCore();
    QString RegisterNewDevice(Device &dev);

    void EnableRawDataEvent(void);
    void DisableRawDataEvent(void);
    void EnableSensorEvent(void);
    void DisableSensorEvent(void);
    void EnableDeviceEvent(void);
    void DisableDeviceEvent(void);
    void EnableDeviceChangeEvent(void);
    void DisableDeviceChangeEvent(void);
    void EnableControllerEvent(void);
    void DisableControllerEvent(void);

    QString DeviceTurnOn(Device &dev);
    QString DeviceTurnOff(Device &dev);

signals:
    void RawDataEvent(QStringList param);
    void ControllerEvent(QStringList param);
    void SensorEvent(QStringList param);
    void DeviceEvent(QStringList param);
    void DeviceChangeEvent(QStringList param);


private slots:

public slots:



private:
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
