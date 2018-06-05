#include <QMouseEvent>
#include <QTimer>
#include <QVector2D>
#include <QVector3D>
#include <QOpenGLShaderProgram>
#include <QOpenGLContext>
#include <QOpenGLFunctions>
#include <QObject>
#include <QFile>
#include <QDateTime>
#include <QFileSystemWatcher>
#include <QOpenGLVertexArrayObject>
#include <QOpenGLBuffer>
#include <QOpenGLShaderProgram>
#include <QOpenGLFunctions>
#include <math.h>
#include <qmath.h>
#include <QFileInfo>
#include <QTime>
#include <QStaticText>

#include"pathviewer.h"
#include"polyeder.h"
#include"stick.h"

using namespace std;

PathwayViewer::PathwayViewer(QWidget *parent):QOpenGLWidget(parent){
    m_trackBalls = new TrackBall(0.05f, QVector3D(0, 1, 0), TrackBall::Sphere);
    translation = QVector2D(0,0);
    m_distExp= 4000;
    m_trackMover= new trackMover();
    showAxis=false;
    showLabel=false;
    translateCenter=QVector3D(0,0,0);
    QPointF p;
    m_trackBalls->push( pixelPosToViewPos(p), QQuaternion());
    rotation = m_trackBalls->rotation();
    namesHighLightVec.clear();
    update();
}

PathwayViewer::~PathwayViewer(){
    delete m_trackBalls;
}

void
PathwayViewer::initializeGL(){
    initializeOpenGLFunctions();
    glClearColor(1, 1, 1, 1);

    glEnable(GL_DEPTH_TEST);
    glEnable( GL_POINT_SMOOTH );
    glEnable(GL_LINE_SMOOTH);
    glBlendFunc( GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA );
    glFrontFace(GL_CW);
    glCullFace(GL_BACK);
    glEnable(GL_CULL_FACE);
    initializeAxis();
    initializeSphere();
    initializeStick();
    init_render_resources();
}

void
PathwayViewer::set_pathway( Pathway& p, bool d_3 ){
	pathway.clear();
	pathway=p;
	show3D=d_3;
    vector<Edge>    edgeVec=pathway.get_edgeVec();
    range=1;
    float	dist = 0;
    standardRadius = 0.001;

    if(d_3){
    	for( size_t i=0; i<edgeVec.size(); i++ ){
    		float x1=(edgeVec[i].get_parent()->get_x()-translateCenter.x())/range;
    		float y1=(edgeVec[i].get_parent()->get_y()-translateCenter.y())/range;
    		float z1=(edgeVec[i].get_parent()->get_z()-translateCenter.z())/range;

    		float x2=(edgeVec[i].get_child()->get_x()-translateCenter.x())/range;
    		float y2=(edgeVec[i].get_child()->get_y()-translateCenter.y())/range;
    		float z2=(edgeVec[i].get_child()->get_z()-translateCenter.z())/range;

    		dist += sqrt( (x1-x2)*(x1-x2) + (y1-y2)*(y1-y2) + (z1-z2)*(z1-z2) );
    	}

    	if( !edgeVec.empty() ){
    		dist /= (float)edgeVec.size();
    	}
    	standardRadius = dist/10.0;
    	m_distExp= 4000;
    }else{
    	//translate center
    	range=pathway.get_range()*10;
    	float x,y,z;
    	x=y=z=0;
    	for( size_t i=0; i<edgeVec.size(); i++ ){
    		x += edgeVec[i].get_parent()->get_x();
    		y += edgeVec[i].get_parent()->get_y();
    	}
    	x /= (float)edgeVec.size();
    	y /= (float)edgeVec.size();
    	translateCenter = QVector3D(x,y,0);
    	//---------------------------------------------------------------------------
    	for( size_t i=0; i<edgeVec.size(); i++ ){
    		float x1=(edgeVec[i].get_parent()->get_x()-translateCenter.x())/range;
    		float y1=(edgeVec[i].get_parent()->get_y()-translateCenter.y())/range;

    		float x2=(edgeVec[i].get_child()->get_x()-translateCenter.x())/range;
    		float y2=(edgeVec[i].get_child()->get_y()-translateCenter.y())/range;

    		dist += sqrt( (x1-x2)*(x1-x2) + (y1-y2)*(y1-y2) );
    	}

    	if( !edgeVec.empty() ){
    		dist /= (float)edgeVec.size();
    	}
    	standardRadius = dist/10.0;
    	m_distExp= 4000;
    }
	update();
}

void
PathwayViewer::clear(){
    m_trackBalls = new TrackBall(0.05f, QVector3D(0, 1, 0), TrackBall::Sphere);
    translation = QVector2D(0,0);
    m_distExp= 4000;
    m_trackMover= new trackMover();
    showAxis=false;
    showLabel=false;
    translateCenter=QVector3D(0,0,0);
    QPointF p;
    m_trackBalls->push( pixelPosToViewPos(p), QQuaternion());
    rotation = m_trackBalls->rotation();
    namesHighLightVec.clear();

	pathway.clear();
	update();
}

void
PathwayViewer::paintGL(){
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    drawPathwayNodeSphere();

    if( show3D ){
    	drawPathwayEdgeStick();
    }else{
    	drawPathwayEdge();
    }

    if( showAxis ){
        drawAxis();
    }

    if( showLabel ){
        drawNodeLabel();
    }

    if(!namesHighLightVec.empty()){
    	drawPathwayNodeHighlight();
    }

}

void
PathwayViewer::resizeGL(int w, int h){
    glViewport(0, 0, w, h);
    orthographic.setToIdentity();
    double hScale = static_cast<double>(h) / static_cast<double>(500);
    double wScale = static_cast<double>(w) / static_cast<double>(500);
    orthographic.ortho(-wScale, wScale, -hScale, hScale, -100, 100);

    window_normalised_matrix.setToIdentity();
    window_normalised_matrix.translate(w/2.0, h/2.0);

    window_painter_matrix.setToIdentity();
    window_painter_matrix.translate(w/2.0, h/2.0);

    projection.setToIdentity();
    projection.perspective(45.f, qreal(w)/qreal(h), 0.1f, 100.f);
}

QPointF PathwayViewer::pixelPosToViewPos(const QPointF& p) {
    return QPointF(2.0 * float(p.x()) / width() - 1.0,
                   1.0 - 2.0 * float(p.y()) / height());
}

void PathwayViewer::mousePressEvent(QMouseEvent *event)
{
    if (event->buttons() & Qt::LeftButton) {
        m_trackBalls->push(pixelPosToViewPos(event->localPos()), QQuaternion() );
        rotation = m_trackBalls->rotation();
    }else if (event->buttons() & Qt::RightButton) {

        m_trackMover->push(pixelPosToViewPos(event->localPos()) );

    }
    update();
}

void
PathwayViewer::highlightNode( const string name ){
	if( std::find(namesHighLightVec.begin(), namesHighLightVec.end(),name)
			==namesHighLightVec.end() && !name.empty() ){
		namesHighLightVec.push_back(name);
	}
}

void PathwayViewer::mouseMoveEvent(QMouseEvent *event)
{
    if (  event->buttons() & Qt::LeftButton ) {
        m_trackBalls->move(pixelPosToViewPos(event->localPos() ), QQuaternion() );
        rotation = m_trackBalls->rotation();
        update();
    }else if (event->buttons() & Qt::RightButton) {
        m_trackMover->move(pixelPosToViewPos(event->localPos()) );
        translation = m_trackMover->translation();
        update();
    }else {
        m_trackBalls->release(pixelPosToViewPos(event->localPos()), QQuaternion() );
    }
}

void PathwayViewer::mouseReleaseEvent( QMouseEvent *event ){

}

