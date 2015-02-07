#include <QCoreApplication>
#include <QDebug>
#include "manager.h"

int main(int argc, char *argv[])
{
    QCoreApplication a(argc, argv);

    Manager *manager = new Manager();

    return a.exec();
}


