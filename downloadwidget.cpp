#include "downloadwidget.h"

#include <QDesktopServices>
#include <QDir>
#include <QFile>
#include <QFileInfo>
#include <QMessageBox>
#include <QSizePolicy>


DownloadWidget::DownloadWidget(const QString &rootDir, QNetworkReply *reply, QWidget *parent) :
    QWidget(parent), reply(reply), rootDir(rootDir)
{
    QUrl url = reply->url();
    localFile = QFileInfo(rootDir+"/"+url.path().section('/',3));


    QHBoxLayout *layout = new QHBoxLayout();

    label = new QLabel();
    icon = new QLabel();
    icon->setPixmap(QPixmap(":/icons/status-loading.png"));
    label->setText(reply->url().toDisplayString().section('/',5));
    buttonOpenFile = new QPushButton(tr("Open File"));
    buttonOpenFile->setVisible(false);
    buttonErrorDescription = new QPushButton(tr("Error Description"));
    buttonErrorDescription->setVisible(false);

    progress = new QProgressBar();
    progress->setRange(0,0);
    connect(reply, &QNetworkReply::downloadProgress,
            this, &DownloadWidget::slotDownloadProgress);
    connect(reply, &QNetworkReply::finished,
            this, &DownloadWidget::slotDownloadFinished);
    connect(reply, &QNetworkReply::readyRead,
            this, &DownloadWidget::slotReadyRead);
    connect(buttonOpenFile, &QPushButton::clicked,
            this, &DownloadWidget::openFile);
    connect(buttonErrorDescription, &QPushButton::clicked,
            this, &DownloadWidget::showErrorDescription);

    layout->setMargin(0);
    layout->addWidget(icon);
    layout->addWidget(label,1);
    layout->addWidget(buttonErrorDescription);
    layout->addWidget(buttonOpenFile);
    layout->addWidget(progress);
    this->setLayout(layout);

    mkPath();
}

DownloadWidget::~DownloadWidget()
{
    // If the download is not finished, or the download failed
    if(reply->isRunning()||reply->error()!=QNetworkReply::NoError){
        reply->abort();
        if(localFile.exists()){
            // Delete the local (probably incomplete) file.
            QFile::remove(localFile.filePath());
        }
    }
    reply->deleteLater();
}

void DownloadWidget::slotDownloadProgress(qint64 bytesReceived, qint64 bytesTotal){
    progress->setMaximum(bytesTotal);
    progress->setValue(bytesReceived);
}

void DownloadWidget::slotDownloadFinished(){
    progress->setMaximum(100);
    progress->setValue(100);

    if (reply->error()) {
        buttonErrorDescription->setVisible(true);
        qDebug() << reply->errorString(); // TODO: Propper error handling
        icon->setPixmap(QPixmap(":/icons/status-error.png"));
        if(localFile.exists()){
            // Delete the local (probably incomplete) file.
            QFile::remove(localFile.filePath());
        }
    }
    else {
        if(emptyFile)
            saveToDisk(localFile.filePath(),reply);

        icon->setPixmap(QPixmap(":/icons/status-ok.png"));
        buttonOpenFile->setVisible(true);
    }
}

void DownloadWidget::slotReadyRead()
{
    if (reply->error()) {
        qDebug() << reply->errorString();
    } else {
        savePartToDisk(localFile.filePath(), reply);
    }
}

bool DownloadWidget::saveToDisk(const QString &filename, QIODevice *data)
{
    QFile file(filename);
    if (!file.open(QIODevice::WriteOnly)) {
        qDebug() << file.errorString();
        return false;
    }

    file.write(data->readAll());
    file.close();

    return true;
}

bool DownloadWidget::savePartToDisk(const QString &filename, QIODevice *data)
{
    emptyFile = false;
    QFile file(filename);
    if (!file.open(QIODevice::WriteOnly|QIODevice::Append)) {
        qDebug() << file.errorString();
        return false;
    }
    file.write(data->readAll());
    file.close();

    return true;
}

void DownloadWidget::mkPath() const
{
    QDir dir(rootDir);
    dir.mkpath(localFile.path());
}

void DownloadWidget::openFile() const
{
    QDesktopServices::openUrl(QUrl("file:"+localFile.filePath()));
}

void DownloadWidget::showErrorDescription(){
    QMessageBox::critical(static_cast<QWidget*>(this),tr("Network Error %1").arg(reply->error()),
                          reply->errorString());
}
