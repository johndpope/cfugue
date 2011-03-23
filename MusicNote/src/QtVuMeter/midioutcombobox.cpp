#include "midioutcombobox.h"

MidiOutComboBox::MidiOutComboBox(QWidget *parent) :
    QComboBox(parent)
{
}

void MidiOutComboBox::PopulateDevices(const TSE3::MidiScheduler* sch)
{
    QStringList strDeviceList;

    std::vector<int> portNums;
    sch->portNumbers(portNums);
    for (size_t port = 0; port < sch->numPorts(); port++)
    {
        if(sch->portWriteable(portNums[port]))
        {
            QString strDevice;
            strDevice = "[" + portNums[port] + "] : " +
                        sch->portName(portNums[port]).c_str() +
                        " : "                    +
                        sch->portType(portNums[port]).c_str() +
                       (sch->portInternal(portNums[port]) ? "Int" : "Ext");
            strDeviceList << strDevice;
        }
    }
    this->clear();
    this->addItems(strDeviceList);
}
