#ifndef DATABASE_H
#define DATABASE_H

#include <QString>
#include <QSqlDatabase>

class Database
{
public:
    static bool connectToDatabase();
    static bool isConnected();
    static bool checkCredentials(const QString &username, const QString &password,
                                 QString &userFullName, QString &userRole, int &userId,
                                 QString &userPhone);
    static void closeDatabase();

    static bool isLoginTaken(const QString &login);
    static bool registerUser(const QString &login,
                             const QString &password,
                             const QString &fullName,
                             const QString &phone = QString(),
                             const QString &role = "client");
    static QString hashPassword(const QString &password);

private:
    static QSqlDatabase m_database;
    Database() = delete;
    ~Database() = delete;
};

#endif // DATABASE_H
