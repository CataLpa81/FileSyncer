#pragma once

#include <QtWidgets/QMainWindow>
#include "ui_FileSyncer.h"
#include "FileSyncerClient.h"
#include <qdrag.h>
#include <qstringlistmodel.h>

namespace FileSyncerSystem
{
    class FileSyncer : public QMainWindow
    {
        Q_OBJECT
    public:
        static FileSyncer* getInstance();
        void Start();
        void StartClient();
    public slots:
        void OnTcpClientConnected();
        void OnTcpClientDisConnected();

        void OnBtnSelectClick();
        void OnBtnSelectLocalClick();
        void OnBtnUploadClick();
        void OnBtnDownloadClick();
        void OnBtnRefreshClick();
        void OnBtnDownloadAllClick();
        void OnBtnDeleteClick();
        void OnBtnConnectedClick();

        void OnFileBytesWritten(FileSyncerTcpSocket*);
        void OnFileBytesReceived(FileSyncerTcpSocket*);

        void OnFileListReady(QList<FileInfo>);
    private:
        void virtual dragEnterEvent(QDragEnterEvent* event);
        void virtual dropEvent(QDropEvent* event);
        void virtual dragMoveEvent(QDragMoveEvent* event);

        void virtual mousePressEvent(QMouseEvent* event);
        void virtual mouseMoveEvent(QMouseEvent* event);
        void virtual mouseReleaseEvent(QMouseEvent* event);

        void OnDragFileOut();
        void UpdateFileSendProgress();
        void UpdateFileReceiveProgress();
        void AnalyzeJson();
        QString GetCurSelectFileName();
        QList<FileInfo> m_fileInfoList;
        static FileSyncer* m_Instance;
        FileSyncerClient* m_Client;
        QStringListModel* m_FileListModel;
        FileSyncer(QWidget* parent = Q_NULLPTR);
        Ui::FileSyncerClass ui;

        QString m_ip;
        qint16 m_port;
        QString m_DefaultSavePath;

        QPoint start_pos;
    };
}
