
#include<QGridLayout>
#include<QSplitter>
#include<QGLFunctions>
#include<QDir>
#include<QPixmap>
#include"KGML.h"
#include "mainwindow.h"
#include"filedownloader.h"

MainWindow::MainWindow(int argc, char *argv[])
{
	GLuint *axisVBO;
	setWindowTitle(tr("pathway"));
	resize(1000, 800);
	QWidget *centralWidget = new QWidget;

	pathViewer = new PathwayViewer();
	pathViewer2 = new PathwayViewer2(this);
	QGridLayout *layout = new QGridLayout;
	layout->addWidget( pathViewer2, 0, 0);

	heatmap = new HeatmapViewer(this);

	centralWidget->setLayout(layout);
	setCentralWidget(centralWidget);

	createActions();
	createMenus();
	createToolBars();

	QDesktopWidget *desktop = QApplication::desktop();
	move( (desktop->width() - this->width())/2, (desktop->height()-this->height())/2 );

	keggDlg = new HttpWindow;

	sideTabWidget = new QTabWidget();
	sideTextEdit = new QTextEdit;
	sideTextEdit->setReadOnly(true);
	sideTextEdit->setWordWrapMode(QTextOption::NoWrap);
	sideTabWidget->addTab(sideTextEdit, QString("File"));

	testTableWidget = new QTableWidget(this);
	sideTabWidget->addTab( testTableWidget, QString("t-test") );

	buttomTabWidget = new QTabWidget();
	buttomTextEdit = new QTextEdit;
	buttomTextEdit->setReadOnly(true);
	buttomTabWidget->addTab( buttomTextEdit, QString("Text") );

	QTextEdit *termEdit = new QTextEdit;
	termEdit->setReadOnly(true);
	QTreeView	*pathwayDataViewer=new QTreeView();

	QSplitter		*mainSplitter = new QSplitter( Qt::Vertical );
	QSplitter       *upSplitter =  new QSplitter( Qt::Horizontal );

	upSplitter->addWidget( sideTabWidget );
	upSplitter->addWidget( pathViewer2 );
	upSplitter->addWidget( heatmap );
	upSplitter->setStretchFactor(0, 15);
	upSplitter->setStretchFactor(1, 20);
	upSplitter->setStretchFactor(2, 10);
	mainSplitter->addWidget(upSplitter);
	mainSplitter->setHandleWidth(0);
	mainSplitter->addWidget(buttomTabWidget);
	mainSplitter->setStretchFactor(0, 20);
	mainSplitter->setStretchFactor(1, 8);
	setCentralWidget( mainSplitter );

	if( 0 ){
		pathway.clear();
		pathway.readPathwayFile("test_data/test_network.txt");
		pathViewer2->set_pathway(pathway, false);
	}
}

MainWindow::~MainWindow()
{
}

void
MainWindow::updateWindow(){

}

void
MainWindow::createMenus(){
    fileMenu =  menuBar()->addMenu( tr("&File") );
    fileMenu->addAction(openKeggPathwayAction);
    fileMenu->addAction(downloadKeggAction);
    fileMenu->addAction(openExpressionAction);
    fileMenu->addAction(savePathwayAction);
    fileMenu->addAction(saveImageAction);

    viewMenu = menuBar()->addMenu( tr("&View") );
    viewMenu->addAction( viewAxisAction );
    viewMenu->addAction( viewLabelAction );
    viewMenu->addAction( viewEdgeLabelAction );
    viewMenu->addAction( view3DAction );

    toolsMenu =  menuBar()->addMenu( tr("&Tools") );
    toolsMenu->addAction(annotationAction);
    toolsMenu->addAction(geneHeatmapAction);

    aboutMenu = menuBar()->addMenu(tr("&About"));
    aboutMenu->addAction(aboutAction);
}

void
MainWindow::createToolBars(){
	editToolBar=addToolBar(tr("search"));
	editToolBar->addWidget(searchEdit);
	editToolBar->addAction(searchAct);
	editToolBar->addAction(clearAct);
	editToolBar->addSeparator();
	editToolBar->addAction(dragConnectedNodeAction);
	editToolBar->addAction(dragFixedEdgeLengthAction);
}

