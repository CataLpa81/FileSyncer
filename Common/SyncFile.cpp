#include "SyncFile.h"


using namespace FileSyncerSystem;

void dataSendInfo::Init()
{
    m_totalBytes = 0;
    m_bytesWritten = 0;
    m_bytesToWrite = 0;
}

void dataReceiveInfo::Init()
{
    m_totalBytes = 0;
    m_bytesReceived = 0;
}

void FileSendInfo::Init()
{
    m_totalBytes = 0;
    m_bytesToWrite = 0;
    m_filePath = "";
    m_fileName = "defalut";
}

void FileReceiveInfo::Init()
{
    m_totalBytes = 0;
    m_bytesReceived = 0;
    m_fileNameSize = 0;
    m_fileName = "defalut";
}