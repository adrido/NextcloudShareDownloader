#include "mainwindow.h"
#include "./ui_mainwindow.h"

#include <QtNetwork/QNetworkAccessManager>
#include <QtNetwork/qnetworkreply.h>
#include <QDesktopServices>
#include <QFileDialog>
#include <QMessageBox>
#include <QString>
#include <QTimer>

void MainWindow::initStatusbar()
{
    statusProgressBar = new QProgressBar(this);
    statusLabelRemoteDirs = new QLabel(this);
    statusLabelRemoteFiles = new QLabel(this);

    statusProgressBar->setFixedHeight(ui->statusbar->height()-4);

    statusLabelRemoteDirs->setText(statusLabelRemoteDirsText.arg('?'));
    statusLabelRemoteFiles->setText(statusLabelRemoteFilesText.arg('?'));

    ui->statusbar->addPermanentWidget(statusLabelRemoteFiles);
    ui->statusbar->addPermanentWidget(statusLabelRemoteDirs);
    ui->statusbar->addPermanentWidget(statusProgressBar,0);
}

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    dirIter = new RemoteDirIterator(&messageLogger,this);

    connect(&messageLogger, SIGNAL(signalLogMsg(MessageLogger::LogLevel, QString)),
            this, SLOT(slotLogMsg(MessageLogger::LogLevel, QString)));

    initStatusbar();

    connect(dirIter, SIGNAL(downloadCompleted()), this, SLOT(dirDownloadCompleted()));
    connect(dirIter, SIGNAL(progressChanged(int, int)),this,SLOT(slotProgressChanged(int, int)));
    connect(dirIter, SIGNAL(filesFoundChanged(int)),
            this, SLOT(slotFilesFound(int)));
    readSettings();
    ui->actionDownload->setEnabled(false);
}

MainWindow::~MainWindow()
{
    writeSettings();
    delete ui;
}

void MainWindow::startDownload()
{
    statusProgressBar->setRange(0,filesToDownload.length());
    auto nwm = new QNetworkAccessManager();
    const QUrl url = QUrl(ui->lineEditUrl->text(),QUrl::ParsingMode::StrictMode);
    const QUrl webdavUrl = toWebDavUrl(url);
    for(const QString &file : filesToDownload){
        QUrl url = QUrl(webdavUrl);
        url.setPath(webdavUrl.path()+file);
        auto reply = nwm->get(QNetworkRequest(url));
        auto dlw = new DownloadWidget(getLocalDir(), nwm, reply);
        ui->scrollAreaWidgetContents->layout()->addWidget(dlw);
    }
    connect(nwm, &QNetworkAccessManager::finished,
            this, &MainWindow::fileDownloaded);
}

void MainWindow::fileDownloaded()
{
    filesDownloaded ++;
    statusProgressBar->setValue(filesDownloaded);
    if(filesDownloaded >= filesToDownload.length() )
        QTimer::singleShot(1, this, &MainWindow::fileDownloadCompleted);//fileDownloadCompleted();
}

void MainWindow::dirDownloadCompleted(){
    if(dirIter->getHaveErrors())
    {
        reset();
        return;
    }

    QStringList remoteFiles = dirIter->getFileList();
    for(const QString& file : remoteFiles){
        QString localFileName = file.section('/',3); //Removes /index.php/webdav/
        //qDebug()<<localFileName;
        if(!QFile::exists(getLocalDir()+"/"+localFileName)){
            //qDebug() << "Datei" <<localFileName <<" existiert nicht! Runterladen!!";
            filesToDownload.append(localFileName);
        }
    }
    if(filesToDownload.length() >=1){
        auto button = QMessageBox::question(this, tr("Download"),
                                            tr("There are %n File(s) that are not found. \n"
                                               "Do you want to Download?", "", filesToDownload.length())
                                            );
        if(button == QMessageBox::Yes){
            startDownload();
        }
        else{
            ui->actionDownload->setEnabled(true);
        }
    }
    else{
            QMessageBox::information(this, tr("Done"), tr("All files are up to date."));
            reset();
    }


}

void MainWindow::fileDownloadCompleted(){
    QMessageBox::information(this, tr("Download"),
                          tr("Download completed.")
                          );
    reset();
}

void MainWindow::slotProgressChanged(int current, int max)
{
    statusProgressBar->setRange(0, max);
    statusProgressBar->setValue(current);
    statusLabelRemoteDirs->setText(statusLabelRemoteDirsText.arg(max));

}

QUrl MainWindow::toWebDavUrl(const QUrl& url) {
    QString path = url.path(); // looks like /s/blahblah
    //QString host = url.host(); // nextcloud.blah.de
    QString username = path.section('/',-1); // this is blahblah which is also the username
    QUrl webdavurl = QUrl(url);
    webdavurl.setPath("/public.php/webdav/");
    webdavurl.setUserName(username);
    return webdavurl;
}

QString MainWindow::getLocalDir()
{
    QString userPath = ui->lineEditDir->text();
    if(userPath.isEmpty()){
        userPath = QDir::currentPath();
    }
    return userPath;
}

