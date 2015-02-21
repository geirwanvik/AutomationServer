#include "server.h"

Server::Server(QObject *parent) : QObject(parent)
{

    tcpServer = new QTcpServer(this);
    socket = new QTcpSocket(this);

    connect(tcpServer,SIGNAL(newConnection()),this,SLOT(newConnection()));

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

void Server::sendData(QStringList list, quint16 elements, quint8 command, quint8 direction)
{
    QByteArray block;
    QDataStream out(&block,QIODevice::WriteOnly);
    out.setVersion(QDataStream::Qt_5_3);

    out << quint16(0);
    out << command;
    out << direction;
    out << elements;

    foreach(QString item, list)
    {
        out << item;
    }

    out.device()->seek(0);
    out << (quint16)(block.size() - sizeof(quint16));

    socket->write(block);
    socket->flush();
}

void Server::newConnection()
{
    socket = tcpServer->nextPendingConnection();
    connect(socket,SIGNAL(readyRead()),this,SLOT(dataReceived()));
    connect(socket,SIGNAL(disconnected()),this,SLOT(disconnected()));
            qDebug() << "Client connected";
}

void Server::disconnected()
{
    qDebug() << "Client disconnected";
}

void Server::dataReceived()
{
    static quint16 nextBlockSize = 0;
    quint16 numberofElements = 0;
    quint8 command = 0, direction = 0;

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
    in >> command;
    in >> direction;
    in >> numberofElements;

    processTelegram(command, direction, numberofElements,in);
    nextBlockSize = 0;
}

void Server::processTelegram(quint8 command, quint8 direction, quint16 elements, QDataStream &in)
{
    switch (command) {
    case 'A': //Transfer all devices
        if(direction == 'R')
        {
           emit readAllDevices();
        }
        if(direction == 'W')
        {
            emit writeAllDevices(in,elements);
        }
        break;
    case 'B': // Transfer one device
        break;
    case 'C': // Device event
        break;
    case 'D': // Send Command
        break;
    case 'E': // Read Log
        break;
    case 'F': // Scan for devices
        if(direction == 'R')
        {

        }
        if(direction == 'W')
        {
            QString msg;
            in >> msg;
            emit toggleLookForDevices(msg);
        }
        break;
    case 'G': // Transfer schedule
        break;
    default:
        emit logEvent(tr("TCP Received command of unknown type %1").arg(command));
        break;
    }
}

