#include "SampleVU_mainwindow.h"
#include "ui_SampleVU_mainwindow.h"
#include <QtDebug>

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    QStringList strList;
    strList<< "S" << "R" << "G" << "M" << "P" << "D" << "N";
    ui->widget->SetBars(strList);

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
        updateBars();
    }

    void MainWindow::updateBars()
    {            
        int nVal = qrand() % 100;

        for(int i=0, nMax  = ui->widget->GetBarCount(); i < nMax; ++i)
        {
            int nValRel = qrand() % 10;
            bool bNeg = (int)(qrand() % 2);
            if(bNeg) nValRel = -nValRel;

            nVal += nValRel;

            ui->widget->GetBar(i)->setValue(nVal);
        }      

    }
