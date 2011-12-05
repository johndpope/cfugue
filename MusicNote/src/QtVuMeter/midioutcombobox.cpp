#include "midioutcombobox.h"
#include "MidiDevice.h"

MidiOutComboBox::MidiOutComboBox(QWidget *parent) :
    QComboBox(parent)
{
}

void MidiOutComboBox::PopulateDevices()
{
    this->clear();
    for(int i=0, nCount = MusicNoteLib::GetMidiOutPortCount(); i < nCount; ++i)
        this->addItem(MusicNoteLib::GetMidiOutPortName(i).c_str());
}
