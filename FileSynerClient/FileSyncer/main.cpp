#include "FileSyncer.h"
#include <QtWidgets/QApplication>

using namespace FileSyncerSystem;
int main(int argc, char* argv[])
{
    QApplication a(argc, argv);
    FileSyncer::getInstance()->Start();
    FileSyncer::getInstance()->show();
    return a.exec();
}