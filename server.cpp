#include "server.h"

Server::Server(QObject *parent) : QObject(parent)
{
    tcpServer = new QTcpServer(this);
    socket = new QTcpSocket;
    connect(tcpServer,SIGNAL(newConnection()),this,SLOT(NewConnection()));

    if(!tcpServer->listen(QHostAddress::Any, 2211))
    {
        qDebug() << "Server could not start!";
    }
    else
    {
        qDebug() << "Server started";
    }
}

Server::~Server()
{

}

void Server::NewConnection()
{
    socket = tcpServer->nextPendingConnection();
    socket->write("hello client\r\n");
    socket->flush();
    socket->waitForBytesWritten(3000);
    socket->close();
}
