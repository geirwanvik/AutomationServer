#include "tellduscoreapi.h"
#include <QDebug>

const int DATA_LENGTH = 20;


TelldusCoreAPI::TelldusCoreAPI(QObject *parent) : QObject(parent)
{
    tdInit();
    tdRegisterDeviceEvent( (TDDeviceEvent) &DeviceEventCallback, 0);
    tdRegisterSensorEvent( (TDSensorEvent) &SensorEventCallback, 0);
    tdRegisterRawDeviceEvent( (TDRawDeviceEvent) &RawDataEventCallback, 0);
    tdRegisterDeviceChangeEvent( (TDDeviceChangeEvent) &DeviceChangeEventCallback, 0);
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

    QTime myClock;
    myClock.start();

    while(tdGetNumberOfDevices() != 0 && myClock.elapsed() < 5000)
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

    int methods = tdMethods(id, 1023);
    dev.SetMethodsSupported(methods); // Check for all supported methods, all 10 bits
    dev.SetLastCommandSent(tdLastSentCommand(id,methods));
    if(methods & TELLSTICK_DIM)
    {
        dev.SetIsDimmer(true);
    }
    else
    {
        dev.SetIsDimmer(false);
    }



    if(returnMsg.isEmpty())
    {
        returnMsg.append("Success");
    }
    return returnMsg;
}

QString TelldusCoreAPI::DeviceTurnOn(Device &dev)
{
    bool success;
    int id = dev.GetId();
    int methods = dev.GetMethodsSupported();
    if(methods & TELLSTICK_TURNON)
    {
        success = tdTurnOn(id);
    }
    if(!success)
    {
        return "Failed";
    }
    return "Success";
}

QString TelldusCoreAPI::DeviceTurnOff(Device &dev)
{
    bool success;
    int id = dev.GetId();
    int methods = dev.GetMethodsSupported();
    if(methods & TELLSTICK_TURNOFF)
    {
        success = tdTurnOn(id);
    }
    if(!success)
    {
        return "Failed";
    }
    return "Success";
}

QString TelldusCoreAPI::DeviceDim(Device &dev)
{
    bool success;
    int id = dev.GetId();
    int methods = dev.GetMethodsSupported();
    if(methods & TELLSTICK_DIM)
    {
        success = tdDim(id,dev.GetValue());
    }
    if(!success)
    {
        return "Failed";
    }
    return "Success";
}

QString TelldusCoreAPI::DeviceCommand(int id, int method, int value)
{
    bool success;
    int returnMethod = tdMethods(id,method);
    if(returnMethod == method)
    {
        if(method == TELLSTICK_TURNON)
        {
            tdTurnOn(id);
        }
        else if(method == TELLSTICK_TURNOFF)
        {
            tdTurnOff(id);
        }
        else if(method == TELLSTICK_DIM)
        {
            tdDim(id,(QString::number(value).constData()->toLatin1()));
        }
    }
}



void WINAPI TelldusCoreAPI::RawDataEventCallback(const char *data, int controllerId, int, void*)
{
    static QStringList lastEvent;
    QStringList eventInfo;
    eventInfo << "RawDataEvent";
    eventInfo << QTime::currentTime().toString("hh:mm:ss.zzz");
    eventInfo << QString::fromLocal8Bit(data).split(";");
    eventInfo << QString::number(controllerId);
    if(lastEvent == eventInfo) // Duplicate callback
    {
        lastEvent = eventInfo;
        return;
    }
    lastEvent = eventInfo;

    TelldusCore::Instance()->RawDataEvent(eventInfo);
}

void WINAPI TelldusCoreAPI::DeviceEventCallback(int deviceId, int method, const char *data, int, void*)
{
    QList<int> eventInfo;
    eventInfo << deviceId;
    eventInfo << method;
    eventInfo << QString::fromLocal8Bit(data).toInt();

    TelldusCore::Instance()->DeviceEvent(eventInfo);
}

void WINAPI TelldusCoreAPI::SensorEventCallback(const char *protocol, const char *model, int sensorId, int dataType, const char *value, int, int, void*)
{
    QStringList eventInfo;
    eventInfo << QString::number(sensorId);
    eventInfo << QString::number(dataType);
    eventInfo << QString::fromLocal8Bit(value);
    eventInfo << QString::fromLocal8Bit(protocol);
    eventInfo << QString::fromLocal8Bit(model);

    TelldusCore::Instance()->SensorEvent(eventInfo);
}

void WINAPI TelldusCoreAPI::DeviceChangeEventCallback(int deviceId, int changeEvent, int changeType, int, void*)
{
    static QStringList lastEvent;
    QStringList eventInfo;
    eventInfo << "DeviceChangeEvent";
    eventInfo << QTime::currentTime().toString("hh:mm:ss.zzz");
    eventInfo << QString::number(deviceId);
    eventInfo << QString::number(changeEvent);
    eventInfo << QString::number(changeType);

    if(lastEvent == eventInfo) // Duplicate callback
    {
        lastEvent = eventInfo;
        return;
    }
    lastEvent = eventInfo;

    TelldusCore::Instance()->DeviceChangeEvent(eventInfo);
}

void WINAPI TelldusCoreAPI::ControllerEventCallback(int controllerId, int changeEvent, int changeType, const char *newValue, int, void*)
{
    static QStringList lastEvent;
    QStringList eventInfo;
    eventInfo << "ControllerEvent";
    eventInfo << QTime::currentTime().toString("hh:mm:ss.zzz");
    eventInfo << QString::number(controllerId);
    eventInfo << QString::number(changeEvent);
    eventInfo << QString::number(changeType);
    eventInfo << QString::fromLocal8Bit(newValue);

    if(lastEvent == eventInfo) // Duplicate callback
    {
        lastEvent = eventInfo;
        return;
    }
    lastEvent = eventInfo;

    TelldusCore::Instance()->ControllerEvent(eventInfo);
}


