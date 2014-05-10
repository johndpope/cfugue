#include "midioutcombobox.h"
#include "MidiDevice.h"

MidiOutComboBox::MidiOutComboBox(QWidget *parent) :
    QComboBox(parent)
{
}

void MidiOutComboBox::PopulateDevices()
{
    this->clear();
    for(int i=0, nCount = CFugue::GetMidiOutPortCount(); i < nCount; ++i)
        this->addItem(CFugue::GetMidiOutPortName(i).c_str());
}
