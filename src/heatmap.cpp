/*
 * heatmap.cpp
 *
 *  Created on: Oct 28, 2016
 *      Author: stan
 */

#include"heatmap.h"

HeatmapViewer::HeatmapViewer(QWidget *parent): QWidget(parent)
{
    shape = Rect;
    antialiased = false;
    dataMatrix.clear();
    rowNameVec.clear();
    colNameVec.clear();
    columnClass.clear();

    setBackgroundRole(QPalette::Base);
    setAutoFillBackground(true);
    scale = 1;
}

QSize HeatmapViewer::minimumSizeHint() const
{
    return QSize(100, 100);
}

QSize HeatmapViewer::sizeHint() const
{
    return QSize(400, 200);
}

bool
HeatmapViewer::setData(  vector<vector<float> > dm,
		vector<string> rn,
		vector<string> cn,
		vector<string> cc,
		bool rowNormalize ){

	dataMatrix.clear();
	rowNameVec.clear();
	colNameVec.clear();
	columnClass.clear();

	for( size_t i=0; i<dm.size(); i++ ){
		QVector<float>	tempVec;
		for( size_t j=0; j<dm[0].size(); j++ ){
			tempVec.push_back( dm[i][j] );
		}
		dataMatrix.push_back( tempVec );
	}
	for( size_t i=0; i<rn.size(); i++ ){
		rowNameVec.push_back( rn[i] );
	}
	for( size_t i=0; i<cn.size(); i++ ){
		colNameVec.push_back( cn[i] );
	}
	for( size_t i=0; i<cc.size(); i++ ){
		columnClass.push_back( cc[i] );
	}

	if( dataMatrix.size() != rowNameVec.size() ){
		cerr<<"!!! size match error: data matrix and row name"<<dataMatrix.size()<<"\t"<<dataMatrix[0].size()<<"\t"<<rowNameVec.size()<<endl;
		return false;
	}else if( dataMatrix[0].size() != colNameVec.size() ){
		cerr<<"!!! size match error: data matrix and column name"<<dataMatrix[0].size()<<"\t"<<colNameVec.size()<<endl;
		for(size_t i=0; i<dataMatrix[0].size(); i++){cout<<dataMatrix[0][i]<<"\t";}
		cout<<endl;
		for(size_t i=0; i<colNameVec.size(); i++){cout<<colNameVec[i]<<"\t";}
//		return false;
	}else if( dataMatrix[0].size() != columnClass.size() ){
		cerr<<"!!! size match error: data matrix and column class:"<<dataMatrix[0].size()<<"\t"<<columnClass.size()<<endl;

		return false;
	}

	//
	/*
	 * normalization: first take log ration, then divide max value to range [-1,1]
	 */
	cout<<"row normalize:"<<rowNormalize<<endl;
	if( rowNormalize ){
		int rowNum = dataMatrix.size();
		int colNum = dataMatrix[0].size();
		max = -10e10;
		min = 10e10;
		//similar as scale in R heatmap.2
		//for each row: first substrate row mean then divide the
		//standard deviation of that row (after substrate row mean).
		for( size_t i=0; i<rowNum; i++ ){
			float mean = 0;
			for( size_t j=0; j<colNum; j++ ){
				mean += dataMatrix[i][j];
			}
			mean /= (float)dataMatrix[i].size();
			float sd = 0;
			for( size_t j=0; j<colNum; j++ ){
				dataMatrix[i][j] = dataMatrix[i][j] - mean;
				sd += dataMatrix[i][j]*dataMatrix[i][j];
			}
			sd = sqrt(sd);
			if( sd != 0 ){
				for( size_t j=0; j<colNum; j++ ){
					dataMatrix[i][j] = dataMatrix[i][j]/sd;
				}
			}

			for( size_t j=0; j<colNum; j++ ){
				max = max > dataMatrix[i][j] ? max:dataMatrix[i][j];
				min = min < dataMatrix[i][j] ? min:dataMatrix[i][j];
			}
		}
	}

	update();
	return true;
}

