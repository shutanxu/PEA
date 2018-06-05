/*
 * pathviewer2.cpp
 *
 *  Created on: Mar 3, 2017
 *      Author: stan
 */

#include"pathviewer2.h"
#include"pathway.h"
using namespace std;

PathwayViewer2::PathwayViewer2(QWidget* parent):QWidget(parent){
    setBackgroundRole(QPalette::Base);
    setAutoFillBackground(true);
    translateCenter = QVector2D(0,0);
    showLabel = false;
    showEdgeLabel = false;
    scale=1;
    mouseSave=QPoint(0,0);
    QPen p(Qt::black,0.5 );
    pen=p;
    selectedNode=-1;
    dragConnected = false;
    dragFixedEdge = false;
    highLightName="---";
}

QSize PathwayViewer2::minimumSizeHint() const
{
    return QSize(50, 50);
}

QSize PathwayViewer2::sizeHint() const
{
    return QSize(400, 200);
}

void PathwayViewer2::setPen(const QPen &pen)
{
    this->pen = pen;
    update();
}

void PathwayViewer2::setBrush(const QBrush &brush)
{
    this->brush = brush;
    update();
}

void PathwayViewer2::setAntialiased(bool antialiased)
{
    this->antialiased = antialiased;
    update();
}

void
PathwayViewer2::wheelEvent(QWheelEvent *event){
	float wheel = event->delta()/120;//n or -n
	float zoom = pow(1.0 + abs(wheel)/4.0 , wheel > 0 ? 1 : -1);

	event->accept();

	if(!(scale==1&&zoom<1)){
		QPoint p=event->pos();
		QPoint cursorP=event->pos();
		QPoint s(mouseSave.x(),mouseSave.y());
		QPoint p1=(cursorP-s)*(zoom-1);
		mouseSave = mouseSave-p1;
	}

	scale*= zoom;
//	if( scale<=1){
//    	scale = 1;
//        mouseSave=QPoint(0,0);
//    }
	update();
}

