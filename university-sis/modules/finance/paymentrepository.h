#ifndef PAYMENTREPOSITORY_H
#define PAYMENTREPOSITORY_H

#include "payment.h"
#include <vector>
#include <optional>

class PaymentRepository {
public:
    PaymentRepository();
    
    bool addPayment(const Payment& payment);
    bool updatePayment(const Payment& payment);
    bool deletePayment(int id);
    
    std::vector<Payment> getAllPayments();
    std::vector<Payment> getAllPaymentsWithNames();  // Gets payments with student names
    std::optional<Payment> getPaymentById(int id);
};

#endif // PAYMENTREPOSITORY_H
