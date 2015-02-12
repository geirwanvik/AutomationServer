#include <QCoreApplication>
#include <QDebug>
#include <QPointer>
#include "manager.h"

int main(int argc, char *argv[])
{
    QCoreApplication a(argc, argv);

    Manager manager;

    return a.exec();
}