void
PathwayViewer2::mouseMoveEvent(QMouseEvent *event){
	if( event->buttons() & Qt::RightButton ){
		QPointF n = event->localPos();
		mouseMove = n - mousePos;
		mousePos = event->localPos();
		mouseSave += mouseMove;
		update();
	}
	if( (event->buttons() & Qt::LeftButton) && selectedNode>=0 ){
		QPointF	eventPos=event->localPos();
		QPointF	nodeP=painterWorldTransf.inverted()*eventPos;
		vector<Node>	nodeVec = pathway.get_nodeVec();
		double x0=nodeVec[selectedNode].get_x();
		double y0=nodeVec[selectedNode].get_y();

		double x= ( (nodeP.x()-width()*0.1)*(pathway.get_maxX()-pathway.get_minX()) )/(width()*0.8)+pathway.get_minX();
		double y= ( (nodeP.y()-width()*0.1)*(pathway.get_maxY()-pathway.get_minY()) )/(width()*0.8)+pathway.get_minY();

		//move with fixed edge length.(only works for single degree node)

		if( dragFixedEdge ){
			vector<Edge> edgeVec = pathway.get_edgeVec();

			size_t	edgeCount=0;
			float	edgeLength=0;
			float	sourceX=0;
			float	sourceY=0;
			for( size_t i=0; i<edgeVec.size(); i++ ){
				Edge	e=edgeVec[i];

				if( e.get_parent()->get_index()==nodeVec[selectedNode].get_index() ){
					edgeCount=edgeCount+1;
					edgeLength=e.get_length2D();
					sourceX=e.get_child()->get_x();
					sourceY=e.get_child()->get_y();

				}else if (e.get_child()->get_index()==nodeVec[selectedNode].get_index() && e.get_parent()->get_name()==string("MGAM2") ){
					edgeCount=1;
					//						edgeCount=edgeCount+1;
					edgeLength=e.get_length2D();
					sourceX=e.get_parent()->get_x();
					sourceY=e.get_parent()->get_y();
					break;
				}
			}
			if( edgeCount==1 ){
				if( (x-sourceX)>edgeLength ){
					x=sourceX+edgeLength;
				}
				if( (x-sourceX)<-edgeLength ){
					x=sourceX-edgeLength;
				}
				float sq=(edgeLength*edgeLength - (x-sourceX)*(x-sourceX));
				sq=sq<0?0:sq;
				float newY1=sqrt(sq)+sourceY;
				float newY2=-sqrt(sq)+sourceY;
				if( abs(y-newY1)<abs(y-newY2) ){
					y=newY1;
				}else{
					y=newY2;
				}
			}
			pathway.set_node_coordX( selectedNode, x );
			pathway.set_node_coordY( selectedNode, y );

		}else{
			pathway.set_node_coordX( selectedNode, x );
			pathway.set_node_coordY( selectedNode, y );
		}

		//move with all connected nodes
		vector<Edge>	edgeVec = pathway.get_edgeVec();
		if( dragConnected ){
			vector<size_t>	movedIndexVec;
			for( size_t i=0; i<edgeVec.size(); i++ ){
				Edge	e=edgeVec[i];
				if( e.get_parent()->get_index()==nodeVec[selectedNode].get_index() ){

					double x1=e.get_child()->get_x() + x-x0;
					double y1=e.get_child()->get_y() + y-y0;
					for( size_t j=0; j<nodeVec.size(); j++ ){
						if( nodeVec[j].get_index()==e.get_child()->get_index() && j!=selectedNode &&
								std::find(movedIndexVec.begin(), movedIndexVec.end(), j) == movedIndexVec.end() ){
							pathway.set_node_coordX( j, x1 );
							pathway.set_node_coordY( j, y1 );
							movedIndexVec.push_back(j);
							break;
						}
					}
				}else if( e.get_child()->get_index()==nodeVec[selectedNode].get_index() ){
					double x1=e.get_parent()->get_x() + x-x0;
					double y1=e.get_parent()->get_y() + y-y0;
					for( size_t j=0; j<nodeVec.size(); j++ ){
						if( nodeVec[j].get_index()==e.get_parent()->get_index() && j!=selectedNode &&
								std::find(movedIndexVec.begin(), movedIndexVec.end(), j) == movedIndexVec.end()   ){
							pathway.set_node_coordX( j, x1 );
							pathway.set_node_coordY( j, y1 );
							movedIndexVec.push_back(j);
							break;
						}
					}
				}
			}
		}
		update();
	}
}

void
PathwayViewer2::mousePressEvent(QMouseEvent *event){
	if( event->buttons() & Qt::RightButton ){
		mouseMove.setX(0);
		mouseMove.setY(0);
		mousePos = event->localPos();
	}
	//drag and move nodes
	if( event->buttons() & Qt::LeftButton ){
		vector<Node>	nodeVec = pathway.get_nodeVec();
		double			eventX = double(event->localPos().x());
		double			eventY = double(event->localPos().y());
		selectedNode=-1;

		for( size_t i=0; i<nodeVec.size(); i++ ){

			double x = ((nodeVec[i].get_x()-pathway.get_minX())*width()*0.8)/(pathway.get_maxX()-pathway.get_minX()) + width()*0.1;
			double y = ((nodeVec[i].get_y()-pathway.get_minY())*width()*0.8)/(pathway.get_maxY()-pathway.get_minY()) + width()*0.1;

			QPoint	p(x,y);
			QPoint	worldP=painterWorldTransf*p;

			double dist = sqrt( pow(eventX-worldP.x(),2)+pow(eventY-worldP.y(),2) );
			float 	r = 7;
			if(maxNodeVal!=0){
				r = abs(nodeVec[i].get_value())*10/maxNodeVal+7;
			}
			r=r*2.0;
			if( dist<r*scale*1.5 ){
				selectedNode=i;
			}
		}
	}
}

void PathwayViewer2::paintEvent(QPaintEvent * /* event */)
{

    float transX =  mouseSave.x() < width()/2 ? mouseSave.x():width()/2;
    float transY =  mouseSave.y() < width()/2 ? mouseSave.y():width()/2;
    transX =  transX < -width()*scale ? -width()*scale:transX;
    transY =  transY < -width()*scale ? -width()*scale:transY;

    mouseSave.setX(transX);
    mouseSave.setY(transY);

	drawEdge();
	drawNode();

	if(showLabel){
		drawLabel();
	}
	if( showEdgeLabel ){
		drawEdgeLabel();
	}
}

