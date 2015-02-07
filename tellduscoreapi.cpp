#include "tellduscoreapi.h"
#include <QDebug>

const int DATA_LENGTH = 20;

TelldusCoreAPI::TelldusCoreAPI(QObject *parent) : QObject(parent)
{
    timeout = new QTime;
    tdInit();
    EnableDeviceChangeEvent();
}

TelldusCoreAPI::~TelldusCoreAPI()
{
    tdClose();
}

int TelldusCoreAPI::GetNumberOfDevicesInCore()
{
    return (tdGetNumberOfDevices());
}

QString TelldusCoreAPI::ClearAllDevicesInCore()
{
    bool success;
    QString returnMsg;
    QByteArray devList;
    QEventLoop loop;


    int numberOfDevices = tdGetNumberOfDevices();

    for (int i = 0; i < numberOfDevices; i++)
    {
      int id = tdGetDeviceId(i);
      devList.append(id);
    }

    for(int i = 0; i<devList.size(); i++)
    {
      success = tdRemoveDevice(devList.at(i));
      if(!success)
      {
          returnMsg.append("Failed to remove dev " + devList.at(i));
      }
    }

    timeout->start();

    while(tdGetNumberOfDevices() != 0 && timeout->elapsed() < 5000)
    {
        // Wait for telldusd to update or 5 sec
    }

    if(returnMsg.isEmpty())
    {
        returnMsg.append("Success");
    }
    return returnMsg;
}

QString TelldusCoreAPI::RegisterNewDevice(Device &dev)
{
    bool success;
    QString returnMsg;

    // Get the device id from telldusd
    int id = tdAddDevice();
    if(id < 0)
    {
        return ("Failed to create new device, error code " + id);
    }
    dev.SetId(id);

    // Set everything else
    success = tdSetName(id,dev.GetName().toUtf8().constData());
    if(!success)
    {
        returnMsg.append("Failed to set name.");
    }

    success = tdSetProtocol(id,dev.GetProtocol().toUtf8().constData());
    if(!success)
    {
        returnMsg.append("Failed to set protocol.");
    }

    success = tdSetModel(id,dev.GetModel().toUtf8().constData());
    if(!success)
    {
        returnMsg.append("Failed to set model.");
    }

    // If the model is codeswitch or selflearning-switch, house and unit parameter must also be set
    // If the model is bell, house parameter must be set.
    if(dev.GetModel().startsWith("bell"))
    {
        success = tdSetDeviceParameter(id,"house",dev.GetParamHouse().toUtf8().constData());
        if(!success)
        {
            returnMsg.append("Failed to set param:house.");
        }

    }
    else if(dev.GetModel().startsWith("codeswitch") || dev.GetModel().startsWith("selflearning-switch"))
    {
        success = tdSetDeviceParameter(id,"house",dev.GetParamHouse().toUtf8().constData());
        if(!success)
        {
            returnMsg.append("Failed to set param:house.");
        }
        success = tdSetDeviceParameter(id,"unit",dev.GetParamUnit().toUtf8().constData());
        if(!success)
        {
            returnMsg.append("Failed to set param:unit.");
        }
    }

    if(returnMsg.isEmpty())
    {
        returnMsg.append("Success");
    }
    return returnMsg;
}

void TelldusCoreAPI::EnableControllerEvent()
{
    controllerEventId = tdRegisterControllerEvent( (TDControllerEvent) &ControllerEvent, 0);
}

void TelldusCoreAPI::DisableControllerEvent()
{
    tdUnregisterCallback(controllerEventId);
}

void TelldusCoreAPI::EnableDeviceEvent()
{
    deviceEventId = tdRegisterDeviceEvent( (TDDeviceEvent) &DeviceEvent, 0);
}

void TelldusCoreAPI::DisableDeviceEvent()
{
    tdUnregisterCallback(deviceEventId);
}

void TelldusCoreAPI::EnableDeviceChangeEvent()
{
    deviceChangedEventId = tdRegisterDeviceChangeEvent( (TDDeviceChangeEvent) &DeviceChangeEvent, 0);
}

void TelldusCoreAPI::DisableDeviceChangeEvent()
{
    tdUnregisterCallback(deviceChangedEventId);
}

void TelldusCoreAPI::EnableRawDataEvent()
{
    rawDataEventId = tdRegisterRawDeviceEvent( (TDRawDeviceEvent) &RawDataEvent, 0);
}

void TelldusCoreAPI::DisableRawDataEvent()
{
    tdUnregisterCallback(rawDataEventId);
}

void TelldusCoreAPI::EnableSensorEvent()
{
    sensorEventId = tdRegisterSensorEvent( (TDSensorEvent) &SensorEvent, 0);
}

void TelldusCoreAPI::DisableSensorEvent()
{
    tdUnregisterCallback(sensorEventId);
}


void WINAPI TelldusCoreAPI::RawDataEvent(const char *data, int controllerId, int callbackId, void *context)
{
    QDateTime local(QDateTime::currentDateTime());
    QString temp = data;
    QStringList processedData = temp.split(";");
    processedData.prepend("\t");
    processedData.prepend(QString::number(local.toMSecsSinceEpoch()));
    qDebug() << processedData;


}

void WINAPI TelldusCoreAPI::DeviceEvent(int deviceId, int method, const char *data, int callbackId, void *context)
{

}

void WINAPI TelldusCoreAPI::SensorEvent(const char *protocol, const char *model, int sensorId, int dataType, const char *value, int ts, int callbackId, void *context)
{
    char timeBuf[80];
    time_t timestamp = ts;

    //Print the sensor
    qDebug() << protocol << model << sensorId;


    //Retrieve the values the sensor supports
    if (dataType == TELLSTICK_TEMPERATURE) {
        strftime(timeBuf, sizeof(timeBuf), "%Y-%m-%d %H:%M:%S", localtime(&timestamp));
        qDebug() << "Temperature" << value << timeBuf;

    } else if (dataType == TELLSTICK_HUMIDITY) {
        strftime(timeBuf, sizeof(timeBuf), "%Y-%m-%d %H:%M:%S", localtime(&timestamp));
        qDebug() << "Humidity:" << value << timeBuf;
    }
}

void WINAPI TelldusCoreAPI::DeviceChangeEvent(int deviceId, int changeEvent, int changeType, int callbackId, void *context)
{
    if(changeEvent == TELLSTICK_DEVICE_ADDED)
    {
        qDebug() << "Device added with id " << deviceId;
    }
    else if(changeEvent == TELLSTICK_DEVICE_REMOVED)
    {
        qDebug() << "Device removed with id " << deviceId;
    }
    else if(changeEvent == TELLSTICK_DEVICE_CHANGED)
    {
        if(changeType == TELLSTICK_CHANGE_NAME)
        {
            qDebug() << "Device changed name id " << deviceId;
        }
        if(changeType == TELLSTICK_CHANGE_PROTOCOL)
        {
            qDebug() << "Device changed protocol id " << deviceId;
        }
        if(changeType == TELLSTICK_CHANGE_MODEL)
        {
            qDebug() << "Device changed model id " << deviceId;
        }
    }
}

void WINAPI TelldusCoreAPI::ControllerEvent(int controllerId, int changeEvent, int changeType, const char *newValue, int callbackId, void *context)
{

}