void
MainWindow::createActions(){

	openKeggPathwayAction=new QAction(tr("&Open kegg Pathway"),this);
	connect(openKeggPathwayAction, SIGNAL(triggered(bool)), this, SLOT(openKeggPathwayFile()));

	openExpressionAction=new QAction(tr("&Open Expression"), this);
	connect(openExpressionAction, SIGNAL(triggered(bool)),this, SLOT(openExpressionFile()));

	downloadKeggAction=new QAction(tr("&Download KEGG pathway"), this);
	connect( downloadKeggAction, SIGNAL(triggered(bool)),this, SLOT(downloadKeggPathway()));

	savePathwayAction=new QAction(tr("&Save Pathway"),this);
	connect( savePathwayAction, SIGNAL(triggered(bool)),this, SLOT(savePathway()));

	saveImageAction = new QAction(tr("&Save Image"), this);
	connect( saveImageAction, SIGNAL(triggered(bool)),this, SLOT(saveImage()));

    viewAxisAction=new QAction(tr("&Axis"),this);
    viewAxisAction->setStatusTip(tr("Show Axis"));
    viewAxisAction->setCheckable(true);
    connect(viewAxisAction, SIGNAL(triggered(bool)), this, SLOT(viewAxis()));

    viewLabelAction=new QAction(tr("&Label Node"),this);
    viewLabelAction->setStatusTip(tr("Show Node Label"));
    viewLabelAction->setCheckable(true);
    connect(viewLabelAction, SIGNAL(triggered(bool)), this, SLOT(viewLabel()));

    viewEdgeLabelAction=new QAction(tr("&Label Edge"),this);
    viewEdgeLabelAction->setStatusTip(tr("Show Edge Label"));
    viewEdgeLabelAction->setCheckable(true);
    connect(viewEdgeLabelAction, SIGNAL(triggered(bool)), this, SLOT(viewEdgeLabel()));

//    dragConnectedNodeAction=new QAction(tr("&Drag Coonected Node"),this);
    dragConnectedNodeAction=new QAction(QIcon(QPixmap("icon/dragConnectedNode.png")),"??",this);
    dragConnectedNodeAction->setCheckable(true);
    connect( dragConnectedNodeAction, SIGNAL(triggered(bool)), this, SLOT(dragConnectedNode()));

    dragFixedEdgeLengthAction = new QAction(QIcon(QPixmap("icon/dragFixedEdge.png")),"??",this);
    dragFixedEdgeLengthAction->setCheckable(true);
    connect( dragFixedEdgeLengthAction, SIGNAL(triggered(bool)), this, SLOT(dragFixedEdgeLength()));

    view3DAction = new QAction(tr("&view 3D"),this);
    view3DAction->setStatusTip(tr("View 3D pathway"));
    view3DAction->setCheckable(true);
    connect(view3DAction, SIGNAL(triggered(bool)), this, SLOT(view3D()));

    searchAct=new QAction(tr("search"),this);
    connect(searchAct, SIGNAL(triggered(bool)), this, SLOT(searchGene()));

    clearAct=new QAction(tr("clear"),this);
    connect(clearAct, SIGNAL(triggered(bool)), this, SLOT(clearSearchGene()));
    searchEdit = new QLineEdit();
    searchEdit->setPlaceholderText("gene name");
    searchEdit->setFixedWidth(100);
    searchEdit->setFocus();

    aboutAction = new QAction(tr("About"),this);
    connect(aboutAction, SIGNAL(triggered(bool)),this,SLOT(about()));

    annotationAction =new QAction(tr("&Annotation"),this);
    connect(annotationAction, SIGNAL(triggered(bool)), this, SLOT(geneAnnotation()));

    annotateDlg = new AnnotationDialog;
    connect( annotateDlg, SIGNAL(finishRun()), this, SLOT(updateTable()) );

    geneHeatmapAction = new QAction(tr("&Draw Heatmap"), this);
    connect( geneHeatmapAction, SIGNAL(triggered(bool)), this, SLOT(drawHeatmap()) );

    heatmapDlg = new HeatmapDialog;
    connect( heatmapDlg, SIGNAL( finish() ), this, SLOT(updateHeatmap()) );
}

