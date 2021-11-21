#pragma once
#include<QString>
#include<qtcpsocket.h>
#include<qdatastream.h>
#include<qfloat16.h>
#include<qobject.h>
#include "SyncFile.h"

namespace FileSyncerSystem
{
	class FileSyncerTcpSocket : public  QTcpSocket
	{
		Q_OBJECT;
	public:
		FileSyncerTcpSocket();
		virtual ~FileSyncerTcpSocket();
		void sendFile(const QString& filePath, qint8 head);
		void sendFile();

		QString GetFileSavePath() const;
		void SetFileSavePath(const QString& path);

		FileSendInfo GetFileSendInfo() const;
		FileReceiveInfo GetFileReceiveInfo() const;
		QStringList filesWaitToSend;
		QDataStream dataBuffer;
	signals:
		void FileBytesWritten(FileSyncerTcpSocket*);
		void FileBytesReceived(FileSyncerTcpSocket*);
	private slots:
		void onConnected();
	public slots:
		void UpdateFileSendProgress();
		void UpdateFileReceiveProgress();
	protected:
		QString m_FileSavePath = NULL;
		FileSendInfo m_FileSendInfo;
		FileReceiveInfo m_FileReceiveInfo;
		QByteArray	m_OutBuffer;
		QByteArray	m_InBuffer;
		bool m_IsRecevingData = false;
		bool m_IsSendingFile = false;
		void ResetFileSendInfo();
		void ResetFileReceiveInfo();
		void AfterReceiveFile();
		void AfterSendFile();
	};
}