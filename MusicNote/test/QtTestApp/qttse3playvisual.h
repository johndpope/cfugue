#ifndef QTTSE3PLAYVISUAL_H
#define QTTSE3PLAYVISUAL_H

#include <QDialog>

namespace Ui {
    class QtTse3PlayVisual;
}

class QtTse3PlayVisual : public QDialog
{
    Q_OBJECT

public:
    explicit QtTse3PlayVisual(QWidget *parent = 0);
    ~QtTse3PlayVisual();

private:
    Ui::QtTse3PlayVisual *ui;
};

#endif // QTTSE3PLAYVISUAL_H
