#ifndef DOWNLOADWIDGET_H
#define DOWNLOADWIDGET_H

#include <QLabel>
#include <QProgressBar>
#include <QVBoxLayout>
#include <QWidget>
#include <QNetworkReply>
#include <QFileInfo>
#include <QPushButton>

class DownloadWidget : public QWidget
{
    Q_OBJECT
public:
    explicit DownloadWidget(const QString &rootDir, QNetworkReply *reply, QWidget *parent = nullptr);
    ~DownloadWidget();
signals:

public slots:
private slots:
    void slotDownloadProgress(qint64 bytesReceived, qint64 bytesTotal);
    void slotDownloadFinished();
    void slotReadyRead();
    void openFile() const;
    void showErrorDescription();
private:
    QNetworkReply *reply;
    QLabel *icon;
    QLabel *label;
    QPushButton *buttonOpenFile;
    QPushButton *buttonErrorDescription;
    QProgressBar *progress;
    const QString rootDir;
    QFileInfo localFile;
    bool emptyFile = true;
    bool saveToDisk(const QString &filename, QIODevice *data);
    bool savePartToDisk(const QString &filename, QIODevice *data);
    void mkPath() const;
};

#endif // DOWNLOADWIDGET_H