void PathwayViewer::mouseDoubleClickEvent(QMouseEvent *event){
    float clickPosX=(event->x()/float(width())-0.5)*2;
    float clickPosY=-(event->y()/float(height())-0.5)*2;

    if(1){
        vector<Node>    nodeVec=pathway.get_nodeVec();

        vector<QVector3D>   vertexVec;
        float   range=1.0;
        size_t  nodeSize = nodeVec.size();

        QMatrix4x4 matrix;
        matrix.translate(translation.x(), translation.y(), 0.0);
        if(show3D){matrix.rotate(rotation);}
        matrix.scale( exp(m_distExp / 1200.0f));

        for( size_t i=0; i<nodeSize; i++ ){
            float   x=(nodeVec[i].get_x()-translateCenter.x())/range;
            float   y=(nodeVec[i].get_y()-translateCenter.y())/range;
            float   z=(nodeVec[i].get_z()-translateCenter.z())/range;
            QVector3D v(x,y,z);
            vertexVec.push_back(v);
            QVector3D proj=orthographic*matrix*v;
            float clickToNodeDist=(clickPosX-proj.x())*(clickPosX-proj.x())+
                    (clickPosY-proj.y())*(clickPosY-proj.y());
            if(clickToNodeDist<0.001){
                translateCenter=translateCenter+QVector3D(x,y,z);
            }
        }
        QPointF currPos = QPointF(0,0);
        trackMover *tm=new trackMover();
        m_trackMover=tm;
        translation = QVector2D(0,0);
    }
    update();
}

void PathwayViewer::wheelEvent(QWheelEvent *event){
    m_distExp -= event->delta();
    update();

    //the mid button is being pressed
    event->accept();
}

int
PathwayViewer::init_render_resources(){

    /* Create a texture that will be used to hold one "glyph" */
    GLuint tex;
    glActiveTexture(GL_TEXTURE0);
    glGenTextures(1, &tex);
    glBindTexture(GL_TEXTURE_2D, tex);

    /* We require 1 byte alignment when uploading texture data */
    glPixelStorei(GL_UNPACK_ALIGNMENT, 1);

    /* Clamping to edges is important to prevent artifacts when scaling */
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

    /* Linear filtering usually looks best for text */
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    /* Initialize the FreeType2 library */
//    if (FT_Init_FreeType(&ft) != 0) { //0 means success
//        fprintf(stderr, "Could not init freetype library\n");
//        return 0;
//    }

    /* Load a font */
//    fontfilename = "FreeSans.ttf";
    fontfilename = "arial.ttf";

//    if ( FT_New_Face(ft, fontfilename, 0, &face)) {
//    	// FT_New_Face return 0 means success
//        fprintf(stderr, "Could not open font %s\n", fontfilename);
//        return 0;
//    }

//    FT_Set_Pixel_Sizes(face, 0, 32);

    string vshaderFile = string("./shader/textRender_vs.glsl");
    string fshaderFile = string("./shader/textRender_fs.glsl");

    if( !renderTextProgram.addShaderFromSourceFile(QOpenGLShader::Vertex,
            QString::fromStdString(vshaderFile)) ){
        cerr<<"can not load vertex shader!!!"<<endl;
    }
    if( !renderTextProgram.addShaderFromSourceFile(QOpenGLShader::Fragment,
            QString::fromStdString(fshaderFile)) ){
        cerr<<"can not load fragment shader!!!"<<fshaderFile<<endl;
    }
    if( !renderTextProgram.link() ){
        cerr<<"can not link shader!!!"<<endl;
    }
    if( !renderTextProgram.bind() ){
        cerr<<"can not bind shader!!!"<<endl;
    }
    return 1;
}

void
PathwayViewer::initializeAxis(){
    string vshaderFile = string("./shader/axis_vs.glsl");
    string fshaderFile = string("./shader/axis_fs.glsl");

    if( !axisProgram.addShaderFromSourceFile(QOpenGLShader::Vertex,
            QString::fromStdString(vshaderFile)) ){
        cerr<<"can not load vertex shader!!!"<<endl;
    }
    if( !axisProgram.addShaderFromSourceFile(QOpenGLShader::Fragment,
            QString::fromStdString(fshaderFile)) ){
        cerr<<"can not load fragment shader!!!"<<endl;
    }
    if( !axisProgram.link() ){
        cerr<<"can not link shader#!!!"<<endl;
    }
    if( !axisProgram.bind() ){
        cerr<<"can not bind shader##!!!"<<endl;
    }
}

void
PathwayViewer::initializeSphere(){
    string vshaderFile = string("./shader/sphere_vs.glsl");
    string fshaderFile = string("./shader/sphere_fs.glsl");

    if( !sphereProgram.addShaderFromSourceFile(QOpenGLShader::Vertex,
            QString::fromStdString(vshaderFile)) ){
        cerr<<"can not load vertex shader!!!"<<endl;
    }
    if( !sphereProgram.addShaderFromSourceFile(QOpenGLShader::Fragment,
            QString::fromStdString(fshaderFile)) ){
        cerr<<"can not load fragment shader!!!"<<endl;
    }
    if( !sphereProgram.link() ){
        cerr<<"can not link shader!!!"<<endl;
    }
    if( !sphereProgram.bind() ){
        cerr<<"can not bind shader!!!"<<endl;
    }
}

void
PathwayViewer::initializeStick(){
    string vshaderFile = string("./shader/stick_vs.glsl");
    string fshaderFile = string("./shader/stick_fs.glsl");

    if( !stickProgram.addShaderFromSourceFile(QOpenGLShader::Vertex,
            QString::fromStdString(vshaderFile)) ){
        cerr<<"can not load vertex shader!!!"<<endl;
    }
    if( !stickProgram.addShaderFromSourceFile(QOpenGLShader::Fragment,
            QString::fromStdString(fshaderFile)) ){
        cerr<<"can not load fragment shader!!!"<<endl;
    }
    if( !stickProgram.link() ){
        cerr<<"can not link shader!!!"<<endl;
    }
    if( !stickProgram.bind() ){
        cerr<<"can not bind shader!!!"<<endl;
    }
}

void
PathwayViewer::updateSphere(){

}

void
PathwayViewer::updateAxis(const bool b){
    if(b){
        showAxis=true;
    }else{
        showAxis=false;
    }
}

void
PathwayViewer::updateLabel(const bool b){
    if(b){
        showLabel=true;
        vector<Node>    nodeVec=pathway.get_nodeVec();
        textureIdVec.clear();
        for( size_t i=0; i<nodeVec.size(); i++ ){
        	const char* nodeLabel = nodeVec[i].get_name().c_str();

        	vector<size_t>     tempVec;
        	string name=nodeVec[i].get_name();
        	for( size_t j=0; j<name.size(); j++ ){
        		char p=name.at(j);
//        		if ( FT_Load_Char(face, p, FT_LOAD_RENDER )!= 0 || p==' '){     // FT_Load_Char return 0 means success
//        			//    			cout<<"! can not find "<<p<<" in label:"<<nodeLabel[0]<<endl;
//        		}else{
//        			FT_GlyphSlot    g=face->glyph;
//        			QImage  glyphImage(g->bitmap.buffer,
//        					g->bitmap.width,
//    						g->bitmap.rows,
//    						g->bitmap.pitch,
//    						QImage::Format_Indexed8);
//        			QOpenGLTexture*     texture =new QOpenGLTexture( glyphImage );
//        			tempVec.push_back(texture->textureId());
//        		}
        	}
        	textureIdVec.push_back(tempVec);
        }
    }else{
        showLabel=false;
    }
}

