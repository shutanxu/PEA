/*
 * pathviewer2.h
 *
 *  Created on: Mar 3, 2017
 *      Author: stan
 */

#ifndef SRC_PATHVIEWER2_H_
#define SRC_PATHVIEWER2_H_

#include<QGLWidget>
#include<QOpenGLWidget>
#include<QOpenGLFunctions>
#include<QOpenGLShaderProgram>
#include<QRubberBand>
#include<GL/gl.h>
#include<QCursor>
#include<string>
#include<fstream>
#include<iostream>

#include"trackball.h"
#include"trackMover.h"
#include"pathway.h"
#include"KGML.h"

using namespace std;

class PathwayViewer2: public QWidget{
	Q_OBJECT
public:
	PathwayViewer2(QWidget *parent = 0);
	void 		set_pathway(Pathway& p, bool d_3 = false);

    QSize 		minimumSizeHint()const Q_DECL_OVERRIDE;
    QSize 		sizeHint()const Q_DECL_OVERRIDE;
    void		updateAxis(bool b){}
    void		updateLabel(bool b){showLabel = b;}
    void		updateEdgeLabel(bool b){showEdgeLabel = b;}
    void		updateDragConnected(bool b){ dragConnected=b; }
    void		updateDragFixedEdge(bool b){ dragFixedEdge=b; }
    void		clear(){}
    void		highlightNode(string s){highLightName=s;}
    void		clearHighlightNode(){highLightName="";}
    void		saveImage(QString fileName);
    Pathway		get_pathway(){ return pathway; }
public slots:
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
	Pathway					pathway;
    QPen 					pen;
    QBrush 					brush;
    bool 					antialiased;
    double					scale;
    QPointF					mousePos;
    QPointF					mouseMove;
    QPointF					mouseSave;
    QVector2D				translateCenter;
    float					range;
    float					minNodeVal;
    float					maxNodeVal;
    bool	                showLabel;
    bool	                showEdgeLabel;
    bool					dragConnected;
    bool					dragFixedEdge;
    void					drawEdge();
    void					drawNode();
    void					drawLabel();
    void					drawEdgeLabel();
    int						selectedNode;
    QTransform				painterWorldTransf;
    string					highLightName;
};


#endif /* SRC_PATHVIEWER2_H_ */

