#include "FileSyncerTcpSocket.h"
#include "Log.h"
#include <qdir.h>
using namespace FileSyncerSystem;

FileSyncerTcpSocket::FileSyncerTcpSocket()
{
	ResetFileSendInfo();
	ResetFileReceiveInfo();
	dataBuffer.setDevice(this);
	dataBuffer.setVersion(QDataStream::Qt_5_9);
}

FileSyncerTcpSocket::~FileSyncerTcpSocket()
{
	if (state() == QAbstractSocket::ConnectedState)
	{
		disconnectFromHost();
	}
}

void FileSyncerTcpSocket::sendFile(const QString& filePath, qint8 head)
{
	if (!waitForConnected())
	{
		Log::Error("Tcp disconnected");
		return;
	}
	m_FileSendInfo.m_localFile = new QFile(filePath);
	if (!m_FileSendInfo.m_localFile->open(QFile::ReadOnly))
	{
		Log::Error("Client: open file" + filePath + " fail");
		m_FileSendInfo.m_localFile = nullptr;
		return;
	}

	m_IsSendingFile = true;
	
	//
	m_FileSendInfo.m_filePath = filePath;
	m_FileSendInfo.m_totalBytes = m_FileSendInfo.m_localFile->size();
	m_FileSendInfo.m_fileName = filePath.right(filePath.size() - filePath.lastIndexOf('/') - 1);
	Log::Info("Send file begin, file name: " + m_FileSendInfo.m_fileName);
	QDataStream sendOut(&m_OutBuffer, QIODevice::WriteOnly);
	sendOut.setVersion(QDataStream::Qt_5_9);
	sendOut << head;
	sendOut << qint64(0) << qint64(0) << m_FileSendInfo.m_fileName;
	m_FileSendInfo.m_totalBytes += m_OutBuffer.size();

	sendOut.device()->seek(sizeof(qint8));
	sendOut << m_FileSendInfo.m_totalBytes << qint64(m_OutBuffer.size() - sizeof(qint64) * 2);
	m_FileSendInfo.m_bytesToWrite = m_FileSendInfo.m_totalBytes - write(m_OutBuffer);
	m_OutBuffer.resize(0);
	//

}

void FileSyncerTcpSocket::sendFile()
{
	if (!waitForConnected())
	{
		Log::Error("FileSyncerTcpSocket::sendFile: Tcp disconnected");
		return;
	}
	if (filesWaitToSend.isEmpty())
	{
		Log::Info("FileSyncerTcpSocket::sendFile: no file wait to send");
		return;
	}

	QString path = filesWaitToSend.back();
	filesWaitToSend.removeLast();
}

void FileSyncerTcpSocket::onConnected()
{
	Log::Info("connect successfully");
}

void FileSyncerTcpSocket::UpdateFileSendProgress()
{
	if (m_FileSendInfo.m_bytesToWrite > 0) {
		m_OutBuffer = m_FileSendInfo.m_localFile->read(qMin(m_FileSendInfo.m_bytesToWrite, PAYLOADSIZE));
		m_FileSendInfo.m_bytesToWrite -= this->write(m_OutBuffer);
		Log::Info("Client: file " 
			+ m_FileSendInfo.m_filePath 
			+ " is sending, rest bytes : " 
			+ QString::number(m_FileSendInfo.m_bytesToWrite));
		m_OutBuffer.resize(0);
		emit FileBytesWritten(this);
	}
	else {
		m_FileSendInfo.m_localFile->close();
		AfterSendFile();
		return;
	}
}