void
PathwayViewer::drawPathwayEdge(){
//	glDisable(GL_DEPTH_TEST);
	glEnable(GL_MULTISAMPLE);
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

	string vshaderFile = string("./shader/axis_vs.glsl");
	string fshaderFile = string("./shader/axis_fs.glsl");

	QOpenGLShaderProgram    pathwayProgram;
	if( !pathwayProgram.addShaderFromSourceFile(QOpenGLShader::Vertex,
			QString::fromStdString(vshaderFile)) ){
		cerr<<"can not load vertex shader!!!"<<endl;
	}
	if( !pathwayProgram.addShaderFromSourceFile(QOpenGLShader::Fragment,
			QString::fromStdString(fshaderFile)) ){
		cerr<<"can not load fragment shader!!!"<<endl;
	}
	if( !pathwayProgram.link() ){
		cerr<<"can not link shader!!!"<<endl;
	}
	if( !pathwayProgram.bind() ){
		cerr<<"can not bind shader!!!"<<endl;
	}

	vector<Edge>    edgeVec=pathway.get_edgeVec();

	//    pathway.print();

	vector<QVector3D>   vertexVec;
	vector<QVector3D>   vertexColor;

	QMatrix4x4 matrix;
	matrix.translate(translation.x(), translation.y(), 0.0);
	if(show3D){if(show3D){matrix.rotate(rotation);}}
	matrix.scale( exp(m_distExp / 1200.0f));
	QMatrix4x4 matrixR;
	matrixR.rotate(rotation);
	matrixR.scale( exp(m_distExp / 1200.0f));

	for( size_t i=0; i<edgeVec.size(); i++ ){
		double x1=(edgeVec[i].get_parent()->get_x()-translateCenter.x())/range;
		double y1=(edgeVec[i].get_parent()->get_y()-translateCenter.y())/range;
		double z1=(edgeVec[i].get_parent()->get_z()-translateCenter.z())/range;
		double x2=(edgeVec[i].get_child()->get_x()-translateCenter.x())/range;
		double y2=(edgeVec[i].get_child()->get_y()-translateCenter.y())/range;
		double z2=(edgeVec[i].get_child()->get_z()-translateCenter.z())/range;

		QVector3D v1(x1,y1,z1);
		QVector3D v2(x2,y2,z2);
		vertexVec.push_back(v1);
		vertexVec.push_back(v2);

		QVector3D c(0,0,0);
		vertexColor.push_back(c);
		vertexColor.push_back(c);
	}

	QOpenGLBuffer vertexBuf;//( QOpenGLBuffer::VertexBuffer );
	vertexBuf.create();
	vertexBuf.setUsagePattern(QOpenGLBuffer::StaticDraw);
	vertexBuf.bind();
	vertexBuf.allocate( &vertexVec[0], edgeVec.size()*2*sizeof(QVector3D) );

	QOpenGLBuffer colorBuf( QOpenGLBuffer::VertexBuffer );
	colorBuf.create();
	colorBuf.setUsagePattern( QOpenGLBuffer::StaticDraw );
	colorBuf.bind();
	colorBuf.allocate(&vertexColor[0], edgeVec.size()*2*sizeof(QVector3D));

	glLineWidth(1);
	axisProgram.bind();
	vertexBuf.bind();
	axisProgram.setUniformValue( "mvp_matrix",  orthographic * matrix );
	axisProgram.enableAttributeArray("position");
	axisProgram.setAttributeBuffer( "position", GL_FLOAT, 0, 3,  sizeof(QVector3D) );
	colorBuf.bind();
	axisProgram.enableAttributeArray("vcolor");
	axisProgram.setAttributeBuffer("vcolor", GL_FLOAT, 0, 3, sizeof(QVector3D) );
	glDrawArrays(GL_LINES, 0, vertexVec.size() );

	axisProgram.release();
	vertexBuf.destroy();
	colorBuf.destroy();
}

void
PathwayViewer::drawPathwayEdgeStick(){
	glEnable(GL_MULTISAMPLE);
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);


    vector<QVector3D>    oneVertexVec,oneNormalVec;
    vector<QColor>       oneColorVec;
    vector<QColor>       stick_colorVec;
    vector<QVector3D>    stick_vertexVec, stick_normalVec;
    QOpenGLBuffer  stickVertexBuf, stickNormalBuf, stickColorBuf ;
    stick_colorVec.clear();
    stick_vertexVec.clear();
    stick_normalVec.clear();

    vector<Node>    nodeVec=pathway.get_nodeVec();
    vector<Edge>    edgeVec=pathway.get_edgeVec();

    for( size_t i=0; i<edgeVec.size(); i++ ){
        oneVertexVec.clear();
        oneNormalVec.clear();
        oneColorVec.clear();

        float x1=(edgeVec[i].get_parent()->get_x()-translateCenter.x())/range;
        float y1=(edgeVec[i].get_parent()->get_y()-translateCenter.y())/range;
        float z1=(edgeVec[i].get_parent()->get_z()-translateCenter.z())/range;
        float x2=(edgeVec[i].get_child()->get_x()-translateCenter.x())/range;
        float y2=(edgeVec[i].get_child()->get_y()-translateCenter.y())/range;
        float z2=(edgeVec[i].get_child()->get_z()-translateCenter.z())/range;

        QVector3D p1(x1,y1,z1);
        QVector3D p2(x2,y2,z2);
        QColor	color1(0,1,0);
        QColor	color2(0,1,0);
        float 	radius=standardRadius/6.0;
        if( p1 != p2 ){
        	getStick(p1,p2,color1,color2,0.5, radius, oneVertexVec,oneNormalVec, oneColorVec);
        	stick_vertexVec.insert(stick_vertexVec.end(),oneVertexVec.begin(),oneVertexVec.end());
        	stick_normalVec.insert(stick_normalVec.end(),oneNormalVec.begin(),oneNormalVec.end());
        	stick_colorVec.insert(stick_colorVec.end(),oneColorVec.begin(),oneColorVec.end());
        }
    }

    vector<float> colorVec;
    for( size_t i=0; i<stick_colorVec.size(); i++ ){
        colorVec.push_back(stick_colorVec[i].red());
        colorVec.push_back(stick_colorVec[i].green());
        colorVec.push_back(stick_colorVec[i].blue());
    }

    stickVertexBuf.create();
    stickVertexBuf.setUsagePattern(QOpenGLBuffer::StaticDraw);
    stickVertexBuf.bind();
    stickVertexBuf.allocate( &stick_vertexVec[0], stick_vertexVec.size()*sizeof(QVector3D) );

    stickNormalBuf.create();
    stickNormalBuf.setUsagePattern(QOpenGLBuffer::StaticDraw);
    stickNormalBuf.bind();
    stickNormalBuf.allocate( &stick_normalVec[0], stick_normalVec.size()*sizeof(QVector3D) );

    stickColorBuf.create();
    stickColorBuf.setUsagePattern(QOpenGLBuffer::StaticDraw);
    stickColorBuf.bind();
    stickColorBuf.allocate( &colorVec[0], colorVec.size()*sizeof(float) );

    //--------------------------------------------------
    glDisable(GL_BLEND);
    QMatrix4x4 matrix;
    matrix.translate(translation.x(), translation.y(), 0.0);
    if(show3D){if(show3D){matrix.rotate(rotation);}}
    matrix.scale( exp(m_distExp / 1200.0f));

    QMatrix4x4 matrixR;
    matrixR.rotate(rotation);
    matrixR.scale( exp(m_distExp / 1200.0f));

    stickProgram.bind();
    stickProgram.setUniformValue( "mvp_matrix",  orthographic * matrix );
    stickProgram.setUniformValue( "mv_matrix",  matrixR );

    stickVertexBuf.bind();
    stickProgram.enableAttributeArray("position");
    stickProgram.setAttributeBuffer("position", GL_FLOAT, 0, 3, sizeof(QVector3D) );

    stickNormalBuf.bind();
    stickProgram.enableAttributeArray("normal");
    stickProgram.setAttributeBuffer( "normal", GL_FLOAT, 0, 3, 3*sizeof(float) );

    stickColorBuf.bind();
    stickProgram.enableAttributeArray("color");
    stickProgram.setAttributeBuffer( "color", GL_FLOAT, 0, 3, 3*sizeof(float) );

    glDrawArrays( GL_TRIANGLES, 0, stick_vertexVec.size() );
    stickProgram.release();
}

