#include "paymentdialog.h"

PaymentDialog::PaymentDialog(QWidget *parent, const Payment* payment)
    : QDialog(parent), m_isEdit(payment != nullptr)
{
    if (payment) {
        m_payment = *payment;
    } else {
        m_payment = Payment();
    }
    
    setWindowTitle(m_isEdit ? "Edit Payment" : "New Payment");
    setWindowFlags(windowFlags() & ~Qt::WindowContextHelpButtonHint);
    setMinimumWidth(400);
    
    setupUi();
}

void PaymentDialog::setupUi()
{
    QVBoxLayout *mainLayout = new QVBoxLayout(this);
    mainLayout->setContentsMargins(20, 20, 20, 20);
    mainLayout->setSpacing(20);

    // Header
    QLabel *titleLabel = new QLabel(m_isEdit ? "Update Payment" : "Register Payment");
    titleLabel->setStyleSheet("font-size: 18px; font-weight: bold; color: #2c3e50; margin-bottom: 10px;");
    mainLayout->addWidget(titleLabel);

    // Form
    QFormLayout *formLayout = new QFormLayout();
    formLayout->setSpacing(15);
    formLayout->setLabelAlignment(Qt::AlignRight | Qt::AlignVCenter);

    m_studentIdEdit = new QSpinBox();
    m_studentIdEdit->setRange(1, 999999);
    m_studentIdEdit->setStyleSheet("QSpinBox { padding: 8px; border: 1px solid #bdc3c7; border-radius: 4px; }");
    
    m_amountEdit = new QDoubleSpinBox();
    m_amountEdit->setRange(0.0, 1000000.0);
    m_amountEdit->setPrefix("$");
    m_amountEdit->setStyleSheet("QDoubleSpinBox { padding: 8px; border: 1px solid #bdc3c7; border-radius: 4px; }");
    
    m_descEdit = new QLineEdit();
    m_descEdit->setPlaceholderText("Tuition Fee, Gym Fee, etc.");
    m_descEdit->setStyleSheet("QLineEdit { padding: 8px; border: 1px solid #bdc3c7; border-radius: 4px; }");
    
    m_statusEdit = new QComboBox();
    m_statusEdit->addItems({"Pending", "Paid", "Overdue"});
    m_statusEdit->setStyleSheet("QComboBox { padding: 8px; border: 1px solid #bdc3c7; border-radius: 4px; }");
    
    m_dateEdit = new QDateEdit();
    m_dateEdit->setCalendarPopup(true);
    m_dateEdit->setDisplayFormat("yyyy-MM-dd");
    m_dateEdit->setStyleSheet("QDateEdit { padding: 8px; border: 1px solid #bdc3c7; border-radius: 4px; }");

    // Pre-fill
    if (m_isEdit) {
        m_studentIdEdit->setValue(m_payment.studentId);
        m_amountEdit->setValue(m_payment.amount);
        m_descEdit->setText(m_payment.description);
        m_statusEdit->setCurrentText(m_payment.status);
        m_dateEdit->setDate(m_payment.date);
    } else {
        m_dateEdit->setDate(QDate::currentDate());
    }

    formLayout->addRow(new QLabel("Student ID:"), m_studentIdEdit);
    formLayout->addRow(new QLabel("Amount:"), m_amountEdit);
    formLayout->addRow(new QLabel("Description:"), m_descEdit);
    formLayout->addRow(new QLabel("Status:"), m_statusEdit);
    formLayout->addRow(new QLabel("Date:"), m_dateEdit);

    mainLayout->addLayout(formLayout);

    // Buttons
    QHBoxLayout *btnLayout = new QHBoxLayout();
    btnLayout->addStretch();

    m_cancelBtn = new QPushButton("Cancel");
    m_cancelBtn->setCursor(Qt::PointingHandCursor);
    m_cancelBtn->setStyleSheet("QPushButton { background-color: #95a5a6; color: white; border: none; padding: 10px 20px; border-radius: 4px; font-weight: bold; } "
                               "QPushButton:hover { background-color: #7f8c8d; }");
    
    m_saveBtn = new QPushButton("Save");
    m_saveBtn->setCursor(Qt::PointingHandCursor);
    m_saveBtn->setStyleSheet("QPushButton { background-color: #3498db; color: white; border: none; padding: 10px 20px; border-radius: 4px; font-weight: bold; } "
                             "QPushButton:hover { background-color: #2980b9; }");

    btnLayout->addWidget(m_cancelBtn);
    btnLayout->addWidget(m_saveBtn);

    mainLayout->addLayout(btnLayout);

    connect(m_cancelBtn, &QPushButton::clicked, this, &QDialog::reject);
    connect(m_saveBtn, &QPushButton::clicked, this, &PaymentDialog::onSave);
}

void PaymentDialog::onSave()
{
    if (m_descEdit->text().isEmpty()) {
        QMessageBox::warning(this, "Validation Error", "Description cannot be empty.");
        return;
    }

    m_payment.studentId = m_studentIdEdit->value();
    m_payment.amount = m_amountEdit->value();
    m_payment.description = m_descEdit->text();
    m_payment.status = m_statusEdit->currentText();
    m_payment.date = m_dateEdit->date();

    accept();
}

Payment PaymentDialog::getPayment() const
{
    return m_payment;
}
