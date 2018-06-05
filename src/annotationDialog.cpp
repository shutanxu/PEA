/*
 * annotationDialog.cpp
 *
 *  Created on: Sep 19, 2016
 *      Author: stan
 */

#include"annotationDialog.h"

using namespace std;

AnnotationDialog::AnnotationDialog(){
	QLabel	*rnaSeqDataLabel = new QLabel(tr("RNA-seq Data:"));
	rnaSeqDataEdit = new QLineEdit;
	QPushButton *loadDataButton = new QPushButton(tr("Load"));
	connect(loadDataButton, SIGNAL(clicked()), this, SLOT(loadFile()));

	QHBoxLayout *dataLayout = new QHBoxLayout;
	dataLayout->addWidget(rnaSeqDataLabel);
	dataLayout->addWidget(rnaSeqDataEdit);
	dataLayout->addWidget(loadDataButton);

	QLabel *pathwayLabel = new QLabel(tr("Pathway"));
	pathwayComb = new QComboBox;

	pathwayComb->addItem(tr("KEGG"));
	pathwayComb->addItem(tr("Reactome"));
	pathwayComb->addItem(tr("chemical and genetic perturbations"));
	pathwayComb->addItem(tr("Canonical pathways"));
	pathwayComb->addItem(tr("BioCarta"));
	pathwayComb->addItem(tr("All"));

	QPushButton*	runButton = new QPushButton(tr("Run"));
	connect( runButton, SIGNAL(clicked()), this, SLOT(run()) );

	QHBoxLayout *pathwayLayout=new QHBoxLayout;
	pathwayLayout->addWidget(pathwayLabel);
	pathwayLayout->addWidget(pathwayComb);

	QHBoxLayout *runLayout=new QHBoxLayout;
	runLayout->addWidget( runButton );

	dataTable = new QTableWidget(this);
	dataTable->horizontalHeader()->setStretchLastSection(true);

	QVBoxLayout *mainLayout= new QVBoxLayout;
	mainLayout->addLayout(dataLayout);
	mainLayout->addWidget(dataTable);
	mainLayout->addLayout(pathwayLayout);
	mainLayout->addLayout(runLayout);
	setLayout(mainLayout);

	annotation=new Annotation();
}

void
AnnotationDialog::loadFile(){
	rnaFileName = QFileDialog::getOpenFileName(this,
			tr("Open RNA-seq Data File"), ".",
			tr("*"));
	rnaSeqDataEdit->insert( rnaFileName );

	//show data in annotation dialog table
	fileName = rnaFileName.toStdString();
	annotation->readFile( fileName );
	vector<string>			geneNameVec = annotation->getGeneNameVec();
	vector<string>			sampleNameVec = annotation->getSampleNameVec();
	vector<string>			sampleClassVec = annotation->getSampleClassVec();
	vector< vector<float> >	dataMatrix = annotation->getDataMatrix();
	size_t rowNum = dataMatrix.size()+2;
	size_t colNum = dataMatrix[0].size()+1;

	dataTable->setRowCount( rowNum );
	dataTable->setColumnCount( colNum );
	QTableWidgetItem *it;
	it = new QTableWidgetItem( QString("Class") );
	it->setFlags( Qt::ItemIsEnabled );
	dataTable->setItem( 0,0,it );
	it = new QTableWidgetItem( QString("Sample") );
	it->setFlags( Qt::ItemIsEnabled );
	dataTable->setItem( 1,0,it );

	for( size_t i=0; i<sampleClassVec.size(); i++ ){
		it = new QTableWidgetItem( QString::fromStdString(sampleClassVec[i]) );
		it->setFlags( Qt::ItemIsEnabled );
		dataTable->setItem( 0,i+1,it );
	}

	for( size_t i=0; i<sampleNameVec.size(); i++ ){
		it = new QTableWidgetItem( QString::fromStdString(sampleNameVec[i]) );
		it->setFlags( Qt::ItemIsEnabled );
		dataTable->setItem( 1,i+1,it );
	}

	size_t matrixRow=dataMatrix.size();
	size_t matrixCol=dataMatrix[0].size();
	for( size_t i=0; i<matrixRow; i++ ){

		it = new QTableWidgetItem( QString::fromStdString(geneNameVec[i]) );
		dataTable->setItem( i+2,0,it );
		for( size_t j=0; j<matrixCol; j++ ){
			it = new QTableWidgetItem( QString::number(dataMatrix[i][j]) );
			dataTable->setItem( i+2,j+1,it );
		}
	}
}

void
AnnotationDialog::run(){
	if (!rnaFileName.isEmpty()){
		cout<<rnaFileName.toStdString()<<endl;

		string	geneListFileName;
		if(pathwayComb->currentText() == QString("All")){
			geneListFileName = string("data/MSigDB/c2_curated_gene_sets/c2.all.v6.0.symbols.gmt");
		}else if( pathwayComb->currentText() == QString("BioCarta") ){
			geneListFileName = string("data/MSigDB/c2_curated_gene_sets/c2.cp.biocarta.v6.0.symbols.gmt");
		}else if( pathwayComb->currentText() == QString("KEGG") ){
			geneListFileName = string("data/MSigDB/c2_curated_gene_sets/c2.cp.kegg.v6.0.symbols.gmt");
		}else if( pathwayComb->currentText() == QString("Reactome") ){
			geneListFileName = string("data/MSigDB/c2_curated_gene_sets/c2.cp.reactome.v6.0.symbols.gmt");
		}else if( pathwayComb->currentText() == QString("chemical and genetic perturbations") ){
			geneListFileName = string("data/MSigDB/c2_curated_gene_sets/c2.cgp.v6.0.symbols.gmt");
		}else if( pathwayComb->currentText() == QString("Canonical pathways") ){
			geneListFileName = string("data/MSigDB/c2_curated_gene_sets/c2.cp.v6.0.symbols.gmt");
		}
		cout<<geneListFileName<<endl;
		annotation->readGeneListFile( geneListFileName );
		annotation->run();
		rnaSeqDataEdit->setText( QString("") );
		update();
		emit	finishRun();
		close();
	}
}