void
PathwayViewer::drawPathwayNodeSphere(){
    glDisable(GL_BLEND);
    glEnable(GL_DEPTH_TEST);
    Polyeder ply(3);
    vector<TriangleSurf> surfs = ply.get_surfTriangles();

    vector<float> allVertex = ply.get_allVertex();
    vector<int> triangleVertexIndex = ply.get_triangleVertexIndex();
    QOpenGLBuffer sphereVertexBuf,sphereIndexBuf;
    sphereVertexBuf.create();
    sphereVertexBuf.setUsagePattern(QOpenGLBuffer::StaticDraw);
    sphereVertexBuf.bind();
    sphereVertexBuf.allocate( &allVertex[0], allVertex.size()*sizeof(float) );

    QOpenGLBuffer indbuf( QOpenGLBuffer::IndexBuffer);
    sphereIndexBuf = indbuf;
    sphereIndexBuf.create();
    sphereIndexBuf.setUsagePattern(QOpenGLBuffer::StaticDraw );
    sphereIndexBuf.bind();
    sphereIndexBuf.allocate( &triangleVertexIndex[0], triangleVertexIndex.size()*sizeof(int) );

    //-----------------------

    glDisable(GL_DIFFUSE);
    glDisable( GL_BLEND );
    glDisable(GL_CULL_FACE);

    QMatrix4x4 matrix, matrix2, matrix3;
    matrix.translate(translation.x(), translation.y(), 0.0);
    if(show3D){matrix.rotate(rotation);}
    if(show3D){matrix2.rotate(rotation);}
    matrix.scale( exp(m_distExp / 1200.0f));
    matrix2.scale( exp(m_distExp / 1200.0f));
    sphereProgram.bind();
    sphereProgram.setUniformValue( "mvp_matrix",  orthographic * matrix );

    sphereProgram.setUniformValue( "mv_matrix",  matrix2 );

    sphereVertexBuf.bind();
    sphereProgram.enableAttributeArray( "position" );
    sphereProgram.setAttributeBuffer( "position", GL_FLOAT, 0, 3, 3*sizeof( float ) );
    sphereIndexBuf.bind();

    //------------------------------
    vector<Node>    nodeVec=pathway.get_nodeVec();
    float maxV = -10e10;
    for( size_t i=0; i<nodeVec.size(); i++ ){
    	float	v = abs(nodeVec[i].get_value());
    	maxV = maxV > v ? maxV : v;
    }

    vector<QVector3D>   vertexVec;
    vector<QVector3D>   vertexColor;

    size_t  nodeSize = nodeVec.size();
    for( size_t i=0; i<nodeVec.size(); i++ ){
    	float   x=(nodeVec[i].get_x()-translateCenter.x())/range;
    	float   y=(nodeVec[i].get_y()-translateCenter.y())/range;
    	float   z=(nodeVec[i].get_z()-translateCenter.z())/range;
    	QVector3D 	center(x,y,z);

    	int type=nodeVec[i].get_type();
    	QVector3D color(1,1,1);

    	if( nodeVec[i].get_value()>0 && type == 0 ){
    		color = QVector3D(1,0,0);
    	}else if( nodeVec[i].get_value()<0 && type == 0 ){
    		color = QVector3D(0,0,1);
    	}else{
    		color = QVector3D(1,1,0);
    	}

//    	float		r=0.001;
//    	float		r=standardRadius;
    	float		ab = abs(nodeVec[i].get_value());
    	float		r = standardRadius/2.0;
    	if( ab>1 ){
    		r=standardRadius*abs(1.3*((ab-1)/(maxV-1)+1))/2.0;
    	}
    	sphereProgram.setUniformValue( "center", center );
    	sphereProgram.setUniformValue( "color", color );
    	sphereProgram.setUniformValue( "radius", r);
    	glDrawElements( GL_TRIANGLES, surfs.size() * 3, GL_UNSIGNED_INT, 0 );
    }
    sphereProgram.release();
}

void
PathwayViewer::drawPathwayNodeHighlight(){

	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	glEnable(GL_BLEND);
	glEnable(GL_LINE_SMOOTH);
	glHint(GL_LINE_SMOOTH_HINT,GL_NICEST);

    string vshaderFile = string("./shader/node_highlight_vs.glsl");
    string fshaderFile = string("./shader/node_highlight_fs.glsl");

    QOpenGLShaderProgram    pathwayProgram;
    if( !pathwayProgram.addShaderFromSourceFile(QOpenGLShader::Vertex,
            QString::fromStdString(vshaderFile)) ){
        cerr<<"can not load vertex shader!!!"<<endl;
    }
    if( !pathwayProgram.addShaderFromSourceFile(QOpenGLShader::Fragment,
            QString::fromStdString(fshaderFile)) ){
        cerr<<"can not load fragment shader!!!"<<endl;
    }
    if( !pathwayProgram.link() ){
        cerr<<"can not link shader!!!"<<endl;
    }
    if( !pathwayProgram.bind() ){
        cerr<<"can not bind shader!!!"<<endl;
    }

    vector<Node>    nodeVec=pathway.get_nodeVec();

    vector<QVector3D>   vertexVec;
    vector<QVector3D>   vertexColor;

    size_t  nodeSize = nodeVec.size();


    QMatrix4x4 matrix;
    matrix.translate(translation.x(), translation.y(), 0.0);
    if(show3D){matrix.rotate(rotation);}
    matrix.scale( exp(m_distExp / 1200.0f));

//    glPointSize(20);
//    glLineWidth(5);

    pathwayProgram.bind();
    pathwayProgram.setUniformValue( "mvp_matrix",  orthographic * matrix );

    for( size_t i=0; i<nodeSize; i++ ){
    	string n=nodeVec[i].get_name();
    	for( size_t j=0; j<namesHighLightVec.size(); j++ ){
    		if( namesHighLightVec[j]==n ){
    			float   x=(nodeVec[i].get_x()-translateCenter.x())/range;
    			float   y=(nodeVec[i].get_y()-translateCenter.y())/range;
    			float   z=(nodeVec[i].get_z()-translateCenter.z())/range;

    			QVector4D v4(x,y,z,1);
    			QVector4D vr=orthographic * matrix*v4;

    			float 	r=0.04;

    			for(int i=0; i<60; i++){
    				double	angle=2*3.14159*i/60.0;
    				double	px=vr.x()+sin(angle)*r;
    				double	py=vr.y()+cos(angle)*r;
    				double 	pz=vr.z();
    				QVector3D v(px,py,pz);
    				vertexVec.push_back(v);
    				QVector3D c(0,1,0);
    				vertexColor.push_back(c);
    			}
    		}
    	}
    }

    QOpenGLBuffer vertexBuf;
    QOpenGLBuffer colorBuf;

    vertexBuf.create();
    colorBuf.create();

    colorBuf.bind();
    colorBuf.allocate(&vertexColor[0], vertexVec.size()*sizeof(QVector3D));
    vertexBuf.bind();
    vertexBuf.allocate( &vertexVec[0], vertexVec.size()*sizeof(QVector3D) );

    vertexBuf.bind();
    int texcoordLocation = pathwayProgram.attributeLocation("a_position");
    pathwayProgram.enableAttributeArray(texcoordLocation);
    pathwayProgram.setAttributeBuffer( texcoordLocation, GL_FLOAT, 0, 3, sizeof(QVector3D) );

    colorBuf.bind();
    int colorLocation = pathwayProgram.attributeLocation("color");
    pathwayProgram.enableAttributeArray(colorLocation);
    pathwayProgram.setAttributeBuffer( colorLocation, GL_FLOAT, 0, 3, sizeof(QVector3D) );

    glDrawArrays( GL_LINES, 0, vertexVec.size() );

    pathwayProgram.release();
    vertexBuf.destroy();
    colorBuf.destroy();
    update();
}

