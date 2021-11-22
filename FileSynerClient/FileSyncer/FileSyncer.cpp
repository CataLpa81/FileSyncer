#include "FileSyncer.h"
#include "FileSyncerClient.h"
#include "Log.h"
#include <qstringlistmodel.h>
#include<qfiledialog.h>
#include <qdrag.h>
#include <qevent.h>
#include <qmimedata.h>
#include <qjsondocument.h>
#include <qjsonobject.h>
#include <qmessagebox.h>

using namespace FileSyncerSystem;

FileSyncer* FileSyncer::m_Instance = NULL;

FileSyncer::FileSyncer(QWidget *parent)
    : QMainWindow(parent)
{
    ui.setupUi(this);
}

void FileSyncer::Start()
{
    AnalyzeJson();
    m_FileListModel = new QStringListModel();
    ui.progressBar->setMaximum(10);
    ui.progressBar->setValue(0);
    ui.lineEdit_LocalFilePath->setEnabled(false);
    ui.label_connectState->setText("disconnected");
    ui.btn_connect->setEnabled(true);
    this->setAcceptDrops(true);
    StartClient();
}

void FileSyncer::AnalyzeJson()
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

    m_ip = jsonObject["ipAddress"].toString();
    m_port = jsonObject["port"].toInt();
    m_DefaultSavePath = jsonObject["defaultSavePath"].toString();
    if (m_ip == NULL || m_port == NULL)
    {
        QMessageBox::about(NULL, QStringLiteral("提示"), QStringLiteral("JSON解析失败！"));
        exit(0);
    }
    
}

void FileSyncer::StartClient()
{
    m_Client = new FileSyncerClient();
    m_Client->setTargetAddress(m_ip, m_port);
    connect(m_Client->tcpClient, SIGNAL(connected()), this, SLOT(OnTcpClientConnected()));
    connect(m_Client->tcpClient, SIGNAL(disconnected()), this, SLOT(OnTcpClientDisConnected()));
    connect(ui.btn_upload, SIGNAL(clicked()), this, SLOT(OnBtnUploadClick()));
    connect(ui.btn_download, SIGNAL(clicked()), this, SLOT(OnBtnDownloadClick()));
    connect(ui.btn_downloadall, SIGNAL(clicked()), this, SLOT(OnBtnDownloadAllClick()));
    connect(ui.btn_select, SIGNAL(clicked()), this, SLOT(OnBtnSelectClick()));
    connect(ui.btn_selectLocal, SIGNAL(clicked()), this, SLOT(OnBtnSelectLocalClick()));
    connect(ui.btn_refresh, SIGNAL(clicked()), this, SLOT(OnBtnRefreshClick()));
    connect(ui.btn_delete, SIGNAL(clicked()), this, SLOT(OnBtnDeleteClick()));
    connect(ui.btn_connect, SIGNAL(clicked()), this, SLOT(OnBtnConnectedClick()));


    connect(m_Client->tcpClient, SIGNAL(FileBytesWritten(FileSyncerTcpSocket*)), this, SLOT(OnFileBytesWritten(FileSyncerTcpSocket*)));
    connect(m_Client->tcpClient, SIGNAL(FileBytesReceived(FileSyncerTcpSocket*)), this, SLOT(OnFileBytesReceived(FileSyncerTcpSocket*)));

    connect(m_Client->tcpClient, SIGNAL(FileListReady(QList<FileInfo>)), this, SLOT(OnFileListReady(QList<FileInfo>)));


    m_Client->doConnect();


}

FileSyncer* FileSyncer::getInstance()
{
    if (m_Instance == nullptr)
        m_Instance = new FileSyncer();
    return m_Instance;
}

void FileSyncer::OnTcpClientConnected()
{
    ui.label_connectState->setText("connected");
    ui.btn_connect->setEnabled(false);
    ui.lineEdit_LocalFilePath->setText(m_DefaultSavePath);
    m_Client->SetFileSavePath(m_DefaultSavePath);
    OnBtnRefreshClick();
}

void FileSyncer::OnTcpClientDisConnected()
{
    ui.label_connectState->setText("disconnected");
    ui.btn_connect->setEnabled(true);
}

void FileSyncer::OnBtnSelectClick()
{
    Log::Info("OnBtnSelectClick");

    QString path = QFileDialog::getOpenFileName(this, "select a file");
    ui.lineEdit_FilePath->setText(path);
}

void FileSyncer::OnBtnSelectLocalClick()
{
    Log::Info("OnBtnSelectLocalClick");

    QString path = QFileDialog::getExistingDirectory(this, "select a dir");
    ui.lineEdit_LocalFilePath->setText(path);
    m_Client->SetFileSavePath(path);
}


void FileSyncer::OnBtnUploadClick()
{
    QString path = ui.lineEdit_FilePath->text();
    Log::Info("OnBtnUploadClick");
    QFile file(path);
    if (!file.open(QFile::ReadOnly))
    {
        Log::Error("Client: open file" + path + " fail");
        QMessageBox::about(NULL, QStringLiteral("提示"), QStringLiteral("无法打开文件！"));
        return;
    }
    m_Client->doSyncSingleFile(path);
}


