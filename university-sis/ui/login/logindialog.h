#ifndef LOGINDIALOG_H
#define LOGINDIALOG_H

#include <QDialog>
#include <QLineEdit>
#include <QComboBox>

class LoginDialog : public QDialog
{
    Q_OBJECT
public:
    struct UserSession {
        QString username;
        QString role;
        int userId;
    };

    explicit LoginDialog(QWidget *parent = nullptr);
    UserSession getSession() const;

private slots:
    void onLogin();

private:
    QLineEdit *m_userEdit;
    QLineEdit *m_passEdit;
    QComboBox *m_roleCombo; // For simulation/demo purposes, or real auth
    UserSession m_session;
};

#endif // LOGINDIALOG_H
