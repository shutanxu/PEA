#ifndef PATHVIEWER_H
#define PATHVIEWER_H

#include<QGLWidget>
#include<QOpenGLWidget>
#include<QOpenGLFunctions>
#include<QOpenGLShaderProgram>
#include<GL/gl.h>
#include<string>
#include<fstream>
#include<iostream>

//#include <ft2build.h>
//#include FT_FREETYPE_H

#include"trackball.h"
#include"trackMover.h"
#include"pathway.h"
#include"KGML.h"

using namespace std;

class PathwayViewer:public QOpenGLWidget, protected QOpenGLFunctions{
    Q_OBJECT
public:
    PathwayViewer(QWidget *parent=0);
    ~PathwayViewer();
    void    set_pathway( Pathway& p, bool d_3=false );
    void    updateAxis(const bool b);
    void    updateLabel(const bool b);
    void 	highlightNode( const string name );
    void 	clearHighlightNode(){ namesHighLightVec.clear(); }
    void	set3DView(bool b){ show3D=b; }
    void	clear();
protected:
    void    initializeGL();
    void    resizeGL( int width, int height );
    void    paintGL();
    void    mousePressEvent( QMouseEvent *event );
    void    mouseMoveEvent( QMouseEvent *event );
    void    mouseReleaseEvent(QMouseEvent *e);
    void    mouseDoubleClickEvent( QMouseEvent *event );
    void    wheelEvent( QWheelEvent *e );
private slots:

private:
    void    initializeLine();
    void    initializeAxis();
    void    initializeSphere();
    void	initializeStick();

    void    updateSphere();

    void    drawTest( const QSize &windowSize );
    void    drawPathwayNode();
    void    drawPathwayNodeGene();
    void    drawPathwayNodeCompound();

    void	drawPathwayNodeSphere();
    void	drawPathwayNodeHighlight();
    void    drawPathwayEdge();
    void	drawPathwayEdgeStick();
    void    drawNodeLabel();
    void    drawLine();
    void    drawAxis();

    QPointF pixelPosToViewPos(const QPointF& p) ;

    int     init_render_resources();
    void    render_text(const char *text, float x,
                        float y, float z, float sx, float sy);
    void    drawTestTexture( const QSize &windowSize );
private:
    bool                showAxis;
    bool                showLabel;
    bool				show3D;

    QMatrix4x4          orthographic;
    QMatrix4x4          window_normalised_matrix;
    QMatrix4x4          window_painter_matrix;
    QMatrix4x4          projection;

    QVector2D           mousePressPosupdateition;
    QVector3D           translateCenter;
    QQuaternion         rotation;
    QVector2D           translation;
    trackMover*         m_trackMover;
    int                 m_distExp;
    TrackBall*          m_trackBalls;
    float				standardRadius;

    QOpenGLShaderProgram    axisProgram;
    QOpenGLShaderProgram    sphereProgram;
    QOpenGLShaderProgram    stickProgram;
private:
    Pathway             	pathway;
    QOpenGLShaderProgram  	renderTextProgram;
//    FT_Library          		ft;
//    FT_Face             		face;
    const char*         		fontfilename;
    vector<vector< size_t > >   textureIdVec;
    vector<QOpenGLTexture*>  	textureVec;
    vector<string> 				namesHighLightVec;
    float   					range;
};

#endif // PATHVIEWER_H