void FileSyncer::OnBtnDownloadClick()
{
    Log::Info("OnBtnDownloadClick");

    QString pathLocal = ui.lineEdit_LocalFilePath->text();
    if (pathLocal == NULL || pathLocal == "")
    {
        QMessageBox::about(NULL, QStringLiteral("提示"), QStringLiteral("没有选择下载路径"));
        Log::Warning("please select a dir");
        return;
    }

    QString targetFileName = GetCurSelectFileName();
    if (targetFileName == NULL)
    {
        QMessageBox::about(NULL, QStringLiteral("提示"), QStringLiteral("没有选择文件"));
        Log::Warning("please select a file");
        return;
    }

    m_Client->RequestDownloadFile(targetFileName);
    
}

void FileSyncer::OnBtnRefreshClick()
{
    Log::Info("OnBtnRefreshClick");

    m_Client->RequestRefresh();

}

void FileSyncer::OnBtnConnectedClick()
{
    Log::Info("OnBtnConnectedClick");
    m_Client->doConnect();
}

void FileSyncer::OnBtnDeleteClick()
{
    Log::Info("OnBtnDeleteClick");

    QString targetFileName = GetCurSelectFileName();
    if (targetFileName == NULL)
    {
        QMessageBox::about(NULL, QStringLiteral("提示"), QStringLiteral("没有选择文件"));
        Log::Warning("please select a file");
        return;
    }

    m_Client->RequestDeleteFile(targetFileName);

}

void FileSyncer::OnBtnDownloadAllClick()
{
    Log::Info("OnBtnDownloadAllClick");
    QString path = QFileDialog::getExistingDirectory();

}

void FileSyncer::OnFileBytesWritten(FileSyncerTcpSocket*)
{
    UpdateFileSendProgress();
}

void FileSyncer::OnFileBytesReceived(FileSyncerTcpSocket*)
{
    UpdateFileReceiveProgress();
}

QString FileSyncer::GetCurSelectFileName()
{
    if (ui.listView_OnlineFiles->currentIndex().row() == -1)
    {
        return NULL;
    }
    QModelIndex index = ui.listView_OnlineFiles->selectionModel()->currentIndex();
    QString targetFileName = m_FileListModel->data(index).toString();
    return targetFileName;
}


void FileSyncer::UpdateFileSendProgress()
{
    FileSendInfo sendInfo= m_Client->tcpClient->GetFileSendInfo();

    ui.progressBar->setMaximum(sendInfo.m_totalBytes);
    ui.progressBar->setValue(sendInfo.m_totalBytes - sendInfo.m_bytesToWrite);

    ui.label_transState->setText("sending");

    if (sendInfo.m_bytesToWrite == 0)
    {
        ui.label_transState->setText("send done");
    }
}

void FileSyncer::UpdateFileReceiveProgress()
{
    FileReceiveInfo receInfo = m_Client->tcpClient->GetFileReceiveInfo();

    ui.progressBar->setMaximum(receInfo.m_totalBytes);
    ui.progressBar->setValue(receInfo.m_bytesReceived);

    ui.label_transState->setText("receiving");

    if (receInfo.m_bytesReceived == receInfo.m_totalBytes)
    {
        ui.label_transState->setText("receive done");
    }
}

void FileSyncer::OnFileListReady(QList<FileInfo> _fileList)
{
    Log::Info("OnFileListReady");

    m_fileInfoList = _fileList;
    QStringList list;
    for (auto fileInfo : m_fileInfoList)
    {
        list.append(fileInfo.fileName);
    }

    m_FileListModel->setStringList(list);
    
    ui.listView_OnlineFiles->setModel(m_FileListModel);
}

void FileSyncer::dragMoveEvent(QDragMoveEvent* event)
{

}

void FileSyncer::mousePressEvent(QMouseEvent* event)
{
    if (event->button() == Qt::LeftButton)
    {
        start_pos = event->pos();

    }
}

void FileSyncer::mouseMoveEvent(QMouseEvent* event)
{
    if (event->buttons() & Qt::LeftButton) {
        int distance = (event->pos() - start_pos).manhattanLength();
        if (distance >= QApplication::startDragDistance())
        {
            /* Drag */
            QMimeData* mime_data = new QMimeData;

            mime_data->setData("text/uri-list", "");

            QDrag* drag = new QDrag(this);
            drag->setMimeData(mime_data);
            drag->exec(Qt::CopyAction);
        }
    }
}

void FileSyncer::mouseReleaseEvent(QMouseEvent* event)
{

}


void FileSyncer::dragEnterEvent(QDragEnterEvent* event)
{
    event->acceptProposedAction();
}

void FileSyncer::dropEvent(QDropEvent* event)
{
    QString path = event->mimeData()->urls().first().toString();
    path = path.replace("file:///", "");
    ui.lineEdit_FilePath->setText(path);
}

void FileSyncer::OnDragFileOut()
{
    QString filename = GetCurSelectFileName();
    QMimeData* mimeData = new QMimeData();
}