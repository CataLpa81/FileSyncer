#pragma once
#include "FileSyncerTcpSocket.h"

namespace FileSyncerSystem
{
	class FileSyncerTcpSocketClient : public FileSyncerTcpSocket
	{
		Q_OBJECT
	public:
		void SendDownloadFileRequest(const QString& fileName);
		void SendDeleteFileRequest(const QString& fileName);
		void SendRefreshRequest();
	signals:
		void FileListReady(QList<FileInfo>);
	private slots:
		void OnBytesWritten(qint64);
		void OnReadReady();
	private:
		void AnalyzeDataHead();
		void ProcessFileHead();
		void ProcessFileListHead();
	};
}