#include "database.h"
#include <QSqlDatabase>
#include <QSqlQuery>
#include <QSqlError>
#include <QDebug>
#include <QMessageBox>
#include <QCoreApplication>
#include <QDir>
#include <QFile>
#include <QStandardPaths>
#include <QCryptographicHash>

QSqlDatabase Database::m_database;

bool Database::connectToDatabase()
{
    QString dbPath = "C:/Users/ignat/OneDrive/Рабочий стол/qt/EquipmentRentalService/rental.db";

    // Проверка существования файла БД
    if (!QFile::exists(dbPath)) {
        return false;
    }

    // Если БД уже открыта и валидна - ничего не делаем
    if (m_database.isOpen() && m_database.isValid()) {
        return true;
    }

    // Закрываем старое подключение, если оно есть
    if (m_database.isOpen()) {
        m_database.close();
    }

    // Создаем новое подключение
    // Используем стандартное имя подключения
    m_database = QSqlDatabase::addDatabase("QSQLITE");
    m_database.setDatabaseName(dbPath);

    // Открываем БД
    if (!m_database.open()) {
        return false;
    }

    // Проверяем наличие таблицы users
    QSqlQuery query(m_database);
    if (!query.exec("SELECT name FROM sqlite_master WHERE type='table' AND name='users'")) {
        m_database.close();
        return false;
    }

    if (!query.next()) {
        m_database.close();
        return false;
    }

    return true;
}

bool Database::isConnected()
{
    return m_database.isOpen() && m_database.isValid();
}

bool Database::checkCredentials(const QString &username, const QString &password,
                                QString &userFullName, QString &userRole, int &userId,
                                QString &userPhone) // Добавили параметр для телефона
{
    if (!isConnected()) {
        return false;
    }

    // ДОБАВИЛИ phone в SELECT
    QSqlQuery query(m_database);
    query.prepare("SELECT id, password, role, full_name, phone FROM users WHERE login = ?");
    query.addBindValue(username);

    if (!query.exec()) {
        return false;
    }

    if (!query.next()) {
        return false;
    }

    // Получаем данные
    userId = query.value(0).toInt();
    QString storedHash = query.value(1).toString();
    userRole = query.value(2).toString();
    userFullName = query.value(3).toString();
    userPhone = query.value(4).toString(); // Получаем телефон

    // Проверка пароля
    QString inputHash = Database::hashPassword(password);

    if (inputHash == storedHash) {
        return true;
    }

    return false;
}

bool Database::isLoginTaken(const QString &login)
{
    if (!isConnected()) {
        return false;
    }

    QSqlQuery query;
    query.prepare("SELECT COUNT(*) FROM users WHERE login = :login");
    query.bindValue(":login", login);

    if (!query.exec()) {
        return true;
    }

    if (query.next()) {
        int count = query.value(0).toInt();
        return count > 0;
    }

    return false;
}

QString Database::hashPassword(const QString &password)
{
    if (password.isEmpty()) {
        return QString();
    }

    QByteArray hash = QCryptographicHash::hash(
        password.toUtf8(),
        QCryptographicHash::Sha256
        );

    QString result = QString(hash.toHex());

    return result;
}


bool Database::registerUser(const QString &login,
                            const QString &password,
                            const QString &fullName,
                            const QString &phone,
                            const QString &role)
{
    if (!isConnected()) {
        return false;
    }

    if (isLoginTaken(login)) {
        return false;
    }

    // Хешируем пароль
    QString hashedPassword = Database::hashPassword(password);

    QString formattedPhone = phone;
    QVariant phoneVariant;

    if (!formattedPhone.isEmpty()) {
        // Удаляем все нецифровые символы
        QString digitsOnly;
        for (const QChar &ch : formattedPhone) {
            if (ch.isDigit()) {
                digitsOnly.append(ch);
            }
        }

        // Проверяем длину (должно быть 11 цифр для российского номера)
        if (digitsOnly.length() != 11) {
            return false;
        }

        // Проверяем, что номер начинается с 7 или 8
        if (!digitsOnly.startsWith("7") && !digitsOnly.startsWith("8")) {
            return false;
        }

        // Проверяем, что после кода страны идет 9
        if (digitsOnly.at(1) != '9') {
            return false;
        }

        // ФОРМАТИРУЕМ: +7 (999) 999-99-99
        // Берем последние 10 цифр (убираем первую 7 или 8)
        QString tenDigits = digitsOnly.mid(1);

        // Форматируем: +7 (XXX) XXX-XX-XX
        formattedPhone = QString("+7 (%1) %2-%3-%4")
                             .arg(tenDigits.left(3))    // первые 3 цифры
                             .arg(tenDigits.mid(3, 3))  // следующие 3
                             .arg(tenDigits.mid(6, 2))  // следующие 2
                             .arg(tenDigits.mid(8, 2)); // последние 2

        phoneVariant = formattedPhone;
    } else {
        phoneVariant = QVariant(QVariant::String);
    }

    QSqlQuery query(m_database);
    query.prepare(
        "INSERT INTO users (login, password, role, full_name, phone, registration_date) "
        "VALUES (?, ?, ?, ?, ?, DATE('now'))"
        );

    query.addBindValue(login);
    query.addBindValue(hashedPassword);
    query.addBindValue(role);
    query.addBindValue(fullName);
    query.addBindValue(phoneVariant);

    if (!query.exec()) {
        return false;
    }
    return true;
}

void Database::closeDatabase()
{
    QSqlDatabase db = QSqlDatabase::database("rental_connection");
    if (db.isOpen()) {
        db.close();
    }
    QSqlDatabase::removeDatabase("rental_connection");
}
