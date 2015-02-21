#include "tellstick.h"
#include <QDebug>

const int DATA_LENGTH = 20;
int callbackRefs[4];

Tellstick::Tellstick(QObject *parent) : QObject(parent)
{
    tdInit();

    removePreviousConfig();

    callbackRefs[0] = tdRegisterDeviceEvent( (TDDeviceEvent) &deviceEventCallback, 0);
    callbackRefs[1] =tdRegisterSensorEvent( (TDSensorEvent) &sensorEventCallback, 0);
    callbackRefs[2] =tdRegisterRawDeviceEvent( (TDRawDeviceEvent) &rawDataEventCallback, 0);
    callbackRefs[3] =tdRegisterDeviceChangeEvent( (TDDeviceChangeEvent) &deviceChangeEventCallback, 0);
}

Tellstick::~Tellstick()
{
    tdUnregisterCallback(callbackRefs[0]);
    tdUnregisterCallback(callbackRefs[1]);
    tdUnregisterCallback(callbackRefs[2]);
    tdUnregisterCallback(callbackRefs[3]);

    tdClose();
}

void Tellstick::removePreviousConfig()
{
    QByteArray devList;

    int numberOfDevices = tdGetNumberOfDevices();

    for (int i = 0; i < numberOfDevices; i++)
    {
      int id = tdGetDeviceId(i);
      devList.append(id);
    }

    for(int i = 0; i<devList.size(); i++)
    {
      tdRemoveDevice(devList.at(i));
    }

    QTime myClock;
    myClock.start();

    while(tdGetNumberOfDevices() != 0 && myClock.elapsed() < 5000)
    {
        // Wait for telldusd to update or 5 sec
    }
    if(tdGetNumberOfDevices() == 0)
    {
        emit logEvent(tr("Previous config successfully removed"));
        return;
    }
    emit logEvent(tr("Failed to remove all devices in config, %1 devices remaining").arg(tdGetNumberOfDevices()));
}

void Tellstick::registerSensor(Device &dev)
{

}

void Tellstick::registerSwitch(Device &dev)
{
    int returnValue;

    // Id set by core
    int id = tdAddDevice();
    if(id < 0) // If error, the error code is returned as the device id, negative number
    {
        errorCode(-1,id);
        return;
    }
    dev.setId(id);

    // Set everything else
    returnValue = tdSetName(id,dev.getName().toUtf8().constData());
    if(!returnValue)
    {
        errorCode(id,returnValue); // Log
        removeSwitch(dev); // Clean up
        return;
    }

    returnValue = tdSetProtocol(id,dev.getProtocol().toUtf8().constData());
    if(!returnValue)
    {
        errorCode(id,returnValue); // Log
        removeSwitch(dev); // Clean up
        return;
    }

    returnValue = tdSetModel(id,dev.getModel().toUtf8().constData());
    if(!returnValue)
    {
        errorCode(id,returnValue); // Log
        removeSwitch(dev); // Clean up
        return;
    }

    // If the model is codeswitch or selflearning-switch, house and unit parameter must also be set
    // If the model is bell, house parameter must be set.
    if(dev.getModel().startsWith("bell"))
    {
        returnValue = tdSetDeviceParameter(id,"house",dev.getHouse().toUtf8().constData());
        if(!returnValue)
        {
            errorCode(id,returnValue); // Log
            removeSwitch(dev); // Clean up
            return;
        }

    }
    else if(dev.getModel().startsWith("codeswitch") || dev.getModel().startsWith("selflearning-switch"))
    {
        returnValue = tdSetDeviceParameter(id,"house",dev.getHouse().toUtf8().constData());
        if(!returnValue)
        {
            errorCode(id,returnValue); // Log
            removeSwitch(dev); // Clean up
            return;
        }
        returnValue = tdSetDeviceParameter(id,"unit",dev.getUnit().toUtf8().constData());
        if(!returnValue)
        {
            errorCode(id,returnValue); // Log
            removeSwitch(dev); // Clean up
            return;
        }
    }

    int supportedCommands = tdMethods(id, SwitchCommandTypes::AllSwitchTypes);

    dev.setSupportedCommands(supportedCommands);

    int lastCommand = tdLastSentCommand(id,supportedCommands);
    int lastValue = atoi(tdLastSentValue(id));

    dev.setLastCommand(lastCommand);
    dev.setLastValue(lastValue);
}

