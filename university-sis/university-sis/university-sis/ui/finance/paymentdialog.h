#ifndef PAYMENTDIALOG_H
#define PAYMENTDIALOG_H

#include <QDialog>
#include <QLineEdit>
#include <QSpinBox>
#include <QDoubleSpinBox>
#include <QComboBox>
#include <QDateEdit>
#include <QPushButton>
#include <QVBoxLayout>
#include <QFormLayout>
#include <QLabel>
#include <QMessageBox>
#include "../../modules/finance/payment.h"

class PaymentDialog : public QDialog {
    Q_OBJECT
public:
    explicit PaymentDialog(QWidget *parent = nullptr, const Payment* payment = nullptr);
    Payment getPayment() const;

private slots:
    void onSave();

private:
    void setupUi();
    
    QSpinBox *m_studentIdEdit;
    QDoubleSpinBox *m_amountEdit;
    QLineEdit *m_descEdit;
    QComboBox *m_statusEdit;
    QDateEdit *m_dateEdit;
    
    QPushButton *m_saveBtn;
    QPushButton *m_cancelBtn;

    Payment m_payment;
    bool m_isEdit;
};

#endif // PAYMENTDIALOG_H
