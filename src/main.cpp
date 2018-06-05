#include <QApplication>
#include "mainwindow.h"
#include"testpathway.h"
#include"geneAnnotation.h"

int main(int argc, char *argv[])
{
//	test_tTestSort();
//    testPathway();
    QApplication a(argc, argv);
    MainWindow *w = new MainWindow(argc, argv);
    w->show();

    return a.exec();
}
