#include "user.h"
#include "database.h"
#include <QDebug>
#include <QSqlQuery>
#include <QDateTime>
User::User()
    : m_id(0), m_login(""), m_role("client"), m_fullName(""), m_phone("")
{
}

User::User(int id, const QString& login, const QString& role,
           const QString& fullName, const QString& phone)
    : m_id(id), m_login(login), m_role(role),
    m_fullName(fullName), m_phone(phone)
{
}

User* User::authenticate(const QString& login, const QString& password)
{
    QString fullName;
    QString role;
    QString phone;
    int id;

    bool success = Database::checkCredentials(login, password,
                                              fullName, role, id, phone);

    if (success) {
        User* user = new User(id, login, role, fullName, phone);
        return user;
    }

    return nullptr;
}

User* User::getUserById(int id)
{
    if (!Database::isConnected()) {
        qDebug() << "База данных не подключена";
        return nullptr;
    }

    QSqlQuery query;
    query.prepare(
        "SELECT id, login, role, full_name, phone, registration_date "
        "FROM users WHERE id = :id"
        );
    query.bindValue(":id", id);

    if (!query.exec()) {
        return nullptr;
    }

    if (!query.next()) {
        qDebug() << "Пользователь с ID" << id << "не найден";
        return nullptr;
    }

    // Создаем пользователя с параметрами конструктора
    User* user = new User(
        query.value("id").toInt(),         // id
        query.value("login").toString(),   // login
        query.value("role").toString(),    // role
        query.value("full_name").toString(), // fullName
        query.value("phone").toString()    // phone
        );

    return user;
}