void HeatmapViewer::setShape(Shape shape)
{
    this->shape = shape;
    update();
}

void HeatmapViewer::setPen(const QPen &pen)
{
    this->pen = pen;
    update();
}

void HeatmapViewer::setBrush(const QBrush &brush)
{
    this->brush = brush;
    update();
}

void HeatmapViewer::setAntialiased(bool antialiased)
{
    this->antialiased = antialiased;
    update();
}

void HeatmapViewer::paintEvent(QPaintEvent * /* event */)
{
	if( dataMatrix.empty() ){
		return;
	}
    static const QPoint points[4] = {
        QPoint(10, 80),
        QPoint(20, 10),
        QPoint(80, 30),
        QPoint(90, 70)
    };

    QPainter painter(this);


    mouseSave += mouseMove;
    mouseSave.setX( mouseSave.x() < 0 ? mouseSave.x():0 );
    mouseSave.setY( mouseSave.y() < 0 ? mouseSave.y():0 );
    painter.translate(mouseSave.x(),mouseSave.y());
    painter.scale(scale,scale);

    painter.setPen(pen);
    painter.setBrush(brush);
    if (antialiased)
        painter.setRenderHint(QPainter::Antialiasing, true);

    float xStep = (width()*4.0/5.0)/(float)dataMatrix[0].size();
    float yStep = (height()*4.0/5.0)/(float)dataMatrix.size();

    /*
     * draw gene name
     */
    for(int j = 0; j < rowNameVec.size(); j++){
    	int y = height()/10.0+j*yStep;

    	float numOfCharacter = 9.0;
    	float widthSize = width()/10.0/numOfCharacter;
    	float heightSize = yStep;
    	float fontSize = widthSize < heightSize ? widthSize : heightSize;


    	painter.setFont(QFont("times", fontSize));
    	painter.drawText(0,y,width()/10.0-2,height()/10.0,Qt::AlignRight, QString(rowNameVec[j].c_str()) );
    }

    /*
     * draw class label heatmap
     */
	float numOfCharacter = 9.0;
	float fontSize = xStep/numOfCharacter;
	string preV;
	int colorIndex = 0;
	int y = height()/10.0-2*yStep;

	int x = width()/10.0;
	int step = 0;

    for( int i=0; i<columnClass.size(); i++ ){
    	cout<<i<<"\t"<<columnClass[i]<<endl;
    	step ++;
		painter.save();
		painter.translate(x, y);

		if( i>1 and columnClass[i] != preV || i==columnClass.size()-1 ){
			QRect rect(0,0, (step)*xStep, yStep);

			step = 0;
			QColor color;
			if( colorIndex == 0 ){
				QColor c(255,0,0);
				color = c;
				colorIndex++;
			}else if( colorIndex == 1 ){
				QColor c(0,0,255);
				color = c;
				colorIndex++;
			}
			preV = columnClass[i];
			QPen pen( color );
			painter.setPen(pen);
			painter.drawRect(rect);
			painter.fillRect(rect,color);
			y = height()/10.0-2*yStep;
			x = width()/10.0+ i*xStep;
			step++;
		}else{
			preV = columnClass[i];
		}
		painter.restore();
    }

    /*
     * draw data heatmap
     */
    for (int i = 0; i<dataMatrix.size(); i++) {
    	int y =height()/10.0+ i*yStep;
    	fontSize = fontSize > 1 ? fontSize: 1.0;
    	painter.setFont(QFont("times",fontSize));

    	for (int j = 0; j < dataMatrix[i].size(); j++) {
    		int x = width()/10.0+j*xStep;
    		painter.save();
    		painter.translate(x, y);

    		QRect rect(0,0, xStep, yStep);
    		QColor color;
    		if( dataMatrix[i][j]<0 ){
    			float b=dataMatrix[i][j]/min;
    			b = b*255;
    			QColor c(255-b,255-b,255);
    			color = c;
    		}else{
    			float r=dataMatrix[i][j]/max;
    			r = r*255;
    			QColor c(255,255-r,255-r);
    			color = c;
    		}
    		QPen pen( color );
    		painter.setPen(pen);
    		painter.drawRect(rect);
    		painter.fillRect(rect,color);
    		painter.restore();
    	}
    }

    painter.setRenderHint(QPainter::Antialiasing, false);
    painter.setPen(palette().dark().color());
    painter.setBrush(Qt::NoBrush);
    painter.drawRect(QRect(0, 0, width() - 1, height() - 1));
}