void Tellstick::removeSwitch(Device &dev)
{
    bool success;
    success = tdRemoveDevice(dev.getId());
    QString result;
    if(success)
    {
        result = tr("Device with id %1 and name %2 removed from Telldus Core").arg(dev.getId()).arg(dev.getName());
    }
    else
    {
        result = tr("Could not remove device with id %1 and name %2 removed from Telldus Core").arg(dev.getId()).arg(dev.getName());
    }
    emit logEvent(result);
}

void Tellstick::registerNewDevice(Device &dev)
{
    QString type = dev.getType();

    if(type == "sensor")
    {
        registerSensor(dev);
    }
    if(type == "switch")
    {
        registerSwitch(dev);
    }
}

void Tellstick::deviceCommand(int id, int command, int value)
{
    char dimmerValue = value;
    int success = tdMethods(id,command);
    if(!success)
    {
        QString msg = tr("Device with id %1 does not support command type %2").arg(id).arg(command);
        emit logEvent(msg);
        return;
    }
    switch(command)
    {
    case SwitchCommandTypes::TurnOn:
        success = tdTurnOn(id);
        break;

    case SwitchCommandTypes::TurnOff:
        success = tdTurnOff(id);
        break;

    case SwitchCommandTypes::Bell:
        success = tdBell(id);
        break;

    case SwitchCommandTypes::Toggle:
        //Not implemented by Telldus
        break;

    case SwitchCommandTypes::Dim:
        success = tdDim(id,dimmerValue);
        break;

    case SwitchCommandTypes::Learn:
        success = tdLearn(id);
        break;

    case SwitchCommandTypes::Execute:
        success = tdExecute(id);
        break;

    case SwitchCommandTypes::Up:
        success = tdUp(id);
        break;

    case SwitchCommandTypes::Down:
        success = tdDown(id);
        break;

    case SwitchCommandTypes::Stop:
        success = tdStop(id);
        break;
    }

    if(!success)
    {
        errorCode(id,success);
    }
}

void Tellstick::errorCode(int id, int code)
{
    switch(code)
    {
    case ErrorCodes::Success:
        emit logEvent(tr("Error function called without error on id %1").arg(id));
        break;

    case ErrorCodes::NotFound:
        emit logEvent(tr("Error : Device id %1 Tellstick not found").arg(id));
        break;

    case ErrorCodes::PermissionDenied:
        emit logEvent(tr("Error : Device id %1 TellStick permission denied").arg(id));
        break;

    case ErrorCodes::DeviceNotFound:
        emit logEvent(tr("Error : Device id %1 TellStick permission denied").arg(id));
        break;

    case ErrorCodes::MethodNotSupported:
        emit logEvent(tr("Error : Device id %1 The method is not supported by the device.").arg(id));
        break;

    case ErrorCodes::Communication:
        emit logEvent(tr("Error : Device id %1 Error when communicating with TellStick").arg(id));
        break;

    case ErrorCodes::ConnectingService:
        emit logEvent(tr("Error : Device id %1 The client library could not connect to the service").arg(id));
        break;

    case ErrorCodes::UnknownResponse:
        emit logEvent(tr("Error : Device id %1 The client library received a response from the service it did not understand").arg(id));
        break;

    case ErrorCodes::Syntax:
        emit logEvent(tr("Error : Device id %1 Input/command could not be parsed or didn't follow input rules").arg(id));
        break;

    case ErrorCodes::BrokenPipe:
        emit logEvent(tr("Error : Device id %1 Pipe broken during communication").arg(id));
        break;

    case ErrorCodes::CommunicatingService:
        emit logEvent(tr("Error : Device id %1 Timeout waiting for response from the Telldus Service").arg(id));
        break;

    case ErrorCodes::UnknownError:
        emit logEvent(tr("Error : Device id %1 An unknown error has occurred").arg(id));
        break;
    }
}

