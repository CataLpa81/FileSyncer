#include "FileSyncerTcpSocketServer.h"
#include "Log.h"
#include<qdir.h>
#include "DataConst.h"
#include <qhostaddress.h>
using namespace FileSyncerSystem;

void FileSyncerTcpSocketServer::AnalyzeDataHead()
{
	if (m_IsRecevingData)
	{
		UpdateFileReceiveProgress();
		return;
	}
	qint8 head;
	if (this->bytesAvailable() < sizeof(qint8))
	{
		Log::Warning("head length invaild");
		return;
	}
	this->dataBuffer >> head;
	switch (head)
	{
	case FROM_CLIENT_DELETEFILE:
		ProcessDeleteFileHead();
		break;
	case FROM_CLIENT_UPLOAD:
		ProcessUploadHead();
		break;
	case FROM_CLIENT_DOWNLOAD:
		ProcessDownloadHead();
		break;
	case FROM_CLIENT_GETFILELIST:
		ProcessGetFileListHead();
		break;
	default:
		Log::Warning("head type invaild");
		break;
	}
}

void FileSyncerTcpSocketServer::ProcessDeleteFileHead()
{
	QString targetFileName;
	this->dataBuffer >> targetFileName;
	QDir dir(m_FileSavePath);
	if (!dir.exists(targetFileName))
	{
		Log::Error("target File didn't exist");
		return;
	}
	QString filepath = dir.filePath(targetFileName);
	QFile file(filepath);
	if (!file.remove())
	{
		Log::Error("delete file [" + targetFileName + "] fail");
	}
	else
	{
		Log::Info("delete file [" + targetFileName + "] successfully");
	}
	
}

void FileSyncerTcpSocketServer::ProcessUploadHead()
{
	this->UpdateFileReceiveProgress();
}

void FileSyncerTcpSocketServer::ProcessDownloadHead()
{
	QString targetFileName;
	this->dataBuffer >> targetFileName;
	QDir dir(m_FileSavePath);
	if (!dir.exists(targetFileName))
	{
		Log::Error("target File didn't exist");
		return;
	}
	QString filepath = dir.filePath(targetFileName);
	this->sendFile(filepath, FROM_SERVER_FILE);
}

void FileSyncerTcpSocketServer::ProcessGetFileListHead()
{
	Log::Info("Get GET_FILE_LIST_HEAD");
	QDir dir(m_FileSavePath);
	dir.setFilter(QDir::Files | QDir::NoSymLinks);
	QFileInfoList fileInfoList = dir.entryInfoList(dir.filter() | QDir::NoDotAndDotDot);

	QByteArray	outBuffer;
	QDataStream outDataStream(&outBuffer, QIODevice::WriteOnly);
	outDataStream.setVersion(QDataStream::Qt_5_9);
	quint32 dataLength = 0;
	//message head
	outDataStream << FROM_SERVER_FILELIST;
	outDataStream << quint32(0);
	for (auto fileInfo : fileInfoList)
	{
		QString fileName = fileInfo.fileName();
		QString fileSuffix = fileInfo.suffix();
		outDataStream << fileName;
		outDataStream << fileSuffix;
	}

	outDataStream.device()->seek(sizeof(qint8));
	outDataStream << (quint32)(outBuffer.size()- sizeof(quint32) - sizeof(qint8));

	auto totalBytes = outBuffer.size();
	totalBytes = totalBytes - write(outBuffer);
	flush();
	
	Log::Info("File list has been sent");
	if (totalBytes != 0)
	{
		Log::Error("FileSyncerTcpSocketServer::ProcessGetFileListHead ");
	}
}

QString FileSyncerTcpSocketServer::GetFilePath()
{
	return m_FilePath;
}

void FileSyncerTcpSocketServer::setFilePath(const QString& filePath)
{
	m_FilePath = filePath;
}

void FileSyncerTcpSocketServer::MSGError(QAbstractSocket::SocketError error)
{
	QString hostAddress = this->peerAddress().toString();

	switch (error)
	{
		case QAbstractSocket::RemoteHostClosedError://¿Í»§¶Ë¶Ï¿ª
		{
			Log::Info("client from " + hostAddress + " has disconnected");
			emit clientDisconnected(this);
			break;
		}
		default:
		{
			Log::Info("get error from " + hostAddress + " error code :" + error);
			break;
		}
	}
}

void FileSyncerTcpSocketServer::OnDisconnect()
{
	QString hostAddress = this->peerAddress().toString();

	Log::Info("client from " + hostAddress + " has disconnected");
	emit clientDisconnected(this);
}

void FileSyncerTcpSocketServer::OnBytesWritten(qint64)
{
	if (m_IsSendingFile)
	{
		UpdateFileSendProgress();
	}
	else
	{

	}
}

QString FileSyncerTcpSocketServer::PackFileName(const QString& fileName)
{
	if (fileName.split('/').size() > 1)
	{
		return fileName;
	}
	
	return m_FilePath + fileName;
}
