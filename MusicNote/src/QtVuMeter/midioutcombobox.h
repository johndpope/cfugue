#ifndef MIDIOUTCOMBOBOX_H
#define MIDIOUTCOMBOBOX_H

#include <QComboBox>

class MidiOutComboBox : public QComboBox
{
    Q_OBJECT
public:
    explicit MidiOutComboBox(QWidget *parent = 0);

    void PopulateDevices();

signals:

public slots:

};

#endif // MIDIOUTCOMBOBOX_H
