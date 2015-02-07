#ifndef SERVER_H
#define SERVER_H

#include <QObject>
#include <QtNetwork>

class QTcpServer;
class QNetworkSession;

class Server : public QObject
{
    Q_OBJECT
public:
    explicit Server(QObject *parent = 0);
    ~Server();

signals:

public slots:

private slots:
    void sessionOpened();
    void sendFortune();

private:
    QTcpServer *tcpServer;
    QStringList fortunes;
    QNetworkSession *networkSession;
};

#endif // SERVER_H
