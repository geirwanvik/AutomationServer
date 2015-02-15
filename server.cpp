#include "server.h"

Server::Server(QObject *parent) : QObject(parent)
{

    tcpServer = new QTcpServer(this);
    socket = new QTcpSocket(this);
    nextBlockSize = 0;

    connect(tcpServer,SIGNAL(newConnection()),this,SLOT(NewConnection()));

    if(!tcpServer->listen(QHostAddress::Any, 1234))
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

void Server::SendData(QStringList list)
{
    QByteArray block;
    QDataStream out(&block,QIODevice::WriteOnly);
    out.setVersion(QDataStream::Qt_5_3);

    out << quint16(0);
    out << (quint16)list.size();

    foreach(QString item, list)
    {
        out << item;
    }

    out.device()->seek(0);
    out << (quint16)(block.size() - sizeof(quint16));

    socket->write(block);
    socket->flush();
}

void Server::NewConnection()
{
    socket = tcpServer->nextPendingConnection();
    connect(socket,SIGNAL(readyRead()),this,SLOT(DataReceived()));
    connect(socket,SIGNAL(disconnected()),this,SLOT(Disconnected()));
            qDebug() << "Client connected";
}

void Server::Disconnected()
{
    qDebug() << "Client disconnected";
}

void Server::DataReceived()
{
    static quint16 itemsToReceive = 0, counter = 0;
    static QStringList receivedItems;
    QDataStream in(socket);
    in.setVersion(QDataStream::Qt_5_3);

    if(nextBlockSize == 0)
    {
        if(socket->bytesAvailable() < sizeof(quint16))
        {
            return;
        }
        in >> nextBlockSize;
    }

    if(socket->bytesAvailable() < nextBlockSize)
    {
        return;
    }

    if(itemsToReceive == 0)
    {
        in >> itemsToReceive;
    }

    for(; counter< itemsToReceive; counter++)
    {
        QString newItem;
        in >> newItem;
        receivedItems << newItem;
    }
    if(itemsToReceive == counter)
    {
        nextBlockSize = 0;
        counter = 0;
        itemsToReceive = 0;
        emit TelegramReceived(receivedItems);
        receivedItems.clear();
    }
}


