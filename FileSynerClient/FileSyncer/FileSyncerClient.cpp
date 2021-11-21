#include"FileSyncerClient.h"
#include<qmessagebox.h>
#include "Log.h"
using namespace FileSyncerSystem;

FileSyncerClient::FileSyncerClient()
{
	tcpClient = new FileSyncerTcpSocketClient();
	connect(tcpClient, SIGNAL(connected()), tcpClient, SLOT(onConnected()));
	connect(tcpClient, SIGNAL(bytesWritten(qint64)), tcpClient, SLOT(OnBytesWritten(qint64)));
	connect(tcpClient, SIGNAL(readyRead()), tcpClient, SLOT(OnReadReady()));
}

FileSyncerClient::FileSyncerClient(const QString& ipAdress, const qint16& port)
{
	tcpClient = new FileSyncerTcpSocketClient();
	connect(tcpClient, SIGNAL(connected()), tcpClient, SLOT(onConnected()));
	connect(tcpClient, SIGNAL(bytesWritten(qint64)), tcpClient, SLOT(OnBytesWritten(qint64)));
	connect(tcpClient, SIGNAL(readyRead()), tcpClient, SLOT(OnReadReady()));
	setTargetAddress(ipAdress, port);
}

bool FileSyncerClient::isConnect()
{
	return tcpClient->waitForConnected();
}

void FileSyncerClient::doSync()
{

}

void FileSyncerClient::doConnect()
{
	tcpClient->connectToHost(m_targetIp, m_targetPort);
	if (!tcpClient->waitForConnected(1000))
	{
		Log::Info("Tcp connnect overTime");
		QMessageBox::about(NULL, QStringLiteral("提示"), QStringLiteral("服务器连接失败"));
	}
}

void FileSyncerClient::doConnect(QString ip, qint16 port)
{
	tcpClient->connectToHost(ip, port);
}

void FileSyncerClient::doSyncSingleFile(const QString& filePath)
{
	tcpClient->sendFile(filePath, FROM_CLIENT_UPLOAD);
}


void FileSyncerClient::setTargetAddress(const QString& ip, const qint16& port)
{
	this->m_targetIp = ip;
	this->m_targetPort = port;
}


void FileSyncerClient::RequestDownloadFile(const QString& fileName)
{
	tcpClient->SendDownloadFileRequest(fileName);
}

void FileSyncerClient::RequestDeleteFile(const QString& fileName)
{
	tcpClient->SendDeleteFileRequest(fileName);
}
void FileSyncerClient::RequestRefresh()
{
	tcpClient->SendRefreshRequest();
}

void FileSyncerClient::SetFileSavePath(const QString& savePath)
{
	tcpClient->SetFileSavePath(savePath);
}