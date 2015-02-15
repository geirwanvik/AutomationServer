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
    void TelegramReceived(QStringList);

public slots:
    void SendData(QStringList list);


private slots:
    void NewConnection();
    void Disconnected();
    void DataReceived();

private:
    QTcpServer *tcpServer;
    QTcpSocket *socket;
    quint16 nextBlockSize;
};

#endif // SERVER_H
