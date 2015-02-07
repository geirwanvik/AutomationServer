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

signals:

public slots:

private:
    static void WINAPI RawDataEvent(const char *data, int controllerId, int callbackId, void *context);
    static void WINAPI ControllerEvent(int controllerId, int changeEvent, int changeType, const char *newValue, int callbackId, void *context);
    static void WINAPI SensorEvent(const char *protocol, const char *model, int sensorId, int dataType, const char *value, int ts, int callbackId, void *context);
    static void WINAPI DeviceEvent(int deviceId, int method, const char *data, int callbackId, void *context);
    static void WINAPI DeviceChangeEvent(int deviceId, int changeEvent, int changeType, int callbackId, void *context);

    int rawDataEventId, sensorEventId, deviceEventId, deviceChangedEventId, controllerEventId;

    QTime *timeout;

};

//Global variable
typedef Singleton<TelldusCoreAPI> TelldusCore;

#endif // TELLDUSCOREAPI_H
