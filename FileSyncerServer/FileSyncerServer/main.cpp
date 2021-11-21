#include <QtCore/QCoreApplication>
#include "FlieSyncerServer.h"
using namespace FileSyncerSystem;
int main(int argc, char *argv[])
{
    QCoreApplication a(argc, argv);
    FileSyncerThreadServer* server = new FileSyncerThreadServer();
    server->start();
    return a.exec();
}
