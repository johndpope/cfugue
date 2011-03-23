#ifndef MIDIOUTCOMBOBOX_H
#define MIDIOUTCOMBOBOX_H

#include <QComboBox>

#include "tse3/MidiScheduler.h"

class MidiOutComboBox : public QComboBox
{
    Q_OBJECT
public:
    explicit MidiOutComboBox(QWidget *parent = 0);

    void PopulateDevices(const TSE3::MidiScheduler*);

signals:

public slots:

};

#endif // MIDIOUTCOMBOBOX_H
