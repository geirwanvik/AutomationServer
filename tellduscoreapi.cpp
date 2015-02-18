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

QString TelldusCoreAPI::RegisterNewDevice(Devices &dev)
{
    bool success;
    QString returnMsg;

    // Get the device id from telldusd
    int id = tdAddDevice();
    if(id < 0) // If error, the error code is returned as the device id, negative number
    {
        return ErrorCode(-1,id);
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
        success = tdSetDeviceParameter(id,"house",dev.GetHouse().toUtf8().constData());
        if(!success)
        {
            returnMsg.append("Failed to set param:house.");
        }

    }
    else if(dev.GetModel().startsWith("codeswitch") || dev.GetModel().startsWith("selflearning-switch"))
    {
        success = tdSetDeviceParameter(id,"house",dev.GetHouse().toUtf8().constData());
        if(!success)
        {
            returnMsg.append("Failed to set param:house.");
        }
        success = tdSetDeviceParameter(id,"unit",dev.GetUnit().toUtf8().constData());
        if(!success)
        {
            returnMsg.append("Failed to set param:unit.");
        }
    }

    int supportedCommands = tdMethods(id, TurnOn | TurnOff | Bell | Toggle | Dim | Learn | Execute | Up | Down | Stop);
    dev.SetSupportedCommands(supportedCommands);

    dev.SetLastCommand(tdLastSentCommand(id,supportedCommands));

    if(returnMsg.isEmpty())
    {
        returnMsg.append("Success");
    }
    return returnMsg;
}

QList<Devices> TelldusCoreAPI::LookForSensors()
{
    QList<Devices> deviceList;
    char protocol[DATA_LENGTH], model[DATA_LENGTH];
    int id = 0, dataTypes = 0;
    while(tdSensor(protocol,DATA_LENGTH,model,DATA_LENGTH,&id,&dataTypes) == TELLSTICK_SUCCESS)
    {
        QString unknownParam = "not set";
        QString myProtocol = QString::fromLocal8Bit(protocol);
        QString myModel = QString::fromLocal8Bit(model);
        QString myType = "sensor";
        Devices dev(id,unknownParam,myProtocol,myModel,
                    unknownParam, unknownParam, myType, dataTypes, 0, 0);
        deviceList.append(dev);
        qDebug() << "Found device id " << id;
    }
    return deviceList;
}


QString TelldusCoreAPI::DeviceCommand(int id, int command, int value)
{
    int success;
    char dimmerValue = value;
    int returnMethod = tdMethods(id,command);
    if(returnMethod == command)
    {
        if(command == TELLSTICK_TURNON)
        {
            success = tdTurnOn(id);
        }
        else if(command == TELLSTICK_TURNOFF)
        {
            success = tdTurnOff(id);
        }
        else if(command == TELLSTICK_BELL)
        {
            success = tdBell(id);
        }
        else if(command == TELLSTICK_TOGGLE) // Not implemented by Telldus
        {
            return tr("Error : Device id %1 does not support command %2").arg(id).arg(command);
        }
        else if(command == TELLSTICK_DIM)
        {
            success = tdDim(id,dimmerValue);
        }
        else if(command == TELLSTICK_LEARN)
        {
            success = tdLearn(id);
        }
        else if(command == TELLSTICK_EXECUTE)
        {
            success = tdExecute(id);
        }
        else if(command == TELLSTICK_UP)
        {
            success = tdUp(id);
        }
        else if(command == TELLSTICK_DOWN)
        {
            success = tdDown(id);
        }
        else if(command == TELLSTICK_STOP)
        {
            success = tdStop(id);
        }

        if(success != 0)
        {
            return ErrorCode(id,success);
        }
        else
        {
            return "Success";
        }
    }
    else
    {
        return tr("Error : Device id %1 does not support command %2").arg(id).arg(command);
    }
}

QString TelldusCoreAPI::ErrorCode(int id, int code)
{
    if(code == TELLSTICK_ERROR_NOT_FOUND)
    {
        return tr("Error : Device id %1 Tellstick not found").arg(id);
    }
    if(code == TELLSTICK_ERROR_PERMISSION_DENIED)
    {
        return tr("Error : Device id %1 TellStick permission denied").arg(id);
    }
    if(code == TELLSTICK_ERROR_DEVICE_NOT_FOUND)
    {
        return tr("Error : Device id %1 Device not found").arg(id);
    }
    if(code == TELLSTICK_ERROR_METHOD_NOT_SUPPORTED)
    {
        return tr("Error : Device id %1 The method is not supported by the device.").arg(id);
    }
    if(code == TELLSTICK_ERROR_COMMUNICATION)
    {
        return tr("Error : Device id %1 Error when communicating with TellStick").arg(id);
    }
    if(code == TELLSTICK_ERROR_CONNECTING_SERVICE)
    {
        return tr("Error : Device id %1 The client library could not connect to the service").arg(id);
    }
    if(code == TELLSTICK_ERROR_UNKNOWN_RESPONSE)
    {
        return tr("Error : Device id %1 The client library received a response from the service it did not understand").arg(id);
    }
    if(code == TELLSTICK_ERROR_SYNTAX)
    {
        return tr("Error : Device id %1 Input/command could not be parsed or didn't follow input rules").arg(id);
    }
    if(code == TELLSTICK_ERROR_BROKEN_PIPE)
    {
        return tr("Error : Device id %1 Pipe broken during communication").arg(id);
    }
    if(code == TELLSTICK_ERROR_COMMUNICATING_SERVICE)
    {
        return tr("Error : Device id %1 Timeout waiting for response from the Telldus Service").arg(id);
    }
    if(code == TELLSTICK_ERROR_UNKNOWN)
    {
        return tr("Error : Device id %1 An unknown error has occurred").arg(id);
    }
    return "";
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
    static quint64 lastEvent = 0;
    quint64 thisEvent = QDateTime::currentDateTime().toMSecsSinceEpoch();
    if(thisEvent == lastEvent)
    {
        return;
    }
    lastEvent = thisEvent;

    TelldusCore::Instance()->DeviceEvent(deviceId,method,data);
}

void WINAPI TelldusCoreAPI::SensorEventCallback(const char *protocol, const char *model, int sensorId, int dataType, const char *value, int, int, void*)
{
    TelldusCore::Instance()->SensorEvent(protocol, model, sensorId, dataType, value);
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