void
MainWindow::openKeggPathwayFile(){
//	openKeggPathwayFile_test();

	QString pathwayFileName=QFileDialog::getOpenFileName(this, tr("Open pathway file"), ".",tr("*"));
	if(!pathwayFileName.toStdString().empty()){
		cout<<pathwayFileName.toStdString()<<endl;
		keggPathway = readKeggXMLfile(pathwayFileName.toStdString());
		if( keggPathway.org=="ko" ){
			keggPathway.map_KOgeneID_to_Name("data/keggPathway/KO_ID_Name_HSA_Description.txt");
		}
		pathway.clear();
		bool is3D=false;

		pathway.get_keggPathway( keggPathway, is3D );

		pathViewer2->set_pathway(pathway, is3D);

		string pathwayName= keggPathway.name+" "+ keggPathway.title.c_str();
		sideTextEdit->append(QString( pathwayName.c_str() ) );
		sideTabWidget->setCurrentWidget( sideTextEdit );
	}
}


//test all ko files
void
MainWindow::openKeggPathwayFile_test(){
	QDir myDir("./data/keggPathway/ko_data/");
	QStringList files = myDir.entryList();
	QString pathwayFileName;
	for(size_t i=0; i<files.size();i++){
		pathwayFileName = QString("./data/keggPathway/ko_data/")+files.at(i);
		cout<<pathwayFileName.toStdString()<<endl;

		if(!pathwayFileName.toStdString().empty()){
			cout<<pathwayFileName.toStdString()<<endl;
			keggPathway = readKeggXMLfile(pathwayFileName.toStdString());

			if( keggPathway.org=="ko" ){
				keggPathway.map_KOgeneID_to_Name("data/keggPathway/KO_ID_Name_Description.txt");
			}
			pathway.clear();
			bool is3D=false;

			pathway.get_keggPathway( keggPathway, is3D );

			pathViewer2->set_pathway(pathway, is3D);

			string pathwayName= keggPathway.name+" "+ keggPathway.title.c_str();
			sideTextEdit->append(QString( pathwayName.c_str() ) );
		}
		QTime dieTime= QTime::currentTime().addSecs(0.2);
		while (QTime::currentTime() < dieTime)
			QCoreApplication::processEvents(QEventLoop::AllEvents, 100);
	}
}

void
MainWindow::openExpressionFile(){
	QString expressionFileName=QFileDialog::getOpenFileName(this, tr("Open expression file"), ".",tr("*"));
	cout<<expressionFileName.toStdString()<<endl;
	pathway.readGeneValue(expressionFileName.toStdString().c_str());
    pathViewer2->set_pathway(pathway);
}

void
MainWindow::savePathway(){
	QString	pathwayPath=QFileDialog::getSaveFileName( this,tr("Save File"),"" );
	pathViewer2->get_pathway().printToFile(pathwayPath.toStdString());

}

void
MainWindow::saveImage(){
	QString	imagePath=QFileDialog::getSaveFileName( this,tr("Save File"),"",tr("PNG (*.png)" ) );
	pathViewer2->saveImage(imagePath);
}

void
MainWindow::downloadKeggPathway(){
	connect( keggDlg, SIGNAL(accepted()), this, SLOT(readKeggPathway()) );
	keggDlg->show();
}

void
MainWindow::readKeggPathway(){
	cout<<keggDlg->get_fileName().toStdString()<<endl;

	keggPathway = readKeggXMLfile(keggDlg->get_fileName().toUtf8().constData());

	if( keggPathway.org==string("ko") ){
		keggPathway.map_KOgeneID_to_Name("data/keggPathway/KO_ID_Name_HSA_Description.txt");
	}
	if(!keggPathway.empty){
		pathway.get_keggPathway( keggPathway );
		pathViewer2->set_pathway(pathway);
	}
}

void
MainWindow::geneAnnotation(){
	annotateDlg->open();
	cout<<"Annotate!!!"<<endl;
}

