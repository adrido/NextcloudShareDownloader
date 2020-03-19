#ifndef REMOTEDIRITERATOR_H
#define REMOTEDIRITERATOR_H

#include <QObject>
#include <QtNetwork/QtNetwork>
#include "messagelogger.h"

class RemoteDirIterator : public QObject
{
    Q_OBJECT
    QNetworkAccessManager *manager;
    QVector<QNetworkReply *> currentDownloads;
    QUrl rootUrl;
    QStringList files;
    MessageLogger *messageLogger;
    int maxDownloads = 0;
    int maxDownloadsCompleted = 0;
    bool haveErrors = false;
    QStringList blackList;
public:
    explicit RemoteDirIterator( MessageLogger *messageLogger, QObject *parent = nullptr);
    void setRootUrl(const QUrl &url);
    void downloadDir(const QUrl &url);

    QStringList getFileList() const;
    void reset();
    bool getHaveErrors() const;
    void setBlackList(const QStringList &list);
    bool isBlackListed(const QString &s);

signals:
    void downloadCompleted();
    void statusChanged(QString text);
    void progressChanged(int current, int max);
    void filesFoundChanged(int nFiles);

public slots:
    void downloadFinished(QNetworkReply *reply);
    void sslErrors(const QList<QSslError> &errors);
};

#endif // REMOTEDIRITERATOR_H
