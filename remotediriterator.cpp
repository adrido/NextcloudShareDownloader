#include "remotediriterator.h"
#include "propfindparser.h"

RemoteDirIterator::RemoteDirIterator(MessageLogger *logger, QObject *parent) : QObject(parent)
{
    manager = new QNetworkAccessManager(this);
    connect(manager, SIGNAL(finished(QNetworkReply*)), this, SLOT(downloadFinished(QNetworkReply*)));
    messageLogger = logger;
    //connect(manager, &QNetworkAccessManager::finished,
    //         this, &RemoteDirIterator::downloadFinished);
}

void RemoteDirIterator::setRootUrl(const QUrl &url)
{
    rootUrl = url;
}

void RemoteDirIterator::downloadDir(const QUrl &url)
{
    qDebug() << "Downloading" << url;
    messageLogger->info("📁 " + url.path().section('/',3));
    QNetworkRequest request(url);
    QNetworkReply *reply = manager->sendCustomRequest(request, "PROPFIND");


    connect(reply, SIGNAL(sslErrors(QList<QSslError>)),
            SLOT(sslErrors(QList<QSslError>)));
    connect(reply, QOverload<QNetworkReply::NetworkError>::of(&QNetworkReply::error),
            this, &RemoteDirIterator::slotError);
    currentDownloads.append(reply);

    // Increment downloads for progressbar;
    maxDownloads++;
    emit progressChanged(maxDownloadsCompleted, maxDownloads);
}

void RemoteDirIterator::downloadFinished(QNetworkReply *reply)
{
    if (reply->error()) {
        qDebug() << reply->errorString();
    } else {
        QByteArray a = reply->readAll();
        PropFindParser p(a);
        QList<PropItem> list = p.getList();
        list.pop_front(); // Remove first Item (its the same dir)
        for(PropItem item : list){
            if(item.isDir()){
                QUrl url(rootUrl);
                url.setPath(item.href);
                downloadDir(url);
            }
            else {
                files.append(item.href);
                emit filesFoundChanged(files.length());
            }

        }
    }

    // Free RAM
    currentDownloads.removeAll(reply);
    reply->deleteLater();
    maxDownloadsCompleted++;
    emit progressChanged(maxDownloadsCompleted, maxDownloads);

    if (currentDownloads.isEmpty()) {
        // all downloads finished
        emit downloadCompleted();
    }
}

void RemoteDirIterator::slotError(QNetworkReply::NetworkError code)
{
    qDebug()<< "Error "<<code;
}

void RemoteDirIterator::sslErrors(const QList<QSslError> &errors)
{
    for (const QSslError &error : errors)
        qDebug() << "SSL error: " << error.errorString();
}

QStringList RemoteDirIterator::getFileList(){
    return files;
}

void RemoteDirIterator::reset(){
    files.clear();
    maxDownloads = 0;
    maxDownloadsCompleted = 0;
}
