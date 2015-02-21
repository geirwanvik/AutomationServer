#ifndef SERVER_H
#define SERVER_H

#include <QObject>
#include <QtNetwork>

class Server : public QObject
{
    Q_OBJECT
public:
    explicit Server(QObject *parent = 0);
    ~Server();

signals:
    void logEvent(QString msg);
    void toggleLookForDevices(QString cmd);
    void readAllDevices();
    void writeAllDevices(QDataStream &in, int elements);

public slots:
    void sendData(QStringList list, quint16 elements, quint8 command, quint8 direction);


private slots:
    void newConnection();
    void disconnected();
    void dataReceived();

private:
    void processTelegram(quint8 command, quint8 direction, quint16 elements, QDataStream &in);

    QTcpServer *tcpServer;
    QTcpSocket *socket;
};

#endif // SERVER_H
