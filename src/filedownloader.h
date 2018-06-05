/*
 * filedownloader.h
 *
 *  Created on: Jun 21, 2016
 *      Author: stan
 */

#ifndef FILEDOWNLOADER_H_
#define FILEDOWNLOADER_H_

#include <QDialog>
#include <QNetworkAccessManager>
#include<QDomDocument>
#include<QXmlSimpleReader>
#include<QXmlDefaultHandler>
#include <QUrl>
#include<iostream>
#include<sstream>

class QDialogButtonBox;
class QFile;
class QLabel;
class QLineEdit;
class QProgressDialog;
class QPushButton;
class QSslError;
class QAuthenticator;
class QNetworkReply;

using namespace std;

class HttpWindow : public QDialog
{
    Q_OBJECT

public:
    HttpWindow(QWidget *parent = 0);

    void 			startRequest(QUrl url);
    QString			get_fileName(){ return fileName; }
    string			get_keggEntryName( const QString& url );
private slots:
    void 	downloadFile();
    void 	cancelDownload();
    void 	httpFinished();
    void 	httpReadyRead();
    void 	updateDataReadProgress(qint64 bytesRead, qint64 totalBytes);
    void 	enableDownloadButton();
    void 	keggEntryFinished();
    void	keggEntryReady();
    void	replyFinished(QNetworkReply* r){ };
   // void slotAuthenticationRequired(QNetworkReply*,QAuthenticator *);
#ifndef QT_NO_SSL
    //void sslErrors(QNetworkReply*,const QList<QSslError> &errors);
#endif

private:
    QString 			fileName;
    QLabel 				*statusLabel;
    QLabel 				*urlLabel;
    QLineEdit 			*urlLineEdit;
    QProgressDialog 	*progressDialog;
    QPushButton 		*downloadButton;
    QPushButton 		*quitButton;
    QDialogButtonBox 	*buttonBox;

    QUrl 				url;
    QNetworkAccessManager qnam;
    QNetworkReply 		*reply;
    QFile 				*file;
    int 				httpGetId;
    bool 				httpRequestAborted;
    HttpWindow 			*ui;

};

#endif /* FILEDOWNLOADER_H_ */
