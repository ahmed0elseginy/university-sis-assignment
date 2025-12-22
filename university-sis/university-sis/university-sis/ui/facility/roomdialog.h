#ifndef ROOMDIALOG_H
#define ROOMDIALOG_H

#include <QDialog>
#include <QLineEdit>
#include <QComboBox>
#include <QSpinBox>
#include <QDialogButtonBox>
#include "../../modules/facility/room.h"
#include "../../modules/facility/building.h"
#include <QList>

class RoomDialog : public QDialog {
    Q_OBJECT
public:
    explicit RoomDialog(const QList<Building>& buildings, QWidget *parent = nullptr, Room *room = nullptr);
    Room getRoom() const;

private:
    QComboBox *m_buildingCombo;
    QLineEdit *m_numberEdit;
    QComboBox *m_typeCombo;
    QSpinBox *m_capacitySpin;
    QDialogButtonBox *m_buttonBox;
    int m_id = -1;
};

#endif // ROOMDIALOG_H