void
MainWindow::updateTable(){

	pathViewer2->clear();
	int row = annotateDlg->getAnnotation()->getAnnotationResult().size();

	QTableWidget *tableWidget = new QTableWidget(row,7, this);
	tableWidget->horizontalHeader()->setStretchLastSection(true);

	QTableWidgetItem	*pathwayItem = new QTableWidgetItem(QString("Pathway"));
	QTableWidgetItem	*pathwayIDitem = new QTableWidgetItem(QString("PathwayID"));
	QTableWidgetItem	*pathwayNumItem = new QTableWidgetItem(QString("Pathway gene Num"));
	QTableWidgetItem	*annotateItem = new QTableWidgetItem(QString("Annotated Genes Num"));
	QTableWidgetItem	*esItem = new QTableWidgetItem(QString("Enrichment Score"));
	QTableWidgetItem	*pItem = new QTableWidgetItem(QString("P value"));
	QTableWidgetItem	*qItem = new QTableWidgetItem(QString("Q value"));

	pathwayItem->setFlags( Qt::ItemIsEnabled );
	pathwayIDitem->setFlags( Qt::ItemIsEnabled );
	pathwayNumItem->setFlags( Qt::ItemIsEnabled );
	annotateItem->setFlags( Qt::ItemIsEnabled );
	esItem->setFlags( Qt::ItemIsEnabled );
	pItem->setFlags( Qt::ItemIsEnabled );
	qItem->setFlags( Qt::ItemIsEnabled );

	tableWidget->setItem( 0, 0, pathwayItem );
	tableWidget->setItem( 0, 1, pathwayIDitem );
	tableWidget->setItem( 0, 2, pathwayNumItem );
	tableWidget->setItem( 0, 3, annotateItem );
	tableWidget->setItem( 0, 4, esItem );
	tableWidget->setItem( 0, 5, pItem );
	tableWidget->setItem( 0, 6, qItem );

	annotatedPathwayVec.clear();
	for( size_t i=0; i<row; i++ ){
		string	pathway 			= annotateDlg->getAnnotation()->getAnnotationResult()[i].pathway;
		string 	pathwayID			= "";
		int		pathwayGeneNum 		= annotateDlg->getAnnotation()->getAnnotationResult()[i].pathwayGeneNum;
		int		annotatedGeneNum 	= annotateDlg->getAnnotation()->getAnnotationResult()[i].annotatedGeneNum;
		float	es 					= annotateDlg->getAnnotation()->getAnnotationResult()[i].es;
		float	p_value 			= annotateDlg->getAnnotation()->getAnnotationResult()[i].p_value;
		float	q_value 			= annotateDlg->getAnnotation()->getAnnotationResult()[i].q_value;

		annotatedPathwayVec.push_back( pathway );


		ifstream	iif;
		iif.open("data/MSigDB-KEGG_ko");
		string line;
		while( getline(iif, line) ){
			vector<string>	strVec = tokenBySpace(line);
			if( strVec[0] == pathway ){
				pathwayID = strVec[1];
			}
		}

		pathwayItem =   new QTableWidgetItem(QString::fromStdString(pathway));
		pathwayItem->setSizeHint(QSize(30,10));
		pathwayIDitem = new QTableWidgetItem(QString::fromStdString(pathwayID));
		pathwayNumItem = new QTableWidgetItem(QString::number(pathwayGeneNum));
		annotateItem = new QTableWidgetItem(QString::number(annotatedGeneNum));
		esItem = new QTableWidgetItem(QString::number(es));
		pItem = new QTableWidgetItem(QString::number(p_value));
		qItem = new QTableWidgetItem(QString::number(q_value));

		pathwayItem->setFlags( Qt::ItemIsEnabled );
		pathwayIDitem->setFlags( Qt::ItemIsEnabled );
		pathwayNumItem->setFlags( Qt::ItemIsEnabled );
		annotateItem->setFlags( Qt::ItemIsEnabled );
		esItem->setFlags( Qt::ItemIsEnabled );
		pItem->setFlags( Qt::ItemIsEnabled );
		qItem->setFlags( Qt::ItemIsEnabled );

		tableWidget->setItem( i+1, 0, pathwayItem );
		tableWidget->setItem( i+1, 1, pathwayIDitem );
		tableWidget->setItem( i+1, 2, pathwayNumItem );
		tableWidget->setItem( i+1, 3, annotateItem );
		tableWidget->setItem( i+1, 4, esItem );
		tableWidget->setItem( i+1, 5, pItem );
		tableWidget->setItem( i+1, 6, qItem );
	}
	connect( tableWidget, SIGNAL(cellDoubleClicked(int, int)), this, SLOT(updatePathway(int, int)));
	connect( tableWidget, SIGNAL(cellDoubleClicked(int, int)), this, SLOT(updateSideTable(int, int)));
	buttomTabWidget->addTab(tableWidget, QString("table1"));
	buttomTabWidget->setCurrentWidget(tableWidget);
}

