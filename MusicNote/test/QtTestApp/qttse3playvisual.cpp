#include "qttse3playvisual.h"
#include "ui_qttse3playvisual.h"

#include <QFileDialog>

#include "tse3play/tse3play.h"
#include "tse3/Error.h"

//int TSE3PlayMain(int argc, char *argv[])
//{
//    try
//    {
//        TSE3Play tseplay(argc, argv);
//        return tseplay.go();
//    }
//    catch (const TSE3::Error &e)
//    {
//        std::cerr << "TSE3 error detected: " << TSE3::errString(e.reason())
//                  << "\n";
//        return 1;
//    }
//}

// class PlayerThread : public QThread
// {

// public:
//     MyThread(QString strFilePath) : _argc(argc), _argv(argv) { }
//     void run()
//     {
//         try
//         {
//             TSE3Play tseplay(argc, argv);
//             return tseplay.go();
//         }
//         catch (const TSE3::Error &e)
//         {
//             std::cerr << "TSE3 error detected: " << TSE3::errString(e.reason())
//                       << "\n";
//             return 1;
//         }
//     }
// };


QtTse3PlayVisual::QtTse3PlayVisual(QWidget *parent, TSE3::MidiScheduler* pSch, TSE3::Transport* pTrans) :
    QDialog(parent),
    ui(new Ui::QtTse3PlayVisual), m_pTransport(NULL), m_pScheduler(NULL)
{
    ui->setupUi(this);
    SetupMIDI(pSch, pTrans);
}

QtTse3PlayVisual::~QtTse3PlayVisual()
{
    delete ui;
}

void QtTse3PlayVisual::SetupMIDI(TSE3::MidiScheduler* pSch, TSE3::Transport* pTrans)
{
    if(pSch == NULL)
    {
        if(m_pScheduler == NULL)
        {
            TSE3::MidiSchedulerFactory msf;
            m_pScheduler = msf.createScheduler();
        }

        pSch = m_pScheduler;
    }

    attachTo(pSch); // Lets listen to Scheduler Events

    ui->comboBox_MidiOut->PopulateDevices(pSch);

    // Create the scheduler and other playback objects
    metronome = new Metronome();
    metronome->setStatus(Transport::Playing, dometronome);
    transport = new Transport(metronome, sch);
    setPanic(transport->startPanic());
    setPanic(transport->endPanic());
    transport->midiEcho()->filter()->setStatus(doecho);
}

void QtTse3PlayVisual::on_pushButton_Load_clicked()
{
    QString strFilePath = QFileDialog::getOpenFileName(this, "Open a MIDI File to Play", QString(), "MIDI Files (*.mid);;All Files (*.*)");

    if(!strFilePath.isEmpty())
        m_strFilePath = strFilePath;

    ui->label_Status->setText("File: " + m_strFilePath);
}

void QtTse3PlayVisual::on_pushButton_Start_clicked()
{
    if(!m_strFilePath.isEmpty())
    {
    }
}
