#ifndef QTTSE3PLAYVISUAL_H
#define QTTSE3PLAYVISUAL_H

#include <QDialog>
#include <tse3/MidiScheduler.h>
#include <tse3/Transport.h>

namespace Ui {
    class QtTse3PlayVisual;
}

class QtTse3PlayVisual : public QDialog,
                         public TSE3::Listener<TSE3::MidiSchedulerListener>
{
    Q_OBJECT

public:
    explicit QtTse3PlayVisual(QWidget *parent = 0, TSE3::MidiScheduler* pSch = NULL, TSE3::Transport* pTrans = NULL);
    ~QtTse3PlayVisual();

    void SetupMIDI(TSE3::MidiScheduler* pSch = NULL, TSE3::Transport* pTrans = NULL);

private:
    Ui::QtTse3PlayVisual *ui;
    QString m_strFilePath;

    TSE3::MidiScheduler* m_pScheduler;
    TSE3::Transport*    m_pTransport;

private slots:
    void on_pushButton_Start_clicked();
    void on_pushButton_Load_clicked();
};

#endif // QTTSE3PLAYVISUAL_H
