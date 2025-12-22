#include "buildingdialog.h"
#include <QVBoxLayout>
#include <QMessageBox>

BuildingDialog::BuildingDialog(QWidget *parent, Building *building) : QDialog(parent)
{
    setWindowTitle(building ? "Edit Building" : "Add Building");

    m_nameEdit = new QLineEdit(this);
    m_codeEdit = new QLineEdit(this);
    m_locationEdit = new QLineEdit(this);
    
    QFormLayout *formLayout = new QFormLayout;
    formLayout->addRow("Name:", m_nameEdit);
    formLayout->addRow("Code:", m_codeEdit);
    formLayout->addRow("Location:", m_locationEdit);

    m_buttonBox = new QDialogButtonBox(QDialogButtonBox::Ok | QDialogButtonBox::Cancel, this);
    connect(m_buttonBox, &QDialogButtonBox::accepted, this, &QDialog::accept);
    connect(m_buttonBox, &QDialogButtonBox::rejected, this, &QDialog::reject);

    QVBoxLayout *mainLayout = new QVBoxLayout(this);
    mainLayout->addLayout(formLayout);
    mainLayout->addWidget(m_buttonBox);

    if (building) {
        m_id = building->id;
        m_nameEdit->setText(building->name);
        m_codeEdit->setText(building->code);
        m_locationEdit->setText(building->location);
    }
}

Building BuildingDialog::getBuilding() const
{
    Building b;
    b.id = m_id;
    b.name = m_nameEdit->text();
    b.code = m_codeEdit->text();
    b.location = m_locationEdit->text();
    return b;
}