void
PathwayViewer2::set_pathway(Pathway& p, bool d_3){
	pathway.clear();
	pathway = p;

	//translate center
	range=pathway.get_range()*1.2;
	update();

	vector<Node>  nodeVec = pathway.get_nodeVec();
	minNodeVal = 5e5;
	maxNodeVal = -5e5;
	for( size_t i=0; i<nodeVec.size(); i++ ){
		float val = abs(nodeVec[i].get_value());
		minNodeVal = minNodeVal < val ? minNodeVal : val;
		maxNodeVal = maxNodeVal > val ? maxNodeVal : val;
	}
}

void
PathwayViewer2::saveImage( QString fileName ){

	QPixmap pixmap(this->size());

	this->render(&pixmap);

	pixmap.save(fileName);
	cout<<"save file:"<<endl;
}

void
PathwayViewer2::drawNode(){
    QPainter painter(this);
    painter.translate(mouseSave.x(),mouseSave.y());
    painter.scale(scale,scale);
    painter.setPen(pen);
    painter.setBrush(brush);
    painter.setRenderHint(QPainter::Antialiasing, true);

    painterWorldTransf=painter.worldTransform();

	vector<Node> 	nodeVec = pathway.get_nodeVec();

	for( size_t i=0; i<nodeVec.size(); i++ ){
		double x = ((nodeVec[i].get_x()-pathway.get_minX())*width()*0.8)/(pathway.get_maxX()-pathway.get_minX()) + width()*0.1;
		double y = ((nodeVec[i].get_y()-pathway.get_minY())*float(width())*0.8)/(pathway.get_maxY()-pathway.get_minY()) + width()*0.1;

		float val=1;
		if(nodeVec[i].get_pvalue()>=0){
			val = nodeVec[i].get_pvalue()<0.1 ? 0.1 : nodeVec[i].get_pvalue();
		}
		float	bright = abs( log2(val)*255/log2(0.1) )+50;
		bright = bright>255?255:bright;
		float 	r = 4;
		if(maxNodeVal!=0){
			r = abs(nodeVec[i].get_value())*4/maxNodeVal+7;
		}
		r=r*2;

		QRectF rectangle(x-r/2.0, y-r/2.0, r, r);

    	if( nodeVec[i].get_value()>0 ){
			painter.setPen(QPen(QColor(255,0,0,255), 0.5));
    	}else if(nodeVec[i].get_value()<0 ){
			painter.setPen(QPen(QColor(0,0,255,255), 0.5));
			painter.setBrush(Qt::blue);
    	}else{
    		painter.setBrush(Qt::white);
    	}

    	if( nodeVec[i].get_color()==string("orange") ){
    		painter.setBrush(QColor(255,153,0,255));
    	}else if( nodeVec[i].get_color()==string("green") ){
    		painter.setBrush(QColor(0,255,0,255));
    	}

    	if( nodeVec[i].get_type() == protein ){
    		painter.drawEllipse(rectangle);

    	}else{
    		painter.drawRect(rectangle);
    	}
	}
	painter.end();
}

