#include "midiincombobox.h"
#include "MidiDevice.h"

MidiInComboBox::MidiInComboBox(QWidget *parent) :
    QComboBox(parent)
{
}

void MidiInComboBox::PopulateDevices()
{
    QStringList strDeviceList;

    int nCount = MusicNoteLib::GetMidiInPortCount();
    for(int i=0; i < nCount; ++i)
        strDeviceList << MusicNoteLib::GetMidiInPortName(i);

    this->clear();
    this->addItems(strDeviceList);
}
