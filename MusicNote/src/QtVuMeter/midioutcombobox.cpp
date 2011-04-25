#include "midioutcombobox.h"
#include "MidiDevice.h"

MidiOutComboBox::MidiOutComboBox(QWidget *parent) :
    QComboBox(parent)
{
}

void MidiOutComboBox::PopulateDevices()
{
    QStringList strDeviceList;

    int nCount = MusicNoteLib::GetMidiOutPortCount();
    for(int i=0; i < nCount; ++i)
        strDeviceList << MusicNoteLib::GetMidiOutPortName(i);

    //std::vector<int> portNums;
    //sch->portNumbers(portNums);
    //for (size_t port = 0; port < sch->numPorts(); port++)
    //{
    //    if(sch->portWriteable(portNums[port]))
    //    {
    //        QString strDevice;
    //        strDevice = "[" + portNums[port] + "] : " +
    //                    sch->portName(portNums[port]).c_str() +
    //                    " : "                    +
    //                    sch->portType(portNums[port]).c_str() +
    //                   (sch->portInternal(portNums[port]) ? "Int" : "Ext");
    //        strDeviceList << strDevice;
    //    }
    //}
    this->clear();
    this->addItems(strDeviceList);
}
