#ifndef DATABASEMANAGER_H
#define DATABASEMANAGER_H

#include <QSqlDatabase>
#include <QSqlError>
#include <QSqlQuery>
#include <QString>
#include <QDebug>

class DatabaseManager
{
public:
    static DatabaseManager& instance();
    bool connect();
    bool isOpen() const;
    QSqlDatabase getDatabase() const;
    void initSchema(); // Helper to create tables if they don't exist

private:
    DatabaseManager();
    ~DatabaseManager();
    QSqlDatabase m_db;
};

#endif // DATABASEMANAGER_H
