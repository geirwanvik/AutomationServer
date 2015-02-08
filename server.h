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

public slots:
    void NewConnection();

private slots:


private:
    QTcpServer *tcpServer;
    QTcpSocket *socket;
};

#endif // SERVER_H
