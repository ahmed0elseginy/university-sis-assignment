#include "paymentrepository.h"
#include "../../database/databasemanager.h"
#include <QSqlQuery>
#include <QSqlError>
#include <QVariant>
#include <QDebug>

PaymentRepository::PaymentRepository() {}

bool PaymentRepository::addPayment(const Payment& payment) {
    QSqlDatabase db = DatabaseManager::instance().getDatabase();
    QSqlQuery query(db);
    query.prepare("INSERT INTO payments (student_id, amount, description, status, date) VALUES (:sid, :amt, :desc, :stat, :date)");
    query.bindValue(":sid", payment.studentId);
    query.bindValue(":amt", payment.amount);
    query.bindValue(":desc", payment.description);
    query.bindValue(":stat", payment.status);
    query.bindValue(":date", payment.date);
    
    if (!query.exec()) {
        qDebug() << "Add Payment Error:" << query.lastError().text();
        return false;
    }
    return true;
}

bool PaymentRepository::updatePayment(const Payment& payment) {
    QSqlDatabase db = DatabaseManager::instance().getDatabase();
    QSqlQuery query(db);
    query.prepare("UPDATE payments SET student_id=:sid, amount=:amt, description=:desc, status=:stat, date=:date WHERE payment_id=:id");
    query.bindValue(":sid", payment.studentId);
    query.bindValue(":amt", payment.amount);
    query.bindValue(":desc", payment.description);
    query.bindValue(":stat", payment.status);
    query.bindValue(":date", payment.date);
    query.bindValue(":id", payment.id);
    
    if (!query.exec()) {
        qDebug() << "Update Payment Error:" << query.lastError().text();
        return false;
    }
    return true;
}

bool PaymentRepository::deletePayment(int id) {
    QSqlDatabase db = DatabaseManager::instance().getDatabase();
    QSqlQuery query(db);
    query.prepare("DELETE FROM payments WHERE payment_id = :id");
    query.bindValue(":id", id);
    
    if (!query.exec()) {
        qDebug() << "Delete Payment Error:" << query.lastError().text();
        return false;
    }
    return true;
}

std::vector<Payment> PaymentRepository::getAllPayments() {
    std::vector<Payment> payments;
    QSqlDatabase db = DatabaseManager::instance().getDatabase();
    QSqlQuery query("SELECT payment_id, student_id, amount, description, status, date FROM payments", db);
    
    while (query.next()) {
        Payment p;
        p.id = query.value(0).toInt();
        p.studentId = query.value(1).toInt();
        p.amount = query.value(2).toDouble();
        p.description = query.value(3).toString();
        p.status = query.value(4).toString();
        p.date = query.value(5).toDate();
        payments.push_back(p);
    }
    return payments;
}

std::optional<Payment> PaymentRepository::getPaymentById(int id) {
    QSqlDatabase db = DatabaseManager::instance().getDatabase();
    QSqlQuery query(db);
    query.prepare("SELECT payment_id, student_id, amount, description, status, date FROM payments WHERE payment_id = :id");
    query.bindValue(":id", id);
    
    if (query.exec() && query.next()) {
        Payment p;
        p.id = query.value(0).toInt();
        p.studentId = query.value(1).toInt();
        p.amount = query.value(2).toDouble();
        p.description = query.value(3).toString();
        p.status = query.value(4).toString();
        p.date = query.value(5).toDate();
        return p;
    }
    return std::nullopt;
}
