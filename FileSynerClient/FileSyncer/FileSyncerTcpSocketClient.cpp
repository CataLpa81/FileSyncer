#include "FileSyncerTcpSocketClient.h"
#include "Log.h"
#include "qfileinfo.h"
#include <iostream>
using namespace FileSyncerSystem;

void FileSyncerTcpSocketClient::AnalyzeDataHead()
{
	qint8 head;
	if (this->bytesAvailable() < sizeof(qint8))
	{
		Log::Warning("head length invaild");
		return;
	}
	this->dataBuffer >> head;
	switch (head)
	{
	case FROM_SERVER_FILE:
		ProcessFileHead();
		break;
	case FROM_SERVER_FILELIST:
		ProcessFileListHead();
		break;
	default:
		Log::Warning("head type invaild");
		break;
	}
}
void FileSyncerTcpSocketClient::OnReadReady()
{
	if (m_IsRecevingData)
	{
		UpdateFileReceiveProgress();
		return;
	}
	else
	{
		AnalyzeDataHead();
	}
}

void FileSyncerTcpSocketClient::ProcessFileHead()
{
	this->UpdateFileReceiveProgress();
}

void FileSyncerTcpSocketClient::ProcessFileListHead()
{
	if (this->bytesAvailable() == 0)
	{
		Log::Error("FileSyncerTcpSocketClient::ProcessFileListHead didn't get data");
	}
	std::cout << this->bytesAvailable() << std::endl;
	quint32 dataLength;
	dataBuffer >> dataLength;
	int received = 0;
	QList<FileInfo> fileInfoList;
	QString tempString;
	QString tempSuffix;
	while (received < dataLength)
	{
		dataBuffer >> tempString;
		dataBuffer >> tempSuffix;
		received += 2 * tempString.size() + sizeof(qint32);
		received += 2 * tempSuffix.size() + sizeof(qint32);
		fileInfoList.append(FileInfo(tempString, tempSuffix));
	}
	emit FileListReady(fileInfoList);
	Log::Info("fileInfoList have gotten");

}

void FileSyncerTcpSocketClient::OnBytesWritten(qint64 num)
{
	if (m_IsSendingFile)
	{
		UpdateFileSendProgress();
	}
	else
	{

	}
}

void FileSyncerTcpSocketClient::SendDownloadFileRequest(const QString& fileName)
{
	QByteArray	outBuffer;
	QDataStream outDataStream(&outBuffer, QIODevice::WriteOnly);
	outDataStream.setVersion(QDataStream::Qt_5_9);
	outDataStream << FROM_CLIENT_DOWNLOAD;
	outDataStream << fileName;

	write(outBuffer);
	flush();
	Log::Info("Download Request has been send");
}

void FileSyncerTcpSocketClient::SendDeleteFileRequest(const QString& fileName)
{
	QByteArray	outBuffer;
	QDataStream outDataStream(&outBuffer, QIODevice::WriteOnly);
	outDataStream.setVersion(QDataStream::Qt_5_9);
	outDataStream << FROM_CLIENT_DELETEFILE;
	outDataStream << fileName;
	write(outBuffer);
	flush();
}

void FileSyncerTcpSocketClient::SendRefreshRequest()
{
	QByteArray	outBuffer;
	QDataStream outDataStream(&outBuffer, QIODevice::WriteOnly);
	outDataStream.setVersion(QDataStream::Qt_5_9);
	outDataStream << FROM_CLIENT_GETFILELIST;
	write(outBuffer);
	flush();
}




