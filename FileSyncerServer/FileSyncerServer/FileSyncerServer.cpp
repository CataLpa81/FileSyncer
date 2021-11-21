#include "Log.h"
#include "FlieSyncerServer.h"
#include "FileSyncerTcpSocketServer.h"
#include "DataConst.h"
#include <qjsondocument.h>
#include <qjsonobject.h>
#include<iostream>
using namespace FileSyncerSystem;

FileSyncerServer::FileSyncerServer(QObject* parent) : QObject(parent)
{
	m_TcpServer = new FileSyncerTcpServer();
}

void FileSyncerServer::Start()
{
	AnalyzeJson();
	qDebug() << "FileSyncServer Starting -3-" << QThread::currentThread() << endl;
	
	if (m_TcpServer->listen(QHostAddress::Any, m_port))
	{
		Log::Info("Lisitening...");
	}
}

void FileSyncerServer::AnalyzeJson()
{
	QFile file("config.json");
	file.open(QIODevice::ReadOnly | QIODevice::Text);
	QString value = file.readAll();
	file.close();

	QJsonParseError parseJsonErr;
	QJsonDocument document = QJsonDocument::fromJson(value.toUtf8(), &parseJsonErr);
	if (!(parseJsonErr.error == QJsonParseError::NoError)) {
		Log::Error("json error");
		return;
	}

	QJsonObject jsonObject = document.object();

	QString savePath = jsonObject["FileSavePath"].toString();
	m_TcpServer->SetFileSavePath(savePath);
	int port = jsonObject["port"].toInt();
	m_port = port;
}

void FileSyncerTcpServer::incomingConnection(qintptr socketDescriptor)
{
	qDebug() << "get a connection" << endl;
	FileSyncerTcpSocketServer* socket = new FileSyncerTcpSocketServer();
	socket->SetFileSavePath(m_fileSavePath);
	connect(socket, SIGNAL(connected()), socket, SLOT(onConnected()));
	connect(socket, SIGNAL(bytesWritten(qint64)), socket, SLOT(OnBytesWritten(qint64)));
	connect(socket, SIGNAL(readyRead()), socket, SLOT(AnalyzeDataHead()));
	connect(
		socket,
		SIGNAL(disconnected()),
		socket,
		SLOT(OnDisconnect()));

	connect(
		socket, 
		SIGNAL(clientDisconnected(FileSyncerTcpSocketServer*)), 
		this,
		SLOT(OnClientDisconnect(FileSyncerTcpSocketServer*)));

	socket->setSocketDescriptor(socketDescriptor);

	Log::Info(QString::number(socket->socketDescriptor()));
	m_SocketList.insert(socket);
}

void FileSyncerTcpServer::OnClientDisconnect(FileSyncerTcpSocketServer* socket)
{
	QString hostAddress = socket->peerAddress().toString();
	m_SocketList.remove(socket);
	socket->close();
	socket->deleteLater();
	Log::Info("client from " + hostAddress + " has been removed from Server");
	Log::Info("current client count on server:" + QString::number(m_SocketList.size()));
}

void FileSyncerTcpServer::SetFileSavePath(const QString& path)
{
	if (path == NULL)
	{
		Log::Error("File save path cannot be NULL");
		return;
	}
	m_fileSavePath = path;
}


void FileSyncerThreadServer::run()
{
	m_Server = new FileSyncerServer();
	m_Server->Start();
	exec();
}