void
PathwayViewer::drawPathwayNode(){
    glEnable(GL_DEPTH_TEST);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	glEnable(GL_BLEND);
//	glEnable(GL_POINT_SMOOTH);
	glDisable(GL_POINT_SMOOTH);
	glHint(GL_POINT_SMOOTH_HINT,GL_NICEST);

    string vshaderFile = string("./shader/node_vs.glsl");
    string fshaderFile = string("./shader/node_fs.glsl");

    QOpenGLShaderProgram    pathwayProgram;
    if( !pathwayProgram.addShaderFromSourceFile(QOpenGLShader::Vertex,
            QString::fromStdString(vshaderFile)) ){
        cerr<<"can not load vertex shader!!!"<<endl;
    }
    if( !pathwayProgram.addShaderFromSourceFile(QOpenGLShader::Fragment,
            QString::fromStdString(fshaderFile)) ){
        cerr<<"can not load fragment shader!!!"<<endl;
    }
    if( !pathwayProgram.link() ){
        cerr<<"can not link shader!!!"<<endl;
    }
    if( !pathwayProgram.bind() ){
        cerr<<"can not bind shader!!!"<<endl;
    }

    vector<Node>    nodeVec=pathway.get_nodeVec();

    vector<QVector3D>   vertexVec;
    vector<QVector3D>   vertexColor;

    size_t  nodeSize = nodeVec.size();
    for( size_t i=0; i<nodeSize; i++ ){
        float   x=(nodeVec[i].get_x()-translateCenter.x())/range;
        float   y=(nodeVec[i].get_y()-translateCenter.y())/range;
        float   z=(nodeVec[i].get_z()-translateCenter.z())/range;
        QVector3D v(x,y,z);
        vertexVec.push_back(v);
        if(1){
        	int type=nodeVec[i].get_type();
        	QVector3D c(1,1,1);
        	if(type==0){
        		c=QVector3D(0,0,1);
        	}else if(type==1){
        		c=QVector3D(1,0,0);
        	}else if(type==2){
        		c=QVector3D(0,1,0);
        	}else if(type==3){
        		c=QVector3D(1,1,0);
        	}else if(type==4){
        		c=QVector3D(0,1,1);
        	}else if(type==5){
        		c=QVector3D(1,0,1);
        	}else{
        		c=QVector3D(1,1,1);
        	}
        	vertexColor.push_back(c);
        }
    }

    QOpenGLBuffer vertexBuf;
    QOpenGLBuffer colorBuf;

    vertexBuf.create();
    colorBuf.create();

    colorBuf.bind();
    colorBuf.allocate(&vertexColor[0], vertexVec.size()*sizeof(QVector3D));
    vertexBuf.bind();
    vertexBuf.allocate( &vertexVec[0], vertexVec.size()*sizeof(QVector3D) );

    QMatrix4x4 matrix;
    matrix.translate(translation.x(), translation.y(), 0.0);
    if(show3D){matrix.rotate(rotation);}
    matrix.scale( exp(m_distExp / 1200.0f));

    glPointSize(20);

    pathwayProgram.bind();
    pathwayProgram.setUniformValue( "mvp_matrix",  orthographic * matrix );

    vertexBuf.bind();
    int texcoordLocation = pathwayProgram.attributeLocation("a_position");
    pathwayProgram.enableAttributeArray(texcoordLocation);
    pathwayProgram.setAttributeBuffer( texcoordLocation, GL_FLOAT, 0, 3, sizeof(QVector3D));

    colorBuf.bind();
    int colorLocation = pathwayProgram.attributeLocation("color");
    pathwayProgram.enableAttributeArray(colorLocation);
    pathwayProgram.setAttributeBuffer( colorLocation, GL_FLOAT, 0, 3, sizeof(QVector3D) );

    glDrawArrays( GL_POINTS, 0, vertexVec.size() );

    pathwayProgram.release();
    vertexBuf.destroy();
    colorBuf.destroy();
}

void
PathwayViewer::drawPathwayNodeGene(){
    glEnable(GL_DEPTH_TEST);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	glEnable(GL_BLEND);
	glEnable(GL_POINT_SMOOTH);
	glHint(GL_POINT_SMOOTH_HINT,GL_NICEST);

    string vshaderFile = string("./shader/node_vs.glsl");
    string fshaderFile = string("./shader/node_fs.glsl");

    QOpenGLShaderProgram    pathwayProgram;
    if( !pathwayProgram.addShaderFromSourceFile(QOpenGLShader::Vertex,
            QString::fromStdString(vshaderFile)) ){
        cerr<<"can not load vertex shader!!!"<<endl;
    }
    if( !pathwayProgram.addShaderFromSourceFile(QOpenGLShader::Fragment,
            QString::fromStdString(fshaderFile)) ){
        cerr<<"can not load fragment shader!!!"<<endl;
    }
    if( !pathwayProgram.link() ){
        cerr<<"can not link shader!!!"<<endl;
    }
    if( !pathwayProgram.bind() ){
        cerr<<"can not bind shader!!!"<<endl;
    }

    vector<Node>    nodeVec=pathway.get_nodeVec();

    vector<QVector3D>   vertexVec;
    vector<QVector3D>   vertexColor;

    size_t  nodeSize = nodeVec.size();
    for( size_t i=0; i<nodeSize; i++ ){
		int type=nodeVec[i].get_type();
		QVector3D c(1,1,1);

    	if(type == 0){
    		float   x=(nodeVec[i].get_x()-translateCenter.x())/range;
    		float   y=(nodeVec[i].get_y()-translateCenter.y())/range;
    		float   z=(nodeVec[i].get_z()-translateCenter.z())/range;
    		QVector3D v(x,y,z);
    		vertexVec.push_back(v);

    		c=QVector3D(0,0,1);

    		vertexColor.push_back(c);
    	}
    }

    QOpenGLBuffer vertexBuf;
    QOpenGLBuffer colorBuf;

    vertexBuf.create();
    colorBuf.create();

    colorBuf.bind();
    colorBuf.allocate(&vertexColor[0], vertexVec.size()*sizeof(QVector3D));
    vertexBuf.bind();
    vertexBuf.allocate( &vertexVec[0], vertexVec.size()*sizeof(QVector3D) );

    QMatrix4x4 matrix;
    matrix.translate(translation.x(), translation.y(), 0.0);
    if(show3D){matrix.rotate(rotation);}
    matrix.scale( exp(m_distExp / 1200.0f));

    glPointSize(20);

    pathwayProgram.bind();
    pathwayProgram.setUniformValue( "mvp_matrix",  orthographic * matrix );

    vertexBuf.bind();
    int texcoordLocation = pathwayProgram.attributeLocation("a_position");
    pathwayProgram.enableAttributeArray(texcoordLocation);
    pathwayProgram.setAttributeBuffer( texcoordLocation, GL_FLOAT, 0, 3, sizeof(QVector3D) );

    colorBuf.bind();
    int colorLocation = pathwayProgram.attributeLocation("color");
    pathwayProgram.enableAttributeArray(colorLocation);
    pathwayProgram.setAttributeBuffer( colorLocation, GL_FLOAT, 0, 3, sizeof(QVector3D) );

    glDrawArrays( GL_POINTS, 0, vertexVec.size() );

    pathwayProgram.release();
    vertexBuf.destroy();
    colorBuf.destroy();
}