void WINAPI Tellstick::deviceEventCallback(int deviceId, int method, const char *data, int, void*)
{
    static QString lastMsg;
    QString msg = tr("Device Event from id %1, method %2, data %3").arg(deviceId).arg(method).arg(data);

    static quint64 lastEvent = 0;
    quint64 thisEvent = QDateTime::currentDateTime().toMSecsSinceEpoch();

    if(lastMsg == msg && lastEvent == thisEvent)
    {
        return;
    }
    lastMsg = msg;
    lastEvent = thisEvent;

    int eventValue = atoi(data);
    TellstickSingleton::Instance()->deviceEvent(deviceId, method, eventValue, "switch");

    TellstickSingleton::Instance()->logEvent(msg);
}

void WINAPI Tellstick::sensorEventCallback(const char *protocol, const char *model, int sensorId, int dataType, const char *value, int, int, void*)
{
    static QString lastMsg;
    QString msg = tr("Sensor Event from protocol %1, model %2, id %3, datatype %4, value %5").arg(protocol).arg(model).arg(sensorId).arg(dataType).arg(value);

    static quint64 lastEvent = 0;
    quint64 thisEvent = QDateTime::currentDateTime().toMSecsSinceEpoch();

    if(lastMsg == msg && lastEvent == thisEvent)
    {
        return;
    }
    lastMsg = msg;
    lastEvent = thisEvent;

    int eventValue = atoi(value);
    TellstickSingleton::Instance()->deviceEvent(sensorId, dataType, eventValue, "sensor");

    TellstickSingleton::Instance()->logEvent(msg);
}

void WINAPI Tellstick::rawDataEventCallback(const char *data, int controllerId, int, void*)
{
    static QString lastMsg;
    QString msg = tr("Raw Event from id %1, data %2").arg(controllerId).arg(data);
    static quint64 lastEvent = 0;
    quint64 thisEvent = QDateTime::currentDateTime().toMSecsSinceEpoch();

    if(lastMsg == msg && lastEvent == thisEvent)
    {
        return;
    }
    lastMsg = msg;
    lastEvent = thisEvent;

    TellstickSingleton::Instance()->logEvent(msg);
}

void WINAPI Tellstick::deviceChangeEventCallback(int deviceId, int changeEvent, int changeType, int, void*)
{
    static QString lastMsg;
    QString msg = tr("Device Change Event from id %1, change event %2, change type %3").arg(deviceId).arg(changeEvent).arg(changeType);
    static quint64 lastEvent = 0;
    quint64 thisEvent = QDateTime::currentDateTime().toMSecsSinceEpoch();

    if(lastMsg == msg && lastEvent == thisEvent)
    {
        return;
    }
    lastMsg = msg;
    lastEvent = thisEvent;

    TellstickSingleton::Instance()->logEvent(msg);
}

void WINAPI Tellstick::controllerEventCallback(int controllerId, int changeEvent, int changeType, const char *newValue, int, void*)
{
    static QString lastMsg;
    QString msg = tr("Controller Event from controller id %1, change event %2, change type %3, new value %4").arg(controllerId).arg(changeEvent).arg(changeType).arg(newValue);
    static quint64 lastEvent = 0;
    quint64 thisEvent = QDateTime::currentDateTime().toMSecsSinceEpoch();

    if(lastMsg == msg && lastEvent == thisEvent)
    {
        return;
    }
    lastMsg = msg;
    lastEvent = thisEvent;

    TellstickSingleton::Instance()->logEvent(msg);
}
