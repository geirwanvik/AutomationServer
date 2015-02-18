#ifndef DATABASEITEM_H
#define DATABASEITEM_H

#include <QObject>
#include <QStringList>
class DataBaseItem : public QObject
{
    Q_OBJECT
public:
    explicit DataBaseItem(QObject *parent = 0);
    ~DataBaseItem();

    int getMasterId() const;
    void setMasterId(int value);

    int getTriggerCommand() const;
    void setTriggerCommand(int value);

    int getTriggerValue() const;
    void setTriggerValue(int value);

    QString getTriggerCondition() const;
    void setTriggerCondition(const QString &value);

    void ParseListString(QString item);

    void CompareToTrigger(int command, int value);

signals:
    void SlaveSignal(int id, int command, int value);

public slots:

private:
    struct SlaveItem
    {
    public:
        int slaveId;
        int slaveCommand;
        int slaveValue;
    };

    int masterId;
    int triggerCommand;
    int triggerValue;
    QString triggerCondition;
    QList<SlaveItem> slaveList;

};

#endif // DATABASEITEM_H