void
PathwayViewer::drawPathwayNodeCompound(){
    glEnable(GL_DEPTH_TEST);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	glEnable(GL_BLEND);
	glDisable(GL_POINT_SMOOTH);
	glHint(GL_POINT_SMOOTH_HINT,GL_NICEST);

    string vshaderFile = string("./shader/node_vs.glsl");
    string fshaderFile = string("./shader/node_fs.glsl");

    QOpenGLShaderProgram    pathwayProgram;
    if( !pathwayProgram.addShaderFromSourceFile(QOpenGLShader::Vertex,
            QString::fromStdString(vshaderFile)) ){
        cerr<<"can not load vertex shader!!!"<<endl;
    }
    if( !pathwayProgram.addShaderFromSourceFile(QOpenGLShader::Fragment,
            QString::fromStdString(fshaderFile)) ){
        cerr<<"can not load fragment shader!!!"<<endl;
    }
    if( !pathwayProgram.link() ){
        cerr<<"can not link shader!!!"<<endl;
    }
    if( !pathwayProgram.bind() ){
        cerr<<"can not bind shader!!!"<<endl;
    }

    vector<Node>    nodeVec=pathway.get_nodeVec();

    vector<QVector3D>   vertexVec;
    vector<QVector3D>   vertexColor;

    size_t  nodeSize = nodeVec.size();
    for( size_t i=0; i<nodeSize; i++ ){
		int type=nodeVec[i].get_type();
		QVector3D c(1,1,1);

    	if(type != 0 ){
    		float   x=(nodeVec[i].get_x()-translateCenter.x())/range;
    		float   y=(nodeVec[i].get_y()-translateCenter.y())/range;
    		float   z=(nodeVec[i].get_z()-translateCenter.z())/range;
    		QVector3D v(x,y,z);
    		vertexVec.push_back(v);

    		c=QVector3D(1,1,1);

    		vertexColor.push_back(c);
    	}
    }

    QOpenGLBuffer vertexBuf;
    QOpenGLBuffer colorBuf;

    vertexBuf.create();
    colorBuf.create();

    colorBuf.bind();
    colorBuf.allocate(&vertexColor[0], vertexVec.size()*sizeof(QVector3D));
    vertexBuf.bind();
    vertexBuf.allocate( &vertexVec[0], vertexVec.size()*sizeof(QVector3D) );

    QMatrix4x4 matrix;
    matrix.translate(translation.x(), translation.y(), 0.0);
    if(show3D){matrix.rotate(rotation);}
    matrix.scale( exp(m_distExp / 1200.0f));

    glPointSize(20);

    pathwayProgram.bind();
    pathwayProgram.setUniformValue( "mvp_matrix",  orthographic * matrix );

    vertexBuf.bind();
    int texcoordLocation = pathwayProgram.attributeLocation("a_position");
    pathwayProgram.enableAttributeArray(texcoordLocation);
    pathwayProgram.setAttributeBuffer( texcoordLocation, GL_FLOAT, 0, 3, sizeof(QVector3D) );

    colorBuf.bind();
    int colorLocation = pathwayProgram.attributeLocation("color");
    pathwayProgram.enableAttributeArray(colorLocation);
    pathwayProgram.setAttributeBuffer( colorLocation, GL_FLOAT, 0, 3, sizeof(QVector3D) );

    glDrawArrays( GL_POINTS, 0, vertexVec.size() );

    pathwayProgram.release();
    vertexBuf.destroy();
    colorBuf.destroy();
}

void
PathwayViewer::drawNodeLabel(){
	glDisable(GL_DEPTH_TEST);
    /* Enable blending, necessary for our alpha texture */

	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    glHint(GL_TEXTURE_COMPRESSION_HINT, GL_NICEST);

    glPixelStorei(GL_UNPACK_ALIGNMENT, 1);

    // Set texture options
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

//    float scale =  exp(m_distExp / 1000.0f)/9.0;
//    cout<<"scale: "<<scale<<endl;
    float scale = 1.0;

    float sx = (1.0/width()) * scale;
    float sy = (1.0/height()) * scale;


    vector<Node>    nodeVec=pathway.get_nodeVec();
    size_t 			nodeVecSize=nodeVec.size();



    for( size_t i=0; i<nodeVecSize; i++ ){

        float   x=(nodeVec[i].get_x()-translateCenter.x())/range;
        float   y=(nodeVec[i].get_y()-translateCenter.y())/range;
        float   z=(nodeVec[i].get_z()-translateCenter.z())/range;

        // Calculate model view transformation
        QMatrix4x4 matrix;
        matrix.translate(translation.x(), translation.y(), 0.0);
        if(show3D){matrix.rotate(rotation);}
        matrix.scale( exp(m_distExp / 1200.0f));

        //ensure the texture always face the customer rather than rotate in 3D
        QVector4D   v4d(x, y, z, 1);
        QVector4D   rot4d = orthographic * matrix * v4d;
        size_t		rowCount = 1;
        if( rot4d.x()*rot4d.x()<1.0 && rot4d.y()*rot4d.y()<1.0 ){
        	float       rotX=rot4d.x()+15*sx;
        	float       rotY=rot4d.y();

        	const char*  label=nodeVec[i].get_name().c_str();
        	size_t characterIndex = 0;

        	string name=nodeVec[i].get_name();
        	size_t nameSize=name.size();
        	for( size_t j=0; j<nameSize; j++ ){
        		char p=name.at(j);

        		if( p==' ' ){
        			if( nameSize>20 && j>(nameSize/2*rowCount) ){
        				rotX = rot4d.x()+15*sx;
        				rotY = rot4d.y() - 25*sy;
        				rowCount=rowCount+100;
        			}else{
        				rotX+=10*sx;
        			}
        		}else{
//        			/* Try to load and render the character */
//        			if( !FT_Load_Char(face, p, FT_LOAD_RENDER)){
////        				cout<<"can not load "<<p<<" in "<<nodeVec[i].get_name()<<endl;;
//        			}else{
////        				cout<<"##############333"<<endl;
//        			}
//        			FT_GlyphSlot    g=face->glyph;

        			//http://learnopengl.com/#!In-Practice/Text-Rendering
        			/* Calculate the vertex and texture coordinates */
//        			float x2=rotX+g->bitmap_left*sx;
//        			float y2=rotY-(g->bitmap.rows-g->bitmap_top)*sy; //make sure characters like'b,p' are vertical hint
//        			float w = g->bitmap.width * sx;
//        			float h = g->bitmap.rows * sy;

//        			GLfloat vertices[4][4]={
//        					{x2,     y2+h, 0, 0},
//							{x2 + w, y2+h, 1, 0},
//							{x2,     y2, 0, 1},
//							{x2 + w, y2, 1, 1},
//        			};

        			renderTextProgram.bind();
        			QVector4D color(0,0,0,1);
        			renderTextProgram.setUniformValue("color", color);

        			glBindTexture(GL_TEXTURE_2D, textureIdVec[i][characterIndex]);

        			//correspond the init_render_resources function 'if( p!=' ' ){'
        			characterIndex++;

        			renderTextProgram.setUniformValue("tex", 0);

        			QOpenGLBuffer boxBuf;
        			boxBuf.create();
        			boxBuf.bind();
//        			boxBuf.allocate(vertices, sizeof(vertices));

        			boxBuf.bind();
        			int coordLocation=renderTextProgram.attributeLocation("coord");
        			renderTextProgram.enableAttributeArray(coordLocation);
        			renderTextProgram.setAttributeBuffer(coordLocation, GL_FLOAT, 0, 4);

        		    glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);

        			boxBuf.destroy();

        			renderTextProgram.release();
        			/* Advance the cursor to the start of the next character */
//        			rotX += ((g->advance.x)>>6)*sx;
        		}
        	}

        }
    }
}