void
MainWindow::updatePathway(int r, int c){
	if(r==0){
		return;
	}

	string pathwayName = annotatedPathwayVec[r-1];
	string pathwayType = tokenByDash(pathwayName)[0];
	ifstream	iif;
	iif.open("data/MSigDB-KEGG_ko");
	string line;

	vector<testResult>	testVec = annotateDlg->getAnnotation()->getAnnotationResult()[r-1].testVec;

	while( getline(iif, line) ){
		vector<string>	strVec = tokenBySpace(line);
		if( strVec[0] == pathwayName ){
			string	p("data/keggPathway/ko_data/");
			KGML_pathway	keggPathway = readKeggXMLfile( p+strVec[1], false );
			if(keggPathway.entryVec.empty()){
				cout<<"!!!!"<<endl;
				break;
			}

			keggPathway.map_KOgeneID_to_Name("data/keggPathway/KO_ID_Name_HSA_Description.txt");

			pathway.get_keggPathway( keggPathway, testVec );

			pathViewer2->set_pathway(pathway);

		}
	}
}

void
MainWindow::drawHeatmap(){
	heatmapDlg->open();
}

/*
 * gene test result
 */
void
MainWindow::updateSideTable(int r, int c){
	if(r==0){
		return;
	}
	vector<testResult>	testVec =  annotateDlg->getAnnotation()->getAnnotationResult()[r-1].testVec;
	size_t row	= testVec.size();

	testTableWidget->setColumnCount(3);
	testTableWidget->setRowCount(row);

	testTableWidget->horizontalHeader()->setStretchLastSection(true);

	QTableWidgetItem			*nIt = new QTableWidgetItem( QString("gene") );
	QTableWidgetItem			*vIt = new QTableWidgetItem( QString("t-test value") );
	QTableWidgetItem			*pIt = new QTableWidgetItem( QString("t-test p-value") );

	nIt->setFlags( Qt::ItemIsEnabled );
	vIt->setFlags( Qt::ItemIsEnabled );
	pIt->setFlags( Qt::ItemIsEnabled );

	testTableWidget->setItem( 0, 0, nIt );
	testTableWidget->setItem( 0, 1, vIt );
	testTableWidget->setItem( 0, 2, pIt );

	for( size_t i=0; i<testVec.size(); i++ ){
		QTableWidgetItem	*pathwayNameIt = new QTableWidgetItem( QString(testVec[i].geneName.c_str()) );
		QTableWidgetItem	*valueIt = new QTableWidgetItem( QString::number(testVec[i].value) );
		QTableWidgetItem	*pvalueIt = new QTableWidgetItem( QString::number(testVec[i].pValue) );

		pathwayNameIt->setFlags( Qt::ItemIsEnabled );
		valueIt->setFlags( Qt::ItemIsEnabled );
		pvalueIt->setFlags( Qt::ItemIsEnabled );

		testTableWidget->setItem( i+1, 0, pathwayNameIt );
		testTableWidget->setItem( i+1, 1, valueIt );
		testTableWidget->setItem( i+1, 2, pvalueIt );
	}

	sideTabWidget->setCurrentWidget( testTableWidget );

	vector< string > 		geneListVec = annotateDlg->getAnnotation()->getAnnotationResult()[r-1].geneListVec;
	vector< vector<float> > dataM = annotateDlg->getAnnotation()->getDataMatrix( geneListVec );
	vector< string >		geneVec = annotateDlg->getAnnotation()->getAnnotatedGenes( geneListVec );
	vector< string >		sampleNameVec = annotateDlg->getAnnotation()->getSampleNameVec();
	vector< string >		classIndexVec = annotateDlg->getAnnotation()->getSampleClassVec();

	cout<<"data matrix:"<<dataM.size()<<"\t"<<dataM[0].size()<<endl;
	cout<<"geneList:"<<geneListVec.size()<<endl;
	cout<<"sample:"<<sampleNameVec.size()<<endl;
	cout<<"classIndex"<<classIndexVec.size()<<endl;
	heatmap->setData( dataM, geneVec, sampleNameVec, classIndexVec, true );
	heatmap->update();
}

