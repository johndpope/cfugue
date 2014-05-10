#include "qttse3playvisual.h"
#include "ui_qttse3playvisual.h"

#include <QFileDialog>
#include <QMessageBox>
#include <QWaitCondition>
#include <QStringList>

#include "tse3play/tse3play.h"
#include "tse3/Error.h"
#include "tse3/MidiFile.h"
#include "tse3/Song.h"
#include "tse3/Track.h"

void CPlayerThread::run()
{
    try
    {
        m_pVisualDlg->m_pTransport->filter()->setPort(m_pVisualDlg->m_pScheduler->defaultExternalPort());
        m_pVisualDlg->m_pTransport->setAdaptiveLookAhead(false);
        m_pVisualDlg->m_pTransport->setAutoStop(true);
        TSE3::Clock startClock = (**m_pVisualDlg->m_pPlayable->iterator(0)).time ;
        m_pVisualDlg->m_pTransport->play(m_pVisualDlg->m_pPlayable, 0);//startClock);
        while(m_pVisualDlg->m_pTransport->status() == TSE3::Transport::Playing)
        {
            m_pVisualDlg->m_pTransport->poll();
            if(m_pVisualDlg->m_bShouldStopPlay == false)
                this->msleep(500); //500 ms
        }
        m_pVisualDlg->m_pTransport->stop();
    }
    catch (...)
    {
    }
}

QtTse3PlayVisual::QtTse3PlayVisual(QWidget *parent, TSE3::MidiScheduler* pSch, TSE3::Transport* pTrans) :
    QDialog(parent),
    ui(new Ui::QtTse3PlayVisual), m_pTransport(NULL), m_pScheduler(NULL), m_pMetronome(NULL), m_pPlayable(NULL), m_pPlayerThread(NULL)
{
    ui->setupUi(this);
    SetupMIDI(/*pSch, pTrans*/);

    ui->VuBar->SetBarCount(16, 0, 128); // setup 16 bars with values in the range [0,128]

    // Register the class for qt signals to work
    qRegisterMetaType<TSE3::MidiCommand>("TSE3::MidiCommand");
}

QtTse3PlayVisual::~QtTse3PlayVisual()
{
	on_pushButton_Stop_clicked();
    //if(m_pPlayable != NULL)
    //{
    //    delete m_pPlayable;
    //    m_pPlayable = NULL;
    //}
    // No need to delete Scheduler and Metronome. Transport will take care of it
    if(m_pTransport != NULL)
    {
        delete m_pTransport;
        m_pTransport = NULL;
    }
    delete ui;
}

void QtTse3PlayVisual::SetupMIDI(/*TSE3::MidiScheduler* pSch, TSE3::Transport* pTrans*/)
{
    //if(pSch == NULL)
    //{
        if(m_pScheduler == NULL)
        {
            TSE3::MidiSchedulerFactory msf;
            m_pScheduler = msf.createScheduler();
        }

    //    pSch = m_pScheduler;
    //}
    //else
    //    m_pScheduler = pSch;


    ui->comboBox_MidiOut->PopulateDevices();
    ui->comboBox_MidiIn->PopulateDevices();

    TSE3::Listener<TSE3::MidiSchedulerListener>::attachTo(m_pScheduler); // Lets listen to Scheduler Events

    // Create the scheduler and other playback objects
    m_pMetronome = new TSE3::Metronome();
    //m_pMetronome->setStatus(Transport::Playing, dometronome);
    m_pTransport = new TSE3::Transport(m_pMetronome, m_pScheduler);
    setPanic(m_pTransport->startPanic());
    setPanic(m_pTransport->endPanic());
    m_pTransport->midiEcho()->filter()->setStatus(false);

    //TSE3::Listener<TSE3::TransportListener>::attachTo(m_pTransport);
}

void QtTse3PlayVisual::on_pushButton_Load_clicked()
{
    BusyWaitCursor dummy(this);

    QString strFilePath = QFileDialog::getOpenFileName(this, "Open a MIDI File to Play", QString(), "MIDI Files (*.mid);;All Files (*.*)");

    if(!strFilePath.isEmpty())
    {
        if(m_pPlayable)
        {
            delete m_pPlayable;
            m_pPlayable = NULL;
        }
        m_strFilePath = "";

         try //tse3 has this bad habit of throwing exceptions instead of returning error values
         {
            int               soloTrack=-1;
            std::list<size_t> muteList;

            TSE3::MidiFileImport* pMFI = new TSE3::MidiFileImport(strFilePath.toStdString());
            TSE3::Song* pSong = pMFI->load();
            if(pSong != NULL)
            {
                m_pPlayable = pSong;
                if (pSong)
                {
                    pSong->setSoloTrack(soloTrack);
                    std::list<size_t>::iterator i = muteList.begin();
                    while (i != muteList.end())
                    {
                        if (*i <= pSong->size())
                            (*pSong)[*i]->filter()->setStatus(false);
                        i++;
                    }
                }
                this->ui->pushButton_Start->setEnabled(true);
                m_strFilePath = strFilePath;
                ui->label_Status->setText("File: " + m_strFilePath);
            }
            return;
         }
         catch(...)
         {
            ui->label_Status->setText("Unable to Load: " + strFilePath);
         }
    }

}

void QtTse3PlayVisual::on_pushButton_Start_clicked()
{
    if(m_pPlayable != NULL) // only if already loaded
    {
        if(m_pPlayerThread != NULL) return; // return if playing something already

        m_bShouldStopPlay = false;

        m_pPlayerThread = new CPlayerThread(this);
        m_pPlayerThread->start();
    }
}

void QtTse3PlayVisual::on_pushButton_Stop_clicked()
{
    if(m_pPlayerThread != NULL)
    {
    	m_bShouldStopPlay = true;
    	m_pTransport->stop();
    	//m_pPlayerThread->quit();
    	//m_pPlayerThread->wait(1000);
    	//delete m_pPlayerThread;
    	//m_pPlayerThread = NULL;
	}
}

/**
* Sets up the given panic object according to the variables
* midi, gm, gs, xg.
*/
void QtTse3PlayVisual::setPanic(TSE3::Panic *panic)
{
    panic->setMidiReset(false);
    panic->setGmReset(false);
    panic->setGsReset(false);
    panic->setXgReset(false);
}

void QtTse3PlayVisual::OnPlayerThreadStarted()
{
    ui->pushButton_Start->setEnabled(false);
    ui->pushButton_Stop->setEnabled(true);
    ui->pushButton_Load->setEnabled(false);
}
void QtTse3PlayVisual::OnPlayerThreadFinished()
{
	OnPlayerThreadTerminated();
}
void QtTse3PlayVisual::OnPlayerThreadTerminated()
{
	m_pPlayerThread = NULL;
    ui->pushButton_Start->setEnabled(true);
    ui->pushButton_Stop->setEnabled(false);
    ui->pushButton_Load->setEnabled(true);
}
void QtTse3PlayVisual::OnMidiCommand(TSE3::MidiCommand c)
{
    if (c.status == TSE3::MidiCommands::MidiCommand_NoteOn)
    {
        const int max = 100;

        int newval = c.data2 * max / 127;
        //if (newval > next[c.channel] && newval != now[c.channel])
        //    next[c.channel] = newval;

        ui->VuBar->GetBar(c.channel)->setValue(c.data1);
    }
    else if(c.status == TSE3::MidiCommands::MidiCommand_NoteOff)
    {
        ui->VuBar->GetBar(c.channel)->setValue(0);
    }
}
