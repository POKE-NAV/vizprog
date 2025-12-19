#ifndef USER_H
#define USER_H

#include <QString>

class User
{
public:
    // Конструкторы
    User();
    User(int id, const QString& login, const QString& role, const QString& fullName, const QString& phone);

    // Геттеры
    int getId() const { return m_id; }
    QString getLogin() const { return m_login; }
    QString getRole() const { return m_role; }
    QString getFullName() const { return m_fullName; }
    QString getPhone() const { return m_phone; }
    static User* getUserById(int id);
    // Проверки ролей
    bool isAdmin() const { return m_role == "admin"; }
    bool isValid() const { return m_id > 0 && !m_login.isEmpty(); }

    // Статический метод аутентификации
    static User* authenticate(const QString& login, const QString& password);

private:
    int m_id;
    QString m_login;
    QString m_role;
    QString m_fullName;
    QString m_phone;
};

#endif // USER_H