void
PathwayViewer2::drawEdge(){
    QPainter painter(this);
    painter.translate(mouseSave.x(),mouseSave.y());
    painter.scale(scale,scale);

    painter.setBrush(brush);
    double arrowLength=6;
    double margin = 0;
    double 	r = 4;

	vector<Edge> 	edgeVec = pathway.get_edgeVec();
	float			maxEdgeVal=-10e10;
	for( size_t i=0; i<edgeVec.size(); i++ ){
		maxEdgeVal=maxEdgeVal>abs(edgeVec[i].get_value()) ? maxEdgeVal:abs(edgeVec[i].get_value()) ;
	}
	for(size_t i=0; i<edgeVec.size(); i++){
		double x1 = ((edgeVec[i].get_parent()->get_x()-pathway.get_minX())*width()*0.8)/(pathway.get_maxX()-pathway.get_minX()) + width()*0.1;
		double y1 = ((edgeVec[i].get_parent()->get_y()-pathway.get_minY())*width()*0.8)/(pathway.get_maxY()-pathway.get_minY()) + width()*0.1;
		double x2 = ((edgeVec[i].get_child()->get_x()-pathway.get_minX())*width()*0.8)/(pathway.get_maxX()-pathway.get_minX()) + width()*0.1;
		double y2 = ((edgeVec[i].get_child()->get_y()-pathway.get_minY())*width()*0.8)/(pathway.get_maxY()-pathway.get_minY()) + width()*0.1;


		if( maxNodeVal !=0 ){
			r = abs( edgeVec[i].get_child()->get_value())*4/maxNodeVal+7;
		}

		double at=atan((y2-y1)/(x2-x1));

		double xt=(r+margin)*cos(at);
		double yt=(r+margin)*sin(at);
		QVector2D v( x1, y1 );
		QVector2D v1( x2- xt, y2- yt );
		QVector2D v2( x2+ xt, y2+ yt );
		if(v.distanceToPoint(v1)<v.distanceToPoint(v2)){
			x2=x2- xt;
			y2=y2- yt;
		}else{
			x2=x2+ xt;
			y2=y2+ yt;
		}
		QPointF p1(x1,y1);
		QPointF p2(x2,y2);

		painter.setRenderHint(QPainter::Antialiasing, true);
		float transp=(maxEdgeVal - abs(edgeVec[i].get_value()) )*255*0.1;
		transp=250;

		float lineWidth=1;
		if( edgeVec[i].get_value() > 0 ){
			painter.setPen(QPen(QColor(0,0,0,transp), lineWidth));
			if( !isnan(x1) && !isnan(y1) && !isnan(x2) && !isnan(y2) ){
//				painter.drawLine(x1,y1,x2,y2);
				painter.drawLine(p1,p2);
			}
		}else if(edgeVec[i].get_value() < 0){
			painter.setPen(QPen(QColor(0,0,255, transp), lineWidth));
			if( !isnan(x1) && !isnan(y1) && !isnan(x2) && !isnan(y2) ){
//				painter.drawLine(x1,y1,x2,y2);
				painter.drawLine(p1,p2);
			}
		}

		if( edgeVec[i].get_type()==promote ){
			//fill arrow triangle
			double theta=25*3.14159/180.0;
//			double maxLength=v.distanceToPoint(QVector2D(x2,y2))/2.0;
//			arrowLength=arrowLength>maxLength ? maxLength : arrowLength;
			double xa=0;
			double ya=0;
			double xb=0;
			double yb=0;

			double	xa1=x2-arrowLength*cos(at-theta);
			double	ya1=y2-arrowLength*sin(at-theta);
			double	xb1=x2-arrowLength*cos(at+theta);
			double	yb1=y2-arrowLength*sin(at+theta);

			double	xa2=x2+arrowLength*cos(at-theta);
			double	ya2=y2+arrowLength*sin(at-theta);
			double	xb2=x2+arrowLength*cos(at+theta);
			double	yb2=y2+arrowLength*sin(at+theta);

			if( v.distanceToPoint( QVector2D(xa1,ya1) ) < v.distanceToPoint( QVector2D(xa2,ya2) ) ){
				xa=xa1;
				ya=ya1;
			}else{
				xa=xa2;
				ya=ya2;
			}
			if( v.distanceToPoint( QVector2D(xb1,yb1) ) < v.distanceToPoint( QVector2D(xb2,yb2) ) ){
				xb=xb1;
				yb=yb1;
			}else{
				xb=xb2;
				yb=yb2;
			}

			QPainterPath path;
			path.moveTo(xa,ya);
			path.lineTo(xb,yb);
			path.lineTo(x2,y2);
			path.lineTo(xa,ya);

//			painter.fillPath(path,QBrush(QColor(0,0,0,255)));
			painter.fillPath(path,QBrush(QColor(0,0,0,255)));
		}else if( edgeVec[i].get_type()==inhibit ){
			//fill inhibit arrow vertical
			double theta=90*3.14159/180.0;
			double arrowLength=r;
			double xa=0;
			double ya=0;
			double xb=0;
			double yb=0;

			double	xa1=x2-arrowLength*cos(at-theta);
			double	ya1=y2-arrowLength*sin(at-theta);
			double	xb1=x2-arrowLength*cos(at+theta);
			double	yb1=y2-arrowLength*sin(at+theta);

			double	xa2=x2+arrowLength*cos(at-theta);
			double	ya2=y2+arrowLength*sin(at-theta);
			double	xb2=x2+arrowLength*cos(at+theta);
			double	yb2=y2+arrowLength*sin(at+theta);
			if( v.distanceToPoint( QVector2D(xa1,ya1) ) < v.distanceToPoint( QVector2D(xa2,ya2) ) ){
				xa=xa1;
				ya=ya1;
			}else{
				xa=xa2;
				ya=ya2;
			}
			if( v.distanceToPoint( QVector2D(xb1,yb1) ) < v.distanceToPoint( QVector2D(xb2,yb2) ) ){
				xb=xb1;
				yb=yb1;
			}else{
				xb=xb2;
				yb=yb2;
			}
			QPointF pa(xa, ya);
			QPointF pb(xb, yb);
			if( !isnan(xa) && !isnan(ya) && !isnan(xb) && !isnan(yb) ){
				painter.drawLine(pa, pb);
			}
		}
	}
	painter.end();
}