void MainWindow::startRefresh()
{
    reset();
    readBlackList();
    const QUrl url = QUrl(ui->lineEditUrl->text(),QUrl::ParsingMode::StrictMode);
    if(url.isEmpty() || !url.isValid() || url.isLocalFile() || url.isRelative()){
        QMessageBox::information(this, tr("Invalid URL"), tr("Please enter a valid URL."));
        return;
    }
    ui->actionRefresh->setDisabled(true);
    ui->actionDownload->setDisabled(true);
    const QUrl webdavUrl = toWebDavUrl(url);
    dirIter->setBlackList(blackList);
    dirIter->setRootUrl(webdavUrl);
    dirIter->downloadDir(webdavUrl);
}

void MainWindow::on_actionRefresh_triggered()
{
    startRefresh();
}

void MainWindow::reset()
{
    ui->statusbar->showMessage(tr("Ready"));
    filesToDownload.clear();
    filesDownloaded = 0;
    dirIter->reset();
    ui->actionRefresh->setDisabled(false);
    ui->actionDownload->setEnabled(false);
}

void MainWindow::on_actionAbout_triggered()
{
    const QString aboutText = tr("Nexctloud share downloader")+"<br>"+
            tr("Version: %1").arg(qApp->applicationVersion())+ "<br>"+
            tr("Qt Version: %1").arg(qVersion())+"<br><br>"+
            "<a href='https://github.com/adrido/NextcloudShareDownloader/releases'>"+tr("Check for updates")+"</a>";


    QMessageBox::about(this, tr("About"), aboutText);
}

void MainWindow::writeSettings()
{
    settings.setValue("dir", ui->lineEditDir->text());
    settings.setValue("url", ui->lineEditUrl->text());
    settings.beginGroup("MainWindow");

    if(isMaximized()){
        settings.setValue("maximized", isMaximized());
    }
    else {
        settings.setValue("size", size());
        settings.setValue("pos", pos());
    }
    settings.setValue("maximized", isMaximized());
    settings.endGroup();
}

void MainWindow::readSettings()
{
    ui->lineEditDir->setText(settings.value("dir", QStandardPaths::writableLocation(QStandardPaths::DocumentsLocation)).toString());
    ui->lineEditUrl->setText(settings.value("url", QString()).toString());
    settings.beginGroup("MainWindow");
    if(settings.value("maximized",false).toBool()){
        this->showMaximized();
    }
    else{
        resize(settings.value("size", QSize(800, 600)).toSize());
        move(settings.value("pos", QPoint(200, 200)).toPoint());
    }

    settings.endGroup();
}

void MainWindow::on_actionOpen_Folder_triggered()
{
    QDesktopServices::openUrl(QUrl(ui->lineEditDir->text()));
}

void MainWindow::on_actionOpen_Url_triggered()
{
    QDesktopServices::openUrl(QUrl(ui->lineEditUrl->text()));
}

void MainWindow::slotLogMsg(MessageLogger::LogLevel ll, const QString& msg)
{
    ui->statusbar->showMessage(msg);
}

void MainWindow::slotFilesFound(int nFiles)
{
    statusLabelRemoteFiles->setText(statusLabelRemoteFilesText.arg(nFiles));
}

void MainWindow::on_pushButton_2_clicked()
{
    QString dir = QFileDialog::getExistingDirectory(this, QString(),
                                                    ui->lineEditDir->text(),
                                                    QFileDialog::ShowDirsOnly
                                                    | QFileDialog::DontResolveSymlinks);
    if(!dir.isEmpty())
        ui->lineEditDir->setText(dir);
}

void MainWindow::on_actionReport_Issue_triggered()
{
    QDesktopServices::openUrl(QUrl("https://github.com/adrido/NextcloudShareDownloader/issues"));
}

void MainWindow::on_actionDownload_triggered()
{
    startDownload();
}

void MainWindow::readBlackList()
{
    blackList.clear();
    QString dir = ui->lineEditDir->text();
    QFile fileBlackList(dir + "/BlackList.txt");
    if(fileBlackList.open(QFile::ReadOnly | QFile::Text))
    {
        QTextStream stream( &fileBlackList );
        while(!stream.atEnd()){
            QString line = stream.readLine();

            // Solange die Zeile nicht mit # beginnt oder leer ist
            if(line.startsWith('#') || line.isEmpty()){
                continue;
            }
            blackList.append(line);
            qDebug() << blackList;
        }
    }
}


void MainWindow::on_actionEditBlackList_triggered()
{
    QString dir = ui->lineEditDir->text();
    QFile fileBlackList(dir + "/BlackList.txt");

    // Falls es diese datei nicht gibt
    if(!fileBlackList.exists())
    {
        if(fileBlackList.open(QFile::WriteOnly | QFile::Text))
        {
            QTextStream stream( &fileBlackList );
                stream << tr("# This is the blacklist of %1.").arg(qAppName()) << endl
                       << tr("# You can write the name of the file you want to ignore to this file.") << endl
                       << tr("# One entry per line.") << endl
                       << tr("# Lines that start with a # are ignored.") << endl;
                fileBlackList.close();
        }

    }
    QDesktopServices::openUrl(QUrl(fileBlackList.fileName()));

}
