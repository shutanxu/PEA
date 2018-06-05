#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include<QGridLayout>
#include <QtWidgets>

#include"pathviewer.h"
#include"pathviewer2.h"
#include"annotationDialog.h"
#include"heatmap.h"

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(int argc, char *argv[]);
    ~MainWindow();

private slots:
    void            viewAxis();
    void            viewLabel();
    void            viewEdgeLabel();
    void			dragConnectedNode();
    void			dragFixedEdgeLength();
    void			view3D();
    void			searchGene();
    void			clearSearchGene();
    void			openKeggPathwayFile();
    void			openKeggPathwayFile_test();
    void			openExpressionFile();
    void			savePathway();
    void			saveImage();
    void			downloadKeggPathway();
    void			readKeggPathway( );
    void			geneAnnotation();
    void			updateTable();
    void			updateSideTable(int r, int c);
    void			updatePathway(int r, int c);
    void			drawHeatmap();
    void			updateHeatmap();
    void			about();

private:
    KGML_pathway	keggPathway;
    Pathway 		pathway;
    vector<string>	annotatedPathwayVec;
    PathwayViewer*	pathViewer;
    PathwayViewer2*	pathViewer2;
    HeatmapViewer*	heatmap;
    QTabWidget*		sideTabWidget;
    QTableWidget*	testTableWidget;
    QTabWidget*		buttomTabWidget;
    QTextEdit*		sideTextEdit;
    QTextEdit*		buttomTextEdit;
	HttpWindow*		keggDlg;
	AnnotationDialog*	annotateDlg;
    QMenu*			fileMenu;
    QAction*		saveAction;
    QAction*		openKeggPathwayAction;
    QAction*		openExpressionAction;
    QAction*		downloadKeggAction;
    QAction*		savePathwayAction;
    QAction*		saveImageAction;
    QAction*		clearAction;

    QMenu*			editMenu;

    QMenu*			viewMenu;
    QAction*		viewAxisAction;
    QAction*		viewLabelAction;
    QAction*		viewEdgeLabelAction;
    QAction*		viewStickAction;
    QAction*		viewSphereAction;
    QAction*		viewSurfaceAction;
    QAction*		viewResSurfaceAction;
    QAction*		view3DAction;
    QAction*		dragConnectedNodeAction;
    QAction*		dragFixedEdgeLengthAction;

    QMenu*			toolsMenu;
    QAction*		rmsdAction;

    QMenu*			aboutMenu;
    QAction*		aboutAction;

    QAction*		annotationAction;
    QAction*		geneHeatmapAction;
    HeatmapDialog*	heatmapDlg;

    QToolBar*		editToolBar;
    QLineEdit*		searchEdit;
    QAction*		searchAct;
    QAction*		clearAct;
private:
    void    createMenus();
    void    createActions();
    void	createToolBars();
    void    updateWindow();
};

#endif // MAINWINDOW_H