void
PathwayViewer2::drawLabel(){
    QPainter painter(this);
    painter.translate(mouseSave.x(),mouseSave.y());
    painter.setPen(QPen(Qt::black));
    painter.setBrush(brush);
    painter.setRenderHint(QPainter::Antialiasing, true);

	vector<Node> 	nodeVec = pathway.get_nodeVec();
	float			fontSize = 10 ;

	for( size_t i=0; i<nodeVec.size(); i++ ){

		double x = (((nodeVec[i].get_x()-pathway.get_minX())*width()*0.8)/(pathway.get_maxX()-pathway.get_minX()) + width()*0.1)*scale;
		double y = (((nodeVec[i].get_y()-pathway.get_minY())*width()*0.8)/(pathway.get_maxY()-pathway.get_minY()) + width()*0.1)*scale;

		float	ab = abs(nodeVec[i].get_value());
		float   r = 4;
		if( (maxNodeVal-minNodeVal) != 0 ){
			r = ab/(maxNodeVal - minNodeVal)*4+5;
		}

	    QFont font( "Newyork", fontSize );
	    font.setStyleHint( QFont::SansSerif );
		painter.setFont(font);
		painter.setPen(QPen(Qt::black));
		//change highligted label
		if( nodeVec[i].get_name().find(highLightName) != std::string::npos ){
			painter.setPen(QPen(Qt::green));
		}

		string	label = nodeVec[i].get_name();

		painter.drawText(x-(float(label.length())/2.0)*r*scale,y+r*scale, QString::fromStdString(label) );

	}
	painter.end();
}


void
PathwayViewer2::drawEdgeLabel(){
    QPainter painter(this);
    painter.translate(mouseSave.x(),mouseSave.y());
    painter.setPen(QPen(Qt::black));
    painter.setBrush(brush);
    painter.setRenderHint(QPainter::Antialiasing, true);

	vector<Node> 	nodeVec = pathway.get_nodeVec();
	float			fontSize = 15 ;

	//draw edge value
	vector<Edge>	edgeVec = pathway.get_edgeVec();
	for( size_t i=0; i<edgeVec.size(); i++ ){
		Node*	nodeP=edgeVec[i].get_parent();
		Node*	nodeC=edgeVec[i].get_child();

		double x1=(((nodeP->get_x()-pathway.get_minX())*width()*0.8)/(pathway.get_maxX()-pathway.get_minX()) + width()*0.1)*scale;
		double y1=(((nodeP->get_y()-pathway.get_minY())*width()*0.8)/(pathway.get_maxY()-pathway.get_minY()) + width()*0.1)*scale;

		double x2=(((nodeC->get_x()-pathway.get_minX())*width()*0.8)/(pathway.get_maxX()-pathway.get_minX()) + width()*0.1)*scale;
		double y2=(((nodeC->get_y()-pathway.get_minY())*width()*0.8)/(pathway.get_maxY()-pathway.get_minY()) + width()*0.1)*scale;

		double x=(x1+x2)/2.0;
		double y=(y1+y2)/2.0;

		painter.setFont(QFont("times", fontSize));
		float	label=edgeVec[i].get_value();
		painter.drawText(x,y, QString::number(label) );
	}
	painter.end();
}





