#include "downloadwidget.h"

#include <QDesktopServices>
#include <QDir>
#include <QFile>
#include <QFileInfo>
#include <QSizePolicy>


DownloadWidget::DownloadWidget(const QString &rootDir, QNetworkReply *reply, QWidget *parent) :
    QWidget(parent), reply(reply), rootDir(rootDir)
{
    QUrl url = reply->url();
    localFile = QFileInfo(rootDir+"/"+url.path().section('/',3));


    QHBoxLayout *layout = new QHBoxLayout();

    label = new QLabel();
    icon = new QLabel();
    icon->setPixmap(QPixmap(":/icons/status-loading.svg"));
    label->setText(reply->url().toDisplayString().section('/',5));
    buttonOpenFile = new QPushButton("Open");
    buttonOpenFile->setVisible(false);

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

    layout->setMargin(0);
    layout->addWidget(icon);
    layout->addWidget(label,1);
    layout->addWidget(buttonOpenFile);
    layout->addWidget(progress);
    this->setLayout(layout);

    mkPath();
}

DownloadWidget::~DownloadWidget()
{
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
        qDebug() << reply->errorString(); // TODO: Propper error handling
        icon->setPixmap(QPixmap(":/icons/status-error.svg"));
    }
    else {
        if(emptyFile)
            saveToDisk(localFile.filePath(),reply);

        icon->setPixmap(QPixmap(":/icons/status-ok.svg"));
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
