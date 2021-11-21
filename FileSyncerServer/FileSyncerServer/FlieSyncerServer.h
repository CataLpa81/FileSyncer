#pragma once
#include<qtcpserver.h>
#include<qtcpsocket.h>
#include<qthread.h>
#include<QSet>
#include "FileSyncerTcpSocketServer.h"

namespace FileSyncerSystem
{

	class FileSyncerTcpServer : public QTcpServer
	{
		Q_OBJECT
	public:
		void SetFileSavePath(const QString& path);
	protected:
		virtual void incomingConnection(qintptr handle);
	public slots:
		void OnClientDisconnect(FileSyncerTcpSocketServer*);
	private:
		QSet<FileSyncerTcpSocketServer*> m_SocketList;
		QString m_fileSavePath;

	};

	class FileSyncerServer : public QObject
	{
		Q_OBJECT
	public:
		FileSyncerServer(QObject* parent = nullptr);
		void Start();
	private:
		void AnalyzeJson();

		FileSyncerTcpServer* m_TcpServer;
		int m_port;
		QString m_fileSavePath;
	};

	class FileSyncerThreadServer : public QThread
	{
	public:
		void run();
	private:
		FileSyncerServer* m_Server;
	};

}

