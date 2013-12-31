#ifndef QTTSE3PLAYVISUAL_H
#define QTTSE3PLAYVISUAL_H

#include <QDialog>
#include "tse3/Metronome.h"
#include "tse3/util/MidiScheduler.h"
#include "tse3/Transport.h"
#include <QThread>

namespace Ui {
    class QtTse3PlayVisual;
}

class CPlayerThread;

class QtTse3PlayVisual : public QDialog,
                         public TSE3::Listener<TSE3::MidiSchedulerListener>
                        // public TSE3::Listener<TSE3::TransportListener>
{
    Q_OBJECT

public:
    explicit QtTse3PlayVisual(QWidget *parent = 0, TSE3::MidiScheduler* pSch = NULL, TSE3::Transport* pTrans = NULL);

    ~QtTse3PlayVisual();

    void SetupMIDI(/*TSE3::MidiScheduler* pSch = NULL, TSE3::Transport* pTrans = NULL*/);

protected:

private:
    Ui::QtTse3PlayVisual *ui;
    QString m_strFilePath;
public:
    TSE3::MidiScheduler* m_pScheduler;
    TSE3::Transport*    m_pTransport;
    TSE3::Metronome*    m_pMetronome;

    TSE3::Playable * m_pPlayable;

    CPlayerThread*  m_pPlayerThread;

    bool m_bShouldStopPlay;

    class BusyWaitCursor
    {
        QtTse3PlayVisual* m_pDlg;
    public:
        BusyWaitCursor(QtTse3PlayVisual* pDlg): m_pDlg(pDlg) { pDlg->setCursor(Qt::WaitCursor); }
        ~BusyWaitCursor() { m_pDlg->unsetCursor(); }
    };
private slots:
	void on_pushButton_Stop_clicked();
    void on_pushButton_Start_clicked();
    void on_pushButton_Load_clicked();
    void OnPlayerThreadStarted();
    void OnPlayerThreadFinished();
    void OnPlayerThreadTerminated();
    void OnMidiCommand(TSE3::MidiCommand c);

    void setPanic(TSE3::Panic *panic);
};


 class CPlayerThread : public QThread, public TSE3::TransportCallback
 {
     Q_OBJECT

     QtTse3PlayVisual*  m_pVisualDlg;
 public:
     CPlayerThread(QtTse3PlayVisual* pVisual): m_pVisualDlg(pVisual)
     {
         connect(this, SIGNAL(started()), m_pVisualDlg, SLOT(OnPlayerThreadStarted()), Qt::QueuedConnection);
         connect(this, SIGNAL(finished()), m_pVisualDlg, SLOT(OnPlayerThreadFinished()), Qt::QueuedConnection);
         connect(this, SIGNAL(terminated()), m_pVisualDlg, SLOT(OnPlayerThreadTerminated()), Qt::QueuedConnection);

         connect(this, SIGNAL(OnMidiCommand(TSE3::MidiCommand)), m_pVisualDlg, SLOT(OnMidiCommand(TSE3::MidiCommand)), Qt::QueuedConnection);

         if(m_pVisualDlg->m_pTransport)
             m_pVisualDlg->m_pTransport->attachCallback(this);
     }
     ~CPlayerThread()
     {
         if(m_pVisualDlg->m_pTransport)
             m_pVisualDlg->m_pTransport->detachCallback(this);
     }

     void run();

    /** @reimplemented for TSE3::TransportCallback    */
    virtual void Transport_MidiIn(TSE3::MidiCommand c) { emit OnMidiCommand(c); }

    /** @reimplemented for TSE3::TransportCallback   */
    virtual void Transport_MidiOut(TSE3::MidiCommand c) { emit OnMidiCommand(c); }

    signals:
        void OnMidiCommand(TSE3::MidiCommand c);
 };

#endif // QTTSE3PLAYVISUAL_H
