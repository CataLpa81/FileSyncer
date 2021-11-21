#pragma once

#include<qfile.h>
#include<QString>

namespace FileSyncerSystem
{
	//头部标识
	const qint8 FROM_CLIENT_UPLOAD = 11;
	const qint8 FROM_CLIENT_DOWNLOAD = 12;
	const qint8 FROM_CLIENT_DOWNLOADALL = 13;
	const qint8 FROM_CLIENT_GETFILELIST = 14;
	const qint8 FROM_CLIENT_DELETEFILE = 15;

	const qint8 FROM_SERVER_FILELIST = 21;
	const qint8 FROM_SERVER_FILE = 22;
	
	const qint64 PAYLOADSIZE = 64 * 1024;

	

	struct dataSendInfo
	{
		void Init();
		quint32 m_totalBytes;
		quint32 m_bytesWritten;
		quint32 m_bytesToWrite;
	};

	struct dataReceiveInfo
	{
		void Init();
		quint32 m_totalBytes;
		quint32 m_bytesReceived;
	};

	struct FileSendInfo {
		void Init();
		QFile* m_localFile;
		qint64 m_totalBytes;
		qint64 m_bytesToWrite;
		QString m_filePath;
		QString m_fileName;
	};

	struct FileReceiveInfo
	{
		void Init();
		qint64 m_totalBytes;
		qint64 m_bytesReceived;
		qint64 m_fileNameSize;
		QString m_fileName;
		QFile* m_localFile;
	};

	struct FileInfo
	{
		FileInfo(QString _fileName, QString _fileSuffix) :
			fileName(_fileName),
			fileSuffix(_fileSuffix) {};
		QString fileName;
		QString fileSuffix;
	};
}