void
PathwayViewer::render_text(const char *text, float x, float y, float z,
                           float sx, float sy){
    const char* p;
//    FT_GlyphSlot    g=face->glyph;

    /* Create a texture that will be used to hold one "glyph" */
    GLuint tex;

    glActiveTexture(GL_TEXTURE0);
    glGenTextures(1, &tex);
    glBindTexture(GL_TEXTURE_2D, tex);

    /* We require 1 byte alignment when uploading texture data */
    glPixelStorei(GL_UNPACK_ALIGNMENT, 1);

    /* Clamping to edges is important to prevent artifacts when scaling */
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

    /* Linear filtering usually looks best for text */
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

//    glEnable(GL_POLYGON_SMOOTH);

    // Calculate model view transformation
    QMatrix4x4 matrix;
    matrix.translate(translation.x(), translation.y(), 0.0);
    if(show3D){matrix.rotate(rotation);}
    matrix.scale( exp(m_distExp / 1200.0f));

    //ensure the texture always face the customer rather than rotate in 3D
    QVector4D   v4d(x, y, z, 1);
    QVector4D   rot4d = orthographic * matrix * v4d;
    float       rotX=rot4d.x();
    float       rotY=rot4d.y();

    renderTextProgram.bind();
    QVector4D color(0,0,0,1);
    renderTextProgram.setUniformValue("color", color);

    for(p=text; *p; p++){
        /* Try to load and render the character */
//        if (FT_Load_Char(face, *p, FT_LOAD_RENDER))
//            continue;

        /* Calculate the vertex and texture coordinates */
//    	float x2 = rotX + g->bitmap.width * sx ;
//        float y2 = rotY;
//        float w = g->bitmap.width * sx;
//        float h = g->bitmap.rows * sy;

//        GLfloat vertices[6][4]={
//            { x2,     y2 + h,  0.0, 0.0 },
//            { x2 + w, y2,      1.0, 1.0 },
//            { x2,     y2,      0.0, 1.0 },
//
//            { x2,     y2 + h,   0.0, 0.0 },
//            { x2 + w, y2 + h,   1.0, 0.0 },
//            { x2 + w, y2,       1.0, 1.0 }
//        };
//
//        QImage  glyphImage(g->bitmap.buffer,
//                           g->bitmap.width,
//                           g->bitmap.rows,
//                           g->bitmap.pitch,
//                           QImage::Format_Indexed8);
//        QImage glyphImage;

//        QOpenGLTexture*     texture =new QOpenGLTexture(glyphImage);
//        texture->bind();
//
//        renderTextProgram.setUniformValue("tex", 0);
//
//        QOpenGLBuffer boxBuf;
//        boxBuf.create();
//        boxBuf.bind();
//        boxBuf.allocate(vertices, sizeof(vertices));
//
//        boxBuf.bind();
//        int coordLocation=renderTextProgram.attributeLocation("coord");
//        renderTextProgram.enableAttributeArray(coordLocation);
//        renderTextProgram.setAttributeBuffer(coordLocation, GL_FLOAT, 0, 4);
//
//        glPointSize(20);
//        glDrawArrays(GL_TRIANGLES, 0, 6);
//        boxBuf.destroy();
//        texture->destroy();
//        delete texture;
//
//        /* Advance the cursor to the start of the next character */
//        rotX += g->bitmap.width * sx;
    }
    renderTextProgram.release();
}

void
PathwayViewer::drawAxis(){
    glEnable( GL_BLEND );
    glEnable(GL_LINE_SMOOTH);

    vector<QVector3D> axis;
    vector<QVector3D> axisColor;
    axis.push_back( QVector3D(0, 0, 0) );
    axis.push_back( QVector3D(0, 0, 0.5) );
    axis.push_back( QVector3D(0, 0, 0) );
    axis.push_back( QVector3D(0, 0.5, 0) );
    axis.push_back( QVector3D(0, 0, 0) );
    axis.push_back( QVector3D(0.5, 0, 0) );

    axisColor.push_back( QVector3D(1, 0, 0) );
    axisColor.push_back( QVector3D(1, 0, 0) );
    axisColor.push_back( QVector3D(0, 1, 0) );
    axisColor.push_back( QVector3D(0, 1, 0) );
    axisColor.push_back( QVector3D(0, 0, 1) );
    axisColor.push_back( QVector3D(0, 0, 1) );

    QOpenGLBuffer vertexBuf;//( QOpenGLBuffer::VertexBuffer );
    vertexBuf.create();
    vertexBuf.setUsagePattern(QOpenGLBuffer::StaticDraw);
    vertexBuf.bind();
    vertexBuf.allocate( &axis[0], 6*sizeof(QVector3D) );

    QOpenGLBuffer colorBuf( QOpenGLBuffer::VertexBuffer );
    colorBuf.create();
    colorBuf.setUsagePattern( QOpenGLBuffer::StaticDraw );
    colorBuf.bind();
    colorBuf.allocate(&axisColor[0], 6*sizeof(QVector3D));

    QMatrix4x4 matrix;
    matrix.translate(translation.x(), translation.y(), 0.0);
    if(show3D){matrix.rotate(rotation);}
    matrix.scale( exp(m_distExp / 1200.0f));
    QMatrix4x4 matrixR;
    matrixR.rotate(rotation);
    matrixR.scale( exp(m_distExp / 1200.0f));

//    glPointSize(50);
//    glLineWidth(2);
    axisProgram.bind();
    vertexBuf.bind();
    axisProgram.setUniformValue( "mvp_matrix",  orthographic * matrix );
    axisProgram.enableAttributeArray("position");
    axisProgram.setAttributeBuffer( "position", GL_FLOAT, 0, 3,  sizeof(QVector3D) );
    colorBuf.bind();
    axisProgram.enableAttributeArray("vcolor");
    axisProgram.setAttributeBuffer("vcolor", GL_FLOAT, 0, 3, sizeof(QVector3D) );
    glDrawArrays(GL_LINES, 0, axis.size() );

    axisProgram.release();
    vertexBuf.destroy();
    colorBuf.destroy();
}

void
PathwayViewer::drawTest( const QSize &windowSize ){
//    glEnableClientState( GL_VERTEX_ARRAY );
    string vshaderFile = string("./shader/test_vs.glsl");
    string fshaderFile = string("./shader/test_fs.glsl");
//    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

//    initializeOpenGLFunctions();
    QOpenGLShaderProgram testProgram;

    if( !testProgram.addShaderFromSourceFile(QOpenGLShader::Vertex,
            QString::fromStdString(vshaderFile)) ){
        cerr<<"can not load vertex shader!!!"<<endl;
    }
    if( !testProgram.addShaderFromSourceFile(QOpenGLShader::Fragment,
            QString::fromStdString(fshaderFile)) ){
        cerr<<"can not load fragment shader!!!"<<endl;
    }
    if( !testProgram.link() ){
        cerr<<"can not link shader!!!"<<endl;
    }
    if( !testProgram.bind() ){
        cerr<<"can not bind shader!!!"<<endl;
    }

    vector<QVector3D> axis;
    vector<QVector3D> axisColor;
    axis.push_back( QVector3D(0, 0, 0) );
    axis.push_back( QVector3D(0, 0, 0.5) );
    axis.push_back( QVector3D(0, 0, 0) );
    axis.push_back( QVector3D(0, 0.5, 0) );
    axis.push_back( QVector3D(0, 0, 0) );
    axis.push_back( QVector3D(0.5, 0, 0) );

    axisColor.push_back( QVector3D(1, 0, 0) );
    axisColor.push_back( QVector3D(1, 0, 0) );
    axisColor.push_back( QVector3D(0, 1, 0) );
    axisColor.push_back( QVector3D(0, 1, 0) );
    axisColor.push_back( QVector3D(0, 0, 1) );
    axisColor.push_back( QVector3D(0, 0, 1) );

    QOpenGLBuffer vertexBuf;
    QOpenGLBuffer colorBuf;

    vertexBuf.create();
    colorBuf.create();

    colorBuf.bind();
    colorBuf.allocate(&axisColor[0], 6*sizeof(QVector3D));
    vertexBuf.bind();
    vertexBuf.allocate( &axis[0], 6*sizeof(QVector3D) );

    QMatrix4x4 matrix;
    matrix.translate(translation.x(), translation.y(), 0.0);
    if(show3D){matrix.rotate(rotation);}
    matrix.scale( exp(m_distExp / 1200.0f));

    QMatrix4x4 matrixR;
    matrixR.rotate(rotation);
    matrixR.scale( exp(m_distExp / 1200.0f));

    glPointSize(20);

    testProgram.bind();
    testProgram.setUniformValue( "mvp_matrix",  orthographic * matrix );

    int texcoordLocation = testProgram.attributeLocation("a_position");
    testProgram.enableAttributeArray(texcoordLocation);
    testProgram.setAttributeBuffer( texcoordLocation, GL_FLOAT, 0, 3, sizeof(QVector3D) );

    glDrawArrays( GL_POINTS, 0, axis.size() );

    testProgram.release();
    vertexBuf.destroy();
    colorBuf.destroy();
}

