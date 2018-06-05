/*
 * filedownloader.cpp
 *
 *  Created on: Jun 21, 2016
 *      Author: stan
 */

#include<iostream>
#include "filedownloader.h"

using namespace std;

#include <QtWidgets>
#include <QtNetwork>

#include "filedownloader.h"



HttpWindow::HttpWindow(QWidget *parent)
    : QDialog(parent)
{

    urlLineEdit = new QLineEdit("hsa00010");

    urlLabel = new QLabel(tr("&KEGG Entry:"));
    urlLabel->setBuddy(urlLineEdit);
    statusLabel = new QLabel(tr("Please enter the entry of KEGG you want to "
                                "download."));
    statusLabel->setWordWrap(true);

    downloadButton = new QPushButton(tr("Download"));
    downloadButton->setDefault(true);
    quitButton = new QPushButton(tr("Quit"));
    quitButton->setAutoDefault(false);

    buttonBox = new QDialogButtonBox;
    buttonBox->addButton(downloadButton, QDialogButtonBox::ActionRole);
    buttonBox->addButton(quitButton, QDialogButtonBox::RejectRole);

    connect(urlLineEdit, SIGNAL(textChanged(QString)),
            this, SLOT(enableDownloadButton()));

    connect(downloadButton, SIGNAL(clicked()), this, SLOT(downloadFile()));
    connect(quitButton, SIGNAL(clicked()), this, SLOT(close()));

    QHBoxLayout *topLayout = new QHBoxLayout;
    topLayout->addWidget(urlLabel);
    topLayout->addWidget(urlLineEdit);

    QVBoxLayout *mainLayout = new QVBoxLayout;
    mainLayout->addLayout(topLayout);
    mainLayout->addWidget(statusLabel);
    mainLayout->addWidget(buttonBox);
    setLayout(mainLayout);

    setWindowTitle(tr("HTTP"));
    urlLineEdit->setFocus();
}

void HttpWindow::startRequest(QUrl url)
{
    reply = qnam.get(QNetworkRequest(url));
    connect(reply, SIGNAL(finished()),
            this, SLOT(httpFinished()));
    connect(reply, SIGNAL(readyRead()),
            this, SLOT(httpReadyRead()));
    connect(reply, SIGNAL(downloadProgress(qint64,qint64)),
            this, SLOT(updateDataReadProgress(qint64,qint64)));
}

void HttpWindow::downloadFile()
{
    url = QString("http://www.kegg.jp/kegg-bin/download?entry=")
    		+ urlLineEdit->text() + QString("&format=kgml");

    cout<<url.toString().toStdString()<<endl;
//    QFileInfo fileInfo(url.path());

    fileName = urlLineEdit->text() ;
    if (fileName.isEmpty())
        fileName = "index.html";

    if (QFile::exists(fileName)) {
        if (QMessageBox::question(this, tr("HTTP"),
                                  tr("There already exists a file called %1 in "
                                     "the current directory. Overwrite?").arg(fileName),
                                  QMessageBox::Yes|QMessageBox::No, QMessageBox::No)
            == QMessageBox::No)
            return;
        QFile::remove(fileName);
    }

    file = new QFile(fileName);
    if (!file->open(QIODevice::WriteOnly)) {
        QMessageBox::information(this, tr("HTTP"),
                                 tr("Unable to save the file %1: %2.")
                                 .arg(fileName).arg(file->errorString()));
        delete file;
        file = 0;
        return;
    }

    progressDialog = new QProgressDialog(this);
    connect(progressDialog, SIGNAL(canceled()), this, SLOT(cancelDownload()));
    progressDialog->show();
    progressDialog->setWindowTitle(tr("HTTP"));
    progressDialog->setLabelText(tr("Downloading %1.").arg(fileName));

    downloadButton->setEnabled(false);

    // schedule the request
    httpRequestAborted = false;
    startRequest(url);
}

void HttpWindow::cancelDownload()
{
//    statusLabel->setText(tr("Download canceled."));
//    httpRequestAborted = true;
//    reply->abort();
//    downloadButton->setEnabled(true);
}