void
MainWindow::updateHeatmap(){
	vector<string>  		geneVec = heatmapDlg->getGeneVec();
	if( geneVec.empty() ){
		return;
	}

	vector< vector<float> > dataM = heatmapDlg->getAnnotation()->getDataMatrix(geneVec);
	vector<string>			matrixGeneList =heatmapDlg->getAnnotation()->getAnnotatedGenes(geneVec);
	vector< string >		sampleNameVec = heatmapDlg->getAnnotation()->getSampleNameVec();
	vector< string >		classIndexVec = heatmapDlg->getAnnotation()->getSampleClassVec();

	heatmap->setData( dataM, matrixGeneList, sampleNameVec, classIndexVec, true );
	heatmap->update();
	heatmapDlg->close();
}

void
MainWindow::viewAxis(){
    if(!viewAxisAction->isChecked()){
        viewAxisAction->setChecked(false);
        pathViewer2->updateAxis(false);
        pathViewer2->update();
    }else{
        viewAxisAction->setChecked(true);
        pathViewer2->updateAxis(true);
        pathViewer2->update();
    }
}

void
MainWindow::viewLabel(){
    if(!viewLabelAction->isChecked()){
        viewLabelAction->setChecked(false);
        pathViewer2->updateLabel(false);
        pathViewer2->update();
    }else{
        viewLabelAction->setChecked(true);
        pathViewer2->updateLabel(true);
        pathViewer2->update();
    }
}

void
MainWindow::viewEdgeLabel(){
    if(!viewEdgeLabelAction->isChecked()){
        viewEdgeLabelAction->setChecked(false);
        pathViewer2->updateEdgeLabel(false);
        pathViewer2->update();
    }else{
        viewEdgeLabelAction->setChecked(true);
        pathViewer2->updateEdgeLabel(true);
        pathViewer2->update();
    }
}

void
MainWindow::dragConnectedNode(){
	if(!dragConnectedNodeAction->isChecked()){
		dragConnectedNodeAction->setChecked(false);
		pathViewer2->updateDragConnected(false);
		pathViewer2->update();
	}else{
		dragConnectedNodeAction->setChecked(true);
		pathViewer2->updateDragConnected(true);
		pathViewer2->update();
	}
}

void
MainWindow::dragFixedEdgeLength(){
	if(!dragFixedEdgeLengthAction->isChecked()){
		dragFixedEdgeLengthAction->setChecked(false);
		pathViewer2->updateDragFixedEdge(false);
		pathViewer2->update();
	}else{
		dragFixedEdgeLengthAction->setChecked(true);
		pathViewer2->updateDragFixedEdge(true);
		pathViewer2->update();
	}
}

void
MainWindow::view3D(){

	if( !view3DAction->isChecked() ){
		cout<<"view2D"<<endl;
		view3DAction->setChecked(false);
		pathway.clear();
		bool is3D=false;
		pathway.get_keggPathway( keggPathway, is3D );
		pathViewer2->set_pathway(pathway, is3D);

		pathViewer2->update();
	}else if( view3DAction->isChecked() ){
		cout<<"view3D"<<endl;
		view3DAction->setChecked(true);
		pathViewer2->clear();
		pathway.clear();
		bool is3D=true;
		pathway.get_keggPathway( keggPathway, is3D );
		pathViewer2->set_pathway(pathway, is3D);
		pathViewer2->update();
	}
}

void
MainWindow::searchGene(){
	cout<<"!!!"<<endl;
	QString text=searchEdit->text();
	string	s=text.toUtf8().constData();
	pathViewer2->highlightNode(s);
    pathViewer2->update();
}

void
MainWindow::clearSearchGene(){
	pathViewer2->clearHighlightNode();
	pathViewer2->update();
}

void
MainWindow::about(){
	QDialog* about=new QDialog;
	about->open();
}
