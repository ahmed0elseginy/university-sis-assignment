#include "roomdialog.h"
#include <QVBoxLayout>
#include <QFormLayout>

RoomDialog::RoomDialog(const QList<Building>& buildings, QWidget *parent, Room *room) : QDialog(parent)
{
    setWindowTitle(room ? "Edit Room" : "Add Room");

    m_buildingCombo = new QComboBox(this);
    for(const auto& b : buildings) {
        m_buildingCombo->addItem(b.name, b.id);
    }

    m_numberEdit = new QLineEdit(this);
    m_typeCombo = new QComboBox(this);
    m_typeCombo->addItems({"Lecture Hall", "Laboratory", "Office", "Meeting Room"});
    
    m_capacitySpin = new QSpinBox(this);
    m_capacitySpin->setRange(1, 1000);
    m_capacitySpin->setValue(30);

    QFormLayout *formLayout = new QFormLayout;
    formLayout->addRow("Building:", m_buildingCombo);
    formLayout->addRow("Room Number:", m_numberEdit);
    formLayout->addRow("Type:", m_typeCombo);
    formLayout->addRow("Capacity:", m_capacitySpin);

    m_buttonBox = new QDialogButtonBox(QDialogButtonBox::Ok | QDialogButtonBox::Cancel, this);
    connect(m_buttonBox, &QDialogButtonBox::accepted, this, &QDialog::accept);
    connect(m_buttonBox, &QDialogButtonBox::rejected, this, &QDialog::reject);

    QVBoxLayout *mainLayout = new QVBoxLayout(this);
    mainLayout->addLayout(formLayout);
    mainLayout->addWidget(m_buttonBox);

    if (room) {
        m_id = room->id;
        // Select current building
        int idx = m_buildingCombo->findData(room->buildingId);
        if(idx != -1) m_buildingCombo->setCurrentIndex(idx);
        
        m_numberEdit->setText(room->roomNumber);
        m_typeCombo->setCurrentText(room->type);
        m_capacitySpin->setValue(room->capacity);
    }
}

Room RoomDialog::getRoom() const
{
    Room r;
    r.id = m_id;
    r.buildingId = m_buildingCombo->currentData().toInt();
    r.roomNumber = m_numberEdit->text();
    r.type = m_typeCombo->currentText();
    r.capacity = m_capacitySpin->value();
    return r;
}
