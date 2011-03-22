#include "qttse3playvisual.h"
#include "ui_qttse3playvisual.h"

QtTse3PlayVisual::QtTse3PlayVisual(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::QtTse3PlayVisual)
{
    ui->setupUi(this);
}

QtTse3PlayVisual::~QtTse3PlayVisual()
{
    delete ui;
}
