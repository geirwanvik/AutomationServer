#include "databaseitem.h"

DataBaseItem::DataBaseItem(QObject *parent) : QObject(parent)
{

}

DataBaseItem::~DataBaseItem()
{

}
int DataBaseItem::getMasterId() const
{
    return masterId;
}

void DataBaseItem::setMasterId(int value)
{
    masterId = value;
}
int DataBaseItem::getTriggerCommand() const
{
    return triggerCommand;
}

void DataBaseItem::setTriggerCommand(int value)
{
    triggerCommand = value;
}
int DataBaseItem::getTriggerValue() const
{
    return triggerValue;
}

void DataBaseItem::setTriggerValue(int value)
{
    triggerValue = value;
}
QString DataBaseItem::getTriggerCondition() const
{
    return triggerCondition;
}

void DataBaseItem::setTriggerCondition(const QString &value)
{
    triggerCondition = value;
}
//"master,id:3,command:1,value:,condition:=,slave,id:1,command:1,value:,slave,id:2,command:16,value:255";
void DataBaseItem::ParseListString(QString item)
{
    QStringList itemList;
    itemList = item.split(",");
    int inc = itemList.indexOf("master");
    if(inc>=0)
    {
        inc++;
        this->masterId = itemList.at(inc++).toInt();
        this->triggerCommand = itemList.at(inc++).toInt();
        this->triggerValue = itemList.at(inc++).toInt();
        this->triggerCondition = itemList.at(inc++);

        while(inc < itemList.size())
        {
            inc++;
            SlaveItem item;
            item.slaveId = itemList.at(inc++).toInt();
            item.slaveCommand = itemList.at(inc++).toInt();
            item.slaveValue = itemList.at(inc++).toInt();
            slaveList.append(item);
        }
    }
}

void DataBaseItem::CompareToTrigger(int command, int value)
{
    if(triggerCommand != command)
    {
        return;
    }

    foreach(SlaveItem slave,slaveList)
    {
        emit SlaveSignal(slave.slaveId,slave.slaveCommand,slave.slaveValue);
    }
}





