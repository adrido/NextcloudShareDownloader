#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QFileSystemModel>
#include <QDir>
#include <QNetworkReply>
#include <QSettings>
#include <QProgressBar>
#include <QLabel>
#include <QListWidget>
#include "remotediriterator.h"
#include "messagelogger.h"
#include "downloadwidget.h"

QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

    void getRemoteDir(const QString &path);

    QString getLocalDir();
    void startRefresh();
    
    void initStatusbar();

public slots:
    void dirDownloadCompleted();
    void fileDownloadCompleted();
    void slotFilesFound(int nFiles);
private slots:
    QUrl toWebDavUrl(const QUrl& url);

    void on_actionRefresh_triggered();

    void on_actionAbout_triggered();

    void on_actionOpen_Folder_triggered();

    void on_actionOpen_Url_triggered();

    void slotProgressChanged(int current, int max);
    void slotLogMsg(MessageLogger::LogLevel ll, const QString& msg);

    void fileDownloaded();
    void on_pushButton_2_clicked();

    void on_actionReport_Issue_triggered();

    void on_actionDownload_triggered();

private:
    Ui::MainWindow *ui;
    RemoteDirIterator *dirIter;
    QProgressBar *statusProgressBar;
    QLabel *statusLabelRemoteDirs;
    QLabel *statusLabelRemoteFiles;
    const QString statusLabelRemoteDirsText = "📂 %1";
    const QString statusLabelRemoteFilesText = "📄 %1";
    void reset();
    QStringList filesToDownload;
    int filesDownloaded = 0;
    QSettings settings;
    MessageLogger messageLogger;
    void writeSettings();
    void readSettings();
    void startDownload();
};
#endif // MAINWINDOW_H
