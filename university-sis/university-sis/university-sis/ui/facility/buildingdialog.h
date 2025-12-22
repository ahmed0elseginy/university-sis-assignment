#ifndef BUILDINGDIALOG_H
#define BUILDINGDIALOG_H

#include <QDialog>
#include <QLineEdit>
#include <QFormLayout>
#include <QDialogButtonBox>
#include "../../modules/facility/building.h"

class BuildingDialog : public QDialog {
    Q_OBJECT
public:
    explicit BuildingDialog(QWidget *parent = nullptr, Building *building = nullptr);
    Building getBuilding() const;

private:
    QLineEdit *m_nameEdit;
    QLineEdit *m_codeEdit;
    QLineEdit *m_locationEdit;
    QDialogButtonBox *m_buttonBox;
    int m_id = -1;
};

#endif // BUILDINGDIALOG_H
