/*
 * heatmap.h
 *
 *  Created on: Oct 28, 2016
 *      Author: stan
 */

#ifndef SRC_HEATMAP_H_
#define SRC_HEATMAP_H_

#include<iostream>
#include<math.h>
#include<time.h>
#include<vector>
#include<string>
#include<QBrush>
#include<QPen>
#include<QPixmap>
#include<QWidget>
#include<QStaticText>
#include<QPainter>
#include<QVector>
#include<QFont>
#include<QDialog>
#include<QLineEdit>
#include<QLabel>
#include<QPushButton>
#include<QHBoxLayout>
#include<QVBoxLayout>
#include<QTextEdit>
#include<QCheckBox>
#include<QFileDialog>
#include<QWheelEvent>

#include"annotation.h"
#include"tokenSeparator.h"

using namespace std;

class HeatmapViewer : public QWidget
{
    Q_OBJECT

public:
    enum Shape { Line, Points, Polyline, Polygon, Rect,
    			RoundedRect, Ellipse, Arc,
                 Chord, Pie, Path, Text, Pixmap };

    HeatmapViewer(QWidget *parent = 0);

    bool setData(  vector<vector<float> > dm,
    		vector<string> rn,
    		vector<string> cn,
			vector<string> cc,
			bool normalize=false);

    QSize minimumSizeHint()const Q_DECL_OVERRIDE;
    QSize sizeHint()const Q_DECL_OVERRIDE;

public slots:
    void setShape(Shape shape);
    void setPen(const QPen &pen);
    void setBrush(const QBrush &brush);
    void setAntialiased(bool antialiased);

protected:
    void 	paintEvent(QPaintEvent *event);
    void	wheelEvent(QWheelEvent *event);
    void	mouseMoveEvent(QMouseEvent *event);
    void	mouseClickEvent(QMouseEvent *event);
    void	mousePressEvent(QMouseEvent *event);
private:
    Shape 					shape;
    QPen 					pen;
    QBrush 					brush;
    bool 					antialiased;

    QVector<QVector<float> >	dataMatrix;
    QVector<string>				rowNameVec;
    QVector<string>				colNameVec;
    QVector<string>				columnClass;

    float					scale;
    QPointF					mousePos;
    QPointF					mouseMove;
    QPointF					mouseSave;

	float		max;
	float		min;
};

class HeatmapDialog:public QDialog{
	Q_OBJECT
public:
	HeatmapDialog();
	vector<string>	getGeneVec(){ return geneVec; }
	Annotation* 	getAnnotation(){ return annotation; }
public slots:
	void run();
	void loadFile();

signals:
	void loadData( QString file );
	void finish();
private:
	QLineEdit*	fileEdit;
	QString		fileName;
	QTextEdit*	geneEdit;
	Annotation*	annotation;
	vector<string>	geneVec;

};

#endif /* SRC_HEATMAP_H_ */