void FileSyncerTcpSocket::UpdateFileReceiveProgress()
{
	m_IsRecevingData = true;

    // 如果接收到的数据小于16个字节，保存到来的文件头结构
    if (m_FileReceiveInfo.m_bytesReceived <= sizeof(qint64) * 2) {
        if ((this->bytesAvailable() >= sizeof(qint64) * 2) && (m_FileReceiveInfo.m_fileNameSize == 0)) {
            // 接收数据总大小信息和文件名大小信息
			dataBuffer >> m_FileReceiveInfo.m_totalBytes >> m_FileReceiveInfo.m_fileNameSize;
            m_FileReceiveInfo.m_bytesReceived += sizeof(qint64) * 2;
			Log::Info("------receiving head info---------");
        }
		
        if ((this->bytesAvailable() >= m_FileReceiveInfo.m_fileNameSize) && (m_FileReceiveInfo.m_fileNameSize != 0)) {
            // 接收文件名，并建立文件
			dataBuffer >> m_FileReceiveInfo.m_fileName;
            m_FileReceiveInfo.m_bytesReceived += m_FileReceiveInfo.m_fileNameSize;
			QDir dir(m_FileSavePath);
			int index = 1;
			QFileInfo fileInfo(m_FileReceiveInfo.m_fileName);
			QString oldName = fileInfo.fileName().split(".", QString::SkipEmptyParts).at(0);
			QString suffix = fileInfo.suffix();
			while (dir.exists(m_FileReceiveInfo.m_fileName))
			{
				m_FileReceiveInfo.m_fileName = oldName
					+ "(" + QString::number(index) + ")" + "." + suffix;
				index++;
				Log::Info("File " + oldName + " exists, the file will be rename as "
					+ m_FileReceiveInfo.m_fileName);

			}
			QString wholePath = m_FileSavePath + "/" + m_FileReceiveInfo.m_fileName;
            m_FileReceiveInfo.m_localFile = new QFile(wholePath);
			Log::Info("start receive file :" + m_FileReceiveInfo.m_fileName);
            if (!m_FileReceiveInfo.m_localFile->open(QFile::WriteOnly)) {
                Log::Info("open file error!");
                return;
            }
        }
        else {
            return;
        }
    }

    // 如果接收的数据小于总数据，那么写入文件
    if (m_FileReceiveInfo.m_bytesReceived < m_FileReceiveInfo.m_totalBytes) {
		qint64 bAvailable = this->bytesAvailable();
		if (bAvailable == 0)
		{
			Log::Error("data lost");
			return;
		}
        m_InBuffer = this->read(m_FileReceiveInfo.m_totalBytes - m_FileReceiveInfo.m_bytesReceived);
		m_FileReceiveInfo.m_bytesReceived += m_InBuffer.size();
        m_FileReceiveInfo.m_localFile->write(m_InBuffer);
		emit FileBytesReceived(this);
		m_InBuffer.resize(0);
		Log::Info("File: " + m_FileReceiveInfo.m_fileName + "---progress:" + QString::number(((double)m_FileReceiveInfo.m_bytesReceived / (double)m_FileReceiveInfo.m_totalBytes)));
    }


	// 接收数据完成时
	if (m_FileReceiveInfo.m_bytesReceived == m_FileReceiveInfo.m_totalBytes) {
		
		if (m_FileReceiveInfo.m_localFile != nullptr)
		{
			m_FileReceiveInfo.m_localFile->close();
		}
		AfterReceiveFile();
	}
}

void FileSyncerTcpSocket::ResetFileSendInfo()
{
	m_FileSendInfo.Init();
}

void FileSyncerTcpSocket::ResetFileReceiveInfo()
{
	m_FileReceiveInfo.Init();
}

void FileSyncerTcpSocket::AfterReceiveFile()
{
	Log::Info("File Received successed");
	m_FileReceiveInfo.Init();
	m_IsRecevingData = false;
}

void FileSyncerTcpSocket::AfterSendFile()
{
	Log::Info("File sent successed");
	m_FileSendInfo.Init();
	m_IsSendingFile = false;
}

FileSendInfo FileSyncerTcpSocket::GetFileSendInfo() const
{
	return m_FileSendInfo;
}

FileReceiveInfo FileSyncerTcpSocket::GetFileReceiveInfo() const
{
	return m_FileReceiveInfo;
}


QString FileSyncerTcpSocket::GetFileSavePath() const
{
	return m_FileSavePath;
}

void FileSyncerTcpSocket::SetFileSavePath(const QString& path)
{
	if (path == NULL)
	{
		Log::Error("save path is NULL!!!");
	}
	m_FileSavePath = path;

}