void
PathwayViewer::drawTestTexture( const QSize &windowSize ){

    initializeOpenGLFunctions();
    glClearColor(0, 0, 0, 1);

//! [2]
    // Enable depth buffer
    glEnable(GL_DEPTH_TEST);

    // Enable back face culling
    glEnable(GL_CULL_FACE);

    string vshaderFile = string("./shader/test_texture_vs.glsl");
    string fshaderFile = string("./shader/test_texture_fs.glsl");

    QOpenGLShaderProgram testProgram;

    if( !testProgram.addShaderFromSourceFile(QOpenGLShader::Vertex,
            QString::fromStdString(vshaderFile)) ){
        cerr<<"can not load vertex shader!!!"<<endl;
    }
    if( !testProgram.addShaderFromSourceFile(QOpenGLShader::Fragment,
            QString::fromStdString(fshaderFile)) ){
        cerr<<"can not load fragment shader!!!"<<endl;
    }
    if( !testProgram.link() ){
        cerr<<"can not link shader!!!"<<endl;
    }
    if( !testProgram.bind() ){
        cerr<<"can not bind shader!!!"<<endl;
    }

    QOpenGLTexture *texture;
    // Load cube.png image
    texture = new QOpenGLTexture(QImage("cube.png").mirrored());

//    cout<<"height:"<<texture->height()<<"  width:"<<texture->width()<<endl;

    // Set nearest filtering mode for texture minification
    texture->setMinificationFilter(QOpenGLTexture::Nearest);

    // Set bilinear filtering mode for texture magnification
    texture->setMagnificationFilter(QOpenGLTexture::Linear);

    // Wrap texture coordinates by repeating
    // f.ex. texture coordinate (1.1, 1.2) is same as (0.1, 0.2)
    texture->setWrapMode(QOpenGLTexture::Repeat);

    // Clear color and depth buffer
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    texture->bind();

    //! [6]
    // Calculate model view transformation
    QMatrix4x4 matrix;
    matrix.translate(translation.x(), translation.y(), 0.0);
    if(show3D){matrix.rotate(rotation);}
    matrix.scale( exp(m_distExp / 1200.0f));

    // Set modelview-projection matrix
    testProgram.setUniformValue("mvp_matrix", orthographic * matrix);


    // Use texture unit 0 which contains cube.png
    texture->bind();
    testProgram.setUniformValue("texture", 0);

    struct VertexData
    {
        QVector3D position;
        QVector2D texCoord;
    };

    VertexData vertices[] = {
        // Vertex data for face 0
        {QVector3D(-1.0f, -1.0f,  1.0f), QVector2D(0.0f, 0.0f)},  // v0
        {QVector3D( 1.0f, -1.0f,  1.0f), QVector2D(0.33f, 0.0f)}, // v1
        {QVector3D(-1.0f,  1.0f,  1.0f), QVector2D(0.0f, 0.5f)},  // v2
        {QVector3D( 1.0f,  1.0f,  1.0f), QVector2D(0.33f, 0.5f)}, // v3

        // Vertex data for face 1
        {QVector3D( 1.0f, -1.0f,  1.0f), QVector2D( 0.0f, 0.5f)}, // v4
        {QVector3D( 1.0f, -1.0f, -1.0f), QVector2D(0.33f, 0.5f)}, // v5
        {QVector3D( 1.0f,  1.0f,  1.0f), QVector2D(0.0f, 1.0f)},  // v6
        {QVector3D( 1.0f,  1.0f, -1.0f), QVector2D(0.33f, 1.0f)}, // v7

        // Vertex data for face 2
        {QVector3D( 1.0f, -1.0f, -1.0f), QVector2D(0.66f, 0.5f)}, // v8
        {QVector3D(-1.0f, -1.0f, -1.0f), QVector2D(1.0f, 0.5f)},  // v9
        {QVector3D( 1.0f,  1.0f, -1.0f), QVector2D(0.66f, 1.0f)}, // v10
        {QVector3D(-1.0f,  1.0f, -1.0f), QVector2D(1.0f, 1.0f)},  // v11

        // Vertex data for face 3
        {QVector3D(-1.0f, -1.0f, -1.0f), QVector2D(0.66f, 0.0f)}, // v12
        {QVector3D(-1.0f, -1.0f,  1.0f), QVector2D(1.0f, 0.0f)},  // v13
        {QVector3D(-1.0f,  1.0f, -1.0f), QVector2D(0.66f, 0.5f)}, // v14
        {QVector3D(-1.0f,  1.0f,  1.0f), QVector2D(1.0f, 0.5f)},  // v15

        // Vertex data for face 4
        {QVector3D(-1.0f, -1.0f, -1.0f), QVector2D(0.33f, 0.0f)}, // v16
        {QVector3D( 1.0f, -1.0f, -1.0f), QVector2D(0.66f, 0.0f)}, // v17
        {QVector3D(-1.0f, -1.0f,  1.0f), QVector2D(0.33f, 0.5f)}, // v18
        {QVector3D( 1.0f, -1.0f,  1.0f), QVector2D(0.66f, 0.5f)}, // v19

        // Vertex data for face 5
        {QVector3D(-1.0f,  1.0f,  1.0f), QVector2D(0.33f, 0.5f)}, // v20
        {QVector3D( 1.0f,  1.0f,  1.0f), QVector2D(0.66f, 0.5f)}, // v21
        {QVector3D(-1.0f,  1.0f, -1.0f), QVector2D(0.33f, 1.0f)}, // v22
        {QVector3D( 1.0f,  1.0f, -1.0f), QVector2D(0.66f, 1.0f)}  // v23
    };

    GLushort indices[] = {
         0,  1,  2,  3,  3,     // Face 0 - triangle strip ( v0,  v1,  v2,  v3)
         4,  4,  5,  6,  7,  7, // Face 1 - triangle strip ( v4,  v5,  v6,  v7)
         8,  8,  9, 10, 11, 11, // Face 2 - triangle strip ( v8,  v9, v10, v11)
        12, 12, 13, 14, 15, 15, // Face 3 - triangle strip (v12, v13, v14, v15)
        16, 16, 17, 18, 19, 19, // Face 4 - triangle strip (v16, v17, v18, v19)
        20, 20, 21, 22, 23      // Face 5 - triangle strip (v20, v21, v22, v23)
    };

    QOpenGLBuffer arrayBuf;
    QOpenGLBuffer indexBuf;
//! [1]
    // Transfer vertex data to VBO 0
    arrayBuf.create();
    arrayBuf.bind();
    arrayBuf.allocate(vertices, 24 * sizeof(VertexData));

    // Transfer index data to VBO 1
    indexBuf.create();
    indexBuf.bind();
    indexBuf.allocate(indices, 34 * sizeof(GLushort));

    // Offset for position
    quintptr offset = 0;

    arrayBuf.bind();

    // Tell OpenGL programmable pipeline how to locate vertex position data
    int vertexLocation = testProgram.attributeLocation("a_position");

    testProgram.enableAttributeArray(vertexLocation);
    testProgram.setAttributeBuffer(vertexLocation, GL_FLOAT, offset, 3, sizeof(VertexData));

    // Offset for texture coordinate
    offset += sizeof(QVector3D);

    // Tell OpenGL programmable pipeline how to locate vertex texture coordinate data
    int texcoordLocation = testProgram.attributeLocation("a_texcoord");
//    arrayBuf.bind();
    testProgram.enableAttributeArray(texcoordLocation);
    testProgram.setAttributeBuffer(texcoordLocation, GL_FLOAT, offset, 2, sizeof(VertexData));


//    // Draw cube geometry using indices from VBO 1
//    indexBuf.bind();
//    glDrawElements(GL_TRIANGLE_STRIP, 34, GL_UNSIGNED_SHORT, 0);
    glPointSize(20);
//    glDrawArrays(GL_POINTS, 0, 24);
    glDrawArrays(GL_TRIANGLE_STRIP, 0, 24);

}
