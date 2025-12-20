#ifndef PAYMENT_H
#define PAYMENT_H

#include <QString>
#include <QDate>

struct Payment {
    int id = 0;
    int studentId = 0;
    double amount = 0.0;
    QString description;
    QString status = "Pending"; // Pending, Paid, Overdue
    QDate date = QDate::currentDate();
};

#endif // PAYMENT_H