void
HeatmapViewer::wheelEvent(QWheelEvent *event){
	float wheel = event->delta()/120;//n or -n
	float zoom = pow(1.0 + abs(wheel)/4.0 , wheel > 0 ? 1 : -1);
	event->accept();
	if(1){
		if(!(scale==1&&zoom<1)){
			QPoint p=event->pos();
			QPoint cursorP=event->pos();
			QPoint s(mouseSave.x(),mouseSave.y());
			QPoint p1=(cursorP-s)*(zoom-1);
			mouseSave = mouseSave-p1;
		}
	}

	scale*= zoom;
	if(scale<=1){
		scale=1;
		mouseSave=QPoint(0,0);
	}
	update();
}

void
HeatmapViewer::mouseMoveEvent(QMouseEvent *event){
	if( event->buttons() & Qt::RightButton ){
		QPointF n = event->localPos();
		mouseMove = n - mousePos;
		mousePos = event->localPos();
		mouseSave += mouseMove;
		update();
	}
}

void
HeatmapViewer::mousePressEvent(QMouseEvent *event){
	if( event->buttons() & Qt::RightButton ){
		mouseMove.setX(0);
		mouseMove.setY(0);
		mousePos = event->localPos();
	}
}


/*****************************************************************
 *
 *           HeatmapDialog
 *
 *****************************************************************/

HeatmapDialog::HeatmapDialog(){

	QLabel* inputDataLabel = new QLabel( tr("Input File:") );
	fileEdit = new QLineEdit;
	QPushButton* loadFileButton = new QPushButton( tr("Load") );
	connect( loadFileButton, SIGNAL(clicked()), this, SLOT(loadFile()) );

	QHBoxLayout* fileLayout = new QHBoxLayout;
	fileLayout->addWidget(inputDataLabel);
	fileLayout->addWidget(fileEdit);
	fileLayout->addWidget(loadFileButton);

	QCheckBox* normalizeCheckBox = new QCheckBox( QString(tr("Normalize Data:")) );
	normalizeCheckBox->setCheckState(Qt::Checked);

	QPushButton* submitButton = new QPushButton( tr("Submit") );
	connect( submitButton, SIGNAL(clicked()), this, SLOT(run()));

	QLabel* geneListLabel = new QLabel( tr("Gene List:") );
	geneEdit = new QTextEdit(tr(""));
	QVBoxLayout* geneLayout = new QVBoxLayout;
	geneLayout->addWidget( geneListLabel );
	geneLayout->addWidget( geneEdit );
	geneLayout->addWidget( normalizeCheckBox );
	geneLayout->addWidget( submitButton );

	QVBoxLayout* mainLayout = new QVBoxLayout;
	mainLayout->addLayout( fileLayout );
	mainLayout->addLayout( geneLayout );

	setLayout(mainLayout);
}

void
HeatmapDialog::loadFile(){
	fileName = QFileDialog::getOpenFileName(this,
			tr("Open RNA-seq Data File"), ".",
			tr("*"));
	fileEdit->insert( fileName );
}

void
HeatmapDialog::run(){
	geneVec.clear();
	annotation = new Annotation();
	annotation->readFile( fileName.toStdString() );
	QString genes = geneEdit->toPlainText();
	cout<<genes.toStdString()<<endl;
	vector<string> gene = tokenByNewLine( genes.toStdString() );
	for( size_t i=0; i<gene.size(); i++ ){
		geneVec.push_back( gene[i] );
	}
	cout<<"geneVec: "<<geneVec.size()<<endl;
	emit finish();
}

