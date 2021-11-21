#pragma once
#include<qtcpsocket.h>
#include "FileSyncerTcpSocket.h"

namespace FileSyncerSystem
{

	class FileSyncerTcpSocketServer : public FileSyncerTcpSocket
	{
		Q_OBJECT;
	signals:
		void clientDisconnected(FileSyncerTcpSocketServer*);
	public slots:
		void AnalyzeDataHead();
	private slots:
		void MSGError(QAbstractSocket::SocketError);
		void OnDisconnect();
		void OnBytesWritten(qint64);
	private:
		dataSendInfo m_DataSendInfo;
		dataReceiveInfo m_DataReceiveInfo;
		QString m_FilePath;
		QString GetFilePath();
		void setFilePath(const QString& filePath);
		void ProcessUploadHead();
		void ProcessDownloadHead();
		void ProcessGetFileListHead();
		void ProcessDeleteFileHead();

		QString PackFileName(const QString&);
	};

}