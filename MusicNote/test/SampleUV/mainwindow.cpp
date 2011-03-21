#include "mainwindow.h"
#include "e:/codebase/qsynthmeter.h"
#include "ui_mainwindow.h"
#include <QtDebug>

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);
   // qsynthMeter* pMeter = new qsynthMeter(this);
    //ui->centralWidget = pMeter;
    //ui->widget = pMeter;
    QTimer *timer = new QTimer(this);
    connect(timer, SIGNAL(timeout()), this, SLOT(updateBars()));
    timer->start(100);
}

MainWindow::~MainWindow()
{
    delete ui;
}


    void MainWindow::mouseMoveEvent ( QMouseEvent * event )
    {

    }

    void MainWindow::updateBars()
    {
        int nVal = qrand() % 12;
        bool bNeg = (int)(qrand() % 2);
        if(bNeg) nVal = -nVal;

        ui->widget->setValue(0, nVal);
        ui->widget->setValue(1, nVal);

        int nVal2 = qrand() %100;

        ui->widget_2->GetBar(abs(nVal))->setValue(nVal2 );        
        qDebug() << nVal;
    }
