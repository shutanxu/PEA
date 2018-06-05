/*
 * annotationDialog.h
 *
 *  Created on: Sep 19, 2016
 *      Author: stan
 */

#ifndef SRC_ANNOTATIONDIALOG_H_
#define SRC_ANNOTATIONDIALOG_H_

#include <QWidget>
#include <QtWidgets>
#include<string>
#include<vector>
#include"annotation.h"

using namespace std;

class AnnotationDialog:public QDialog{
	Q_OBJECT

public:
	AnnotationDialog();
	Annotation*	getAnnotation(){ return annotation; }

public slots:
	void	run();
	void	loadFile();

signals:
	void	loadData(QString file);
	void	finishRun();
private:
	void			creatIcons();
	QLineEdit*		rnaSeqDataEdit;
	QString			rnaFileName;
	QComboBox*		pathwayComb;
	string			fileName;
	Annotation*		annotation;
	QTableWidget*	dataTable;
};

#endif /* SRC_ANNOTATIONDIALOG_H_ */
