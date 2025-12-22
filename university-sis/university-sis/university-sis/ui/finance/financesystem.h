#ifndef FINANCESYSTEM_H
#define FINANCESYSTEM_H

#include <QWidget>
#include <QTableView>
#include <QStandardItemModel>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QLabel>
#include <QLineEdit>
#include <QPushButton>
#include "../../modules/finance/paymentrepository.h"

class FinanceSystem : public QWidget
{
    Q_OBJECT

public:
    explicit FinanceSystem(QWidget *parent = nullptr);

private slots:
    void onAddPayment();
    void onSearch(const QString &text);
    void editPayment(int id);
    void deletePayment(int id);

private:
    void setupUi();
    void loadPayments();
    void styleTable();
    void refreshData();

    QTableView *m_view;
    QStandardItemModel *m_model;
    QLineEdit *m_searchBar;
    QPushButton *m_btnAdd;
    
    PaymentRepository m_repo;
};

#endif // FINANCESYSTEM_H
