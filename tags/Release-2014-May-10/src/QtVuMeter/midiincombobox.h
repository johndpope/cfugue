#ifndef MIDIINCOMBOBOX_H
#define MIDIINCOMBOBOX_H

#include <QComboBox>

class MidiInComboBox : public QComboBox
{
    Q_OBJECT
public:
    explicit MidiInComboBox(QWidget *parent = 0);

    void PopulateDevices();
signals:

public slots:

};

#endif // MIDIINCOMBOBOX_H
