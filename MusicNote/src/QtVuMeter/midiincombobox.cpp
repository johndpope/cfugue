#include "midiincombobox.h"
#include "MidiDevice.h"

MidiInComboBox::MidiInComboBox(QWidget *parent) :
    QComboBox(parent)
{
}

void MidiInComboBox::PopulateDevices()
{
    this->clear();
    for(int i=0, nCount = MusicNoteLib::GetMidiInPortCount(); i < nCount; ++i)
     this->addItem(MusicNoteLib::GetMidiInPortName(i).c_str());
}
