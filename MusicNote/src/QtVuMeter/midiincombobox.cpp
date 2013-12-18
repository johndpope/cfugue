#include "midiincombobox.h"
#include "MidiDevice.h"

MidiInComboBox::MidiInComboBox(QWidget *parent) :
    QComboBox(parent)
{
}

void MidiInComboBox::PopulateDevices()
{
    this->clear();
    for(int i=0, nCount = CFugue::GetMidiInPortCount(); i < nCount; ++i)
     this->addItem(CFugue::GetMidiInPortName(i).c_str());
}
