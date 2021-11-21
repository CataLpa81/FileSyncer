#pragma once
#include "SyncFile.h"
#include "FileSyncerTcpSocket.h"
#include<QString>
#include<qtcpsocket.h>
#include<qdatastream.h>
#include<qthread.h>
#include<qdir.h>
#include<qfilesystemwatcher.h>
#include "FileSyncerTcpSocketClient.h"

namespace FileSyncerSystem
{

	class FileSyncerClient : public QObject
	{
		Q_OBJECT
	public:
		FileSyncerClient();
		FileSyncerClient(const QString& ipAdress, const qint16& port);
		void doSync();
		void doSyncSingleFile(const QString& filePath);
		void setTargetAddress(const QString& ip, const qint16& port);
		void doConnect();
		void doConnect(QString ip, qint16 port);
		bool isConnect();

		void RequestDownloadFile(const QString& fileName);
		void RequestDeleteFile(const QString& fileName);
		void RequestRefresh();

		void SetFileSavePath(const QString&);

		FileSyncerTcpSocketClient* tcpClient;
	private:
		QString m_targetIp;
		qint16 m_targetPort;
	};
}