void HttpWindow::httpFinished()
{
    if (httpRequestAborted) {
        if (file) {
            file->close();
            file->remove();
            delete file;
            file = 0;
        }
        reply->deleteLater();
        progressDialog->hide();
        return;
    }

    progressDialog->hide();
    file->flush();
    file->close();

    QVariant redirectionTarget = reply->attribute(QNetworkRequest::RedirectionTargetAttribute);
    if (reply->error()) {
        file->remove();
        QMessageBox::information(this, tr("HTTP"),
                                 tr("Download failed: %1.")
                                 .arg(reply->errorString()));
        downloadButton->setEnabled(true);
    } else if (!redirectionTarget.isNull()) {
        QUrl newUrl = url.resolved(redirectionTarget.toUrl());
        if (QMessageBox::question(this, tr("HTTP"),
                                  tr("Redirect to %1 ?").arg(newUrl.toString()),
                                  QMessageBox::Yes | QMessageBox::No) == QMessageBox::Yes) {
            url = newUrl;
            reply->deleteLater();
            file->open(QIODevice::WriteOnly);
            file->resize(0);
            startRequest(url);
            return;
        }
    } else {
        QString fileName = QFileInfo(QUrl(urlLineEdit->text()).path()).fileName();
        statusLabel->setText(tr("Downloaded %1 to %2.").arg(fileName).arg(QDir::currentPath()));
        downloadButton->setEnabled(true);
    }

    reply->deleteLater();
    reply = 0;
    delete file;
    file = 0;

    //signal
    emit accepted();
}

void HttpWindow::httpReadyRead()
{
    // this slot gets called every time the QNetworkReply has new data.
    // We read all of its new data and write it into the file.
    // That way we use less RAM than when reading it at the finished()
    // signal of the QNetworkReply
    if (file)
        file->write(reply->readAll());
}

void HttpWindow::updateDataReadProgress(qint64 bytesRead, qint64 totalBytes)
{
    if (httpRequestAborted)
        return;
    progressDialog->setMaximum(totalBytes);
    progressDialog->setValue(bytesRead);
}

void HttpWindow::enableDownloadButton()
{
    downloadButton->setEnabled(!urlLineEdit->text().isEmpty());
}

string
HttpWindow::get_keggEntryName(const QString& url){
    QNetworkAccessManager 	manager;
    QNetworkReply 			*rly;
    rly = manager.get( QNetworkRequest(url) );

    QEventLoop	pause;
    connect(rly, SIGNAL(finished()),
            &pause, SLOT(quit()));
    pause.exec();

    string str( rly->readAll() );
    vector<string>	vs;
    stringstream	ss(str);

    string			line("");
    bool 			flag1 = false;
    bool 			flag2 = false;
    string			name("");

    while( std::getline(ss, line, '\n') ){
    	vs.push_back( line );
    	string 		temp1("<nobr>Name</nobr>");
    	size_t 		pos1 = line.find(temp1);

    	string 		temp2("<nobr>Gene name</nobr></th>");
    	size_t 		pos2 = line.find(temp2);

    	if( flag1 ){

    		string			mark1(">");
    		string			mark2("<");
    		size_t			ind1 = line.find(">", line.find(">", line.find(mark1)+1)+1)+1;

    		size_t			ind2 = line.find("<", line.find("<", line.find("<", line.find(mark2)+1)+1)+1);

    		if( ind1<std::string::npos && ind2<std::string::npos && ind1<ind2 ){
    			name = line.substr( ind1, ind2-ind1 );
//    			cout<<"name: "<<name<<endl;
        		break;
    		}
    	}
    	if( pos1>0 && pos1 < line.length() ){
    		flag1 = true;
    	}

    	// gene name
    	if( flag2 ){

    		string			mark1(">");
    		string			mark2("<");
    		size_t			ind1 = line.find(">", line.find(mark1)+1)+1;

    		size_t			ind2 = line.find("<", line.find("<", line.find(mark2)+1)+1) ;

    		if( ind1<std::string::npos && ind2<std::string::npos && ind1<ind2 ){
    			name = line.substr( ind1, ind2-ind1 );
//    			cout<<"name: "<<name<<endl;
        		break;
    		}
    	}
    	if( pos2>0 && pos2 < line.length() ){
    		flag2 = true;
    	}
    }
    return	name;
}

void
HttpWindow::keggEntryFinished(){
	cout<<"finished!@!!"<<endl;
}

void
HttpWindow::keggEntryReady(){
	cout<<"readY!!!"<<endl;
    QFile *f = new QFile("temp");
    if (f)
        f->write(reply->readAll());
}



