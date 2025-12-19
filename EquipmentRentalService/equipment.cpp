#include "equipment.h"
#include "database.h"
#include <QSqlQuery>
#include <QSqlError>
#include <QDebug>

// Инициализация статических констант
const QString Equipment::STATUS_AVAILABLE = "Свободно";
const QString Equipment::STATUS_RENTED = "Арендованно";
const QString Equipment::STATUS_MAINTENANCE = "Обслуживается";
const QString Equipment::STATUS_BOOKED = "Забронировано";

// Конструкторы
Equipment::Equipment()
    : m_id(0)
    , m_pricePerDay(0.0)
    , m_deposit(0.0)
    , m_createdDate(QDateTime::currentDateTime())
{
}

Equipment::Equipment(int id, const QString& name, const QString& type,
                     const QString& status, double pricePerDay,
                     const QString& inventoryNumber, double deposit,
                     const QDateTime& createdDate)
    : m_id(id)
    , m_name(name)
    , m_type(type)
    , m_status(status)
    , m_pricePerDay(pricePerDay)
    , m_inventoryNumber(inventoryNumber)
    , m_deposit(deposit)
    , m_createdDate(createdDate)
{
}

// Форматированные значения (для отображения в UI)
QString Equipment::getFormattedPrice() const
{
    return QString("%1 руб./день").arg(m_pricePerDay, 0, 'f', 2);
}

QString Equipment::getFormattedDeposit() const
{
    return QString("%1 руб.").arg(m_deposit, 0, 'f', 2);
}

QString Equipment::getFormattedCreatedDate() const
{
    return m_createdDate.toString("dd.MM.yyyy");
}

// Сеттеры
void Equipment::setName(const QString& name)
{
    if (!name.trimmed().isEmpty()) {
        m_name = name.trimmed();
    }
}

void Equipment::setType(const QString& type)
{
    if (!type.trimmed().isEmpty()) {
        m_type = type.trimmed();
    }
}

void Equipment::setStatus(const QString& status)
{
    if (validateStatus(status)) {
        m_status = status;
    }
}

void Equipment::setPricePerDay(double price)
{
    if (validatePrice(price)) {
        m_pricePerDay = price;
    }
}

void Equipment::setInventoryNumber(const QString& number)
{
    m_inventoryNumber = number.trimmed();
}

void Equipment::setDeposit(double deposit)
{
    if (validateDeposit(deposit)) {
        m_deposit = deposit;
    }
}

// Бизнес-методы
bool Equipment::isAvailable() const
{
    return m_status == STATUS_AVAILABLE;
}

bool Equipment::isRented() const
{
    return m_status == STATUS_RENTED;
}

bool Equipment::isUnderMaintenance() const
{
    return m_status == STATUS_MAINTENANCE;
}

bool Equipment::isBooked() const
{
    return m_status == STATUS_BOOKED;
}

double Equipment::calculateRentalCost(int days) const
{
    if (days <= 0) return 0.0;
    return m_pricePerDay * days;
}

double Equipment::calculateTotalWithDeposit(int days) const
{
    return calculateRentalCost(days) + m_deposit;
}

bool Equipment::canBeRented() const
{
    return isAvailable() && m_pricePerDay > 0;
}

// Проверка уникальности инвентарного номера
bool Equipment::isInventoryNumberUnique() const
{
    if (!Database::isConnected() || m_inventoryNumber.isEmpty()) {
        return false;
    }

    QSqlQuery query;
    query.prepare("SELECT COUNT(*) FROM equipment WHERE inventory_number = :inv_num AND id != :id");
    query.bindValue(":inv_num", m_inventoryNumber);
    query.bindValue(":id", m_id);

    if (!query.exec() || !query.next()) {
        return false;
    }

    return query.value(0).toInt() == 0;
}

// Работа с БД
bool Equipment::insertIntoDatabase()
{
    if (!Database::isConnected()) {
        qDebug() << "База данных не подключена";
        return false;
    }

    // Проверка уникальности инвентарного номера
    if (!isInventoryNumberUnique()) {
        qDebug() << "Инвентарный номер должен быть уникальным";
        return false;
    }

    QSqlQuery query;
    query.prepare(
        "INSERT INTO equipment (name, type, status, price_per_day, "
        "inventory_number, deposit, created_date) "
        "VALUES (:name, :type, :status, :price, :inv_num, :deposit, :created_date)"
        );

    query.bindValue(":name", m_name);
    query.bindValue(":type", m_type);
    query.bindValue(":status", m_status);
    query.bindValue(":price", m_pricePerDay);
    query.bindValue(":inv_num", m_inventoryNumber);
    query.bindValue(":deposit", m_deposit);
    query.bindValue(":created_date", m_createdDate);

    if (!query.exec()) {
        qDebug() << "Ошибка при добавлении оборудования:" << query.lastError().text();
        return false;
    }

    // Получаем сгенерированный ID
    m_id = query.lastInsertId().toInt();
    return true;
}

bool Equipment::updateInDatabase()
{
    if (!Database::isConnected()) {
        qDebug() << "База данных не подключена";
        return false;
    }

    // Проверка уникальности инвентарного номера
    if (!isInventoryNumberUnique()) {
        qDebug() << "Инвентарный номер должен быть уникальным";
        return false;
    }

    QSqlQuery query;
    query.prepare(
        "UPDATE equipment SET "
        "name = :name, "
        "type = :type, "
        "status = :status, "
        "price_per_day = :price, "
        "inventory_number = :inv_num, "
        "deposit = :deposit "
        "WHERE id = :id"
        );

    query.bindValue(":id", m_id);
    query.bindValue(":name", m_name);
    query.bindValue(":type", m_type);
    query.bindValue(":status", m_status);
    query.bindValue(":price", m_pricePerDay);
    query.bindValue(":inv_num", m_inventoryNumber);
    query.bindValue(":deposit", m_deposit);

    if (!query.exec()) {
        return false;
    }

    return query.numRowsAffected() > 0;
}

bool Equipment::save()
{
    if (m_id == 0) {
        return insertIntoDatabase();
    } else {
        return updateInDatabase();
    }
}

bool Equipment::remove()
{
    if (m_id == 0 || !Database::isConnected()) {
        return false;
    }

    // Проверка, не арендовано ли оборудование
    if (isRented()) {
        qDebug() << "Нельзя удалить арендованное оборудование";
        return false;
    }

    QSqlQuery query;
    query.prepare("DELETE FROM equipment WHERE id = :id");
    query.bindValue(":id", m_id);

    if (!query.exec()) {
        qDebug() << "Ошибка при удалении оборудования:" << query.lastError().text();
        return false;
    }

    return true;
}

bool Equipment::updateStatus(const QString& newStatus)
{
    if (!validateStatus(newStatus) || !Database::isConnected()) {
        return false;
    }

    QSqlQuery query;
    query.prepare("UPDATE equipment SET status = :status WHERE id = :id");
    query.bindValue(":status", newStatus);
    query.bindValue(":id", m_id);

    if (!query.exec()) {
        qDebug() << "Ошибка при обновлении статуса:" << query.lastError().text();
        return false;
    }

    m_status = newStatus;
    return true;
}

// Статические методы
Equipment* Equipment::findById(int id)
{
    if (!Database::isConnected()) {
        return nullptr;
    }

    QSqlQuery query;
    query.prepare("SELECT * FROM equipment WHERE id = :id");
    query.bindValue(":id", id);

    if (!query.exec() || !query.next()) {
        return nullptr;
    }

    Equipment* equipment = new Equipment(
        query.value("id").toInt(),
        query.value("name").toString(),
        query.value("type").toString(),
        query.value("status").toString(),
        query.value("price_per_day").toDouble(),
        query.value("inventory_number").toString(),
        query.value("deposit").toDouble(),
        query.value("created_date").toDateTime()
        );

    return equipment;
}

QList<Equipment*> Equipment::findAll()
{
    QList<Equipment*> result;

    if (!Database::isConnected()) {
        return result;
    }

    QSqlQuery query("SELECT * FROM equipment ORDER BY name");

    while (query.next()) {
        Equipment* equipment = new Equipment(
            query.value("id").toInt(),
            query.value("name").toString(),
            query.value("type").toString(),
            query.value("status").toString(),
            query.value("price_per_day").toDouble(),
            query.value("inventory_number").toString(),
            query.value("deposit").toDouble(),
            query.value("created_date").toDateTime()
            );
        result.append(equipment);
    }

    return result;
}

QList<Equipment*> Equipment::findByStatus(const QString& status)
{
    QList<Equipment*> result;

    if (!validateStatus(status) || !Database::isConnected()) {
        return result;
    }

    QSqlQuery query;
    query.prepare("SELECT * FROM equipment WHERE status = :status ORDER BY name");
    query.bindValue(":status", status);

    if (!query.exec()) {
        return result;
    }

    while (query.next()) {
        Equipment* equipment = new Equipment(
            query.value("id").toInt(),
            query.value("name").toString(),
            query.value("type").toString(),
            query.value("status").toString(),
            query.value("price_per_day").toDouble(),
            query.value("inventory_number").toString(),
            query.value("deposit").toDouble(),
            query.value("created_date").toDateTime()
            );
        result.append(equipment);
    }

    return result;
}

QList<Equipment*> Equipment::findByType(const QString& type)
{
    QList<Equipment*> result;

    if (!Database::isConnected()) {
        return result;
    }

    QSqlQuery query;
    query.prepare("SELECT * FROM equipment WHERE type = :type ORDER BY name");
    query.bindValue(":type", type);

    if (!query.exec()) {
        return result;
    }

    while (query.next()) {
        Equipment* equipment = new Equipment(
            query.value("id").toInt(),
            query.value("name").toString(),
            query.value("type").toString(),
            query.value("status").toString(),
            query.value("price_per_day").toDouble(),
            query.value("inventory_number").toString(),
            query.value("deposit").toDouble(),
            query.value("created_date").toDateTime()
            );
        result.append(equipment);
    }

    return result;
}

QList<Equipment*> Equipment::findByTypeAndStatus(const QString& type, const QString& status)
{
    QList<Equipment*> result;

    if (!Database::isConnected() || !validateStatus(status)) {
        return result;
    }

    QSqlQuery query;
    query.prepare("SELECT * FROM equipment WHERE type = :type AND status = :status ORDER BY name");
    query.bindValue(":type", type);
    query.bindValue(":status", status);

    if (!query.exec()) {
        return result;
    }

    while (query.next()) {
        Equipment* equipment = new Equipment(
            query.value("id").toInt(),
            query.value("name").toString(),
            query.value("type").toString(),
            query.value("status").toString(),
            query.value("price_per_day").toDouble(),
            query.value("inventory_number").toString(),
            query.value("deposit").toDouble(),
            query.value("created_date").toDateTime()
            );
        result.append(equipment);
    }

    return result;
}

QList<Equipment*> Equipment::findAvailable()
{
    return findByStatus(STATUS_AVAILABLE);
}

QList<Equipment*> Equipment::findRented()
{
    return findByStatus(STATUS_RENTED);
}

QList<Equipment*> Equipment::findUnderMaintenance()
{
    return findByStatus(STATUS_MAINTENANCE);
}

QList<QString> Equipment::getAllTypes()
{
    QList<QString> types;

    if (!Database::isConnected()) {
        return types;
    }

    QSqlQuery query("SELECT DISTINCT type FROM equipment ORDER BY type");

    while (query.next()) {
        types.append(query.value(0).toString());
    }

    return types;
}

// Статистика
int Equipment::getTotalCount()
{
    if (!Database::isConnected()) {
        return 0;
    }

    QSqlQuery query("SELECT COUNT(*) FROM equipment");

    if (query.exec() && query.next()) {
        return query.value(0).toInt();
    }

    return 0;
}

int Equipment::getAvailableCount()
{
    if (!Database::isConnected()) {
        return 0;
    }

    QSqlQuery query;
    query.prepare("SELECT COUNT(*) FROM equipment WHERE status = ?");
    query.addBindValue(STATUS_AVAILABLE);

    if (query.exec() && query.next()) {
        return query.value(0).toInt();
    }

    return 0;
}

int Equipment::getRentedCount()
{
    if (!Database::isConnected()) {
        return 0;
    }

    QSqlQuery query;
    query.prepare("SELECT COUNT(*) FROM equipment WHERE status = ?");
    query.addBindValue(STATUS_RENTED);

    if (query.exec() && query.next()) {
        return query.value(0).toInt();
    }

    return 0;
}

int Equipment::getUnderMaintenanceCount()
{
    if (!Database::isConnected()) {
        return 0;
    }

    QSqlQuery query;
    query.prepare("SELECT COUNT(*) FROM equipment WHERE status = ?");
    query.addBindValue(STATUS_MAINTENANCE);

    if (query.exec() && query.next()) {
        return query.value(0).toInt();
    }

    return 0;
}

// Валидация
bool Equipment::validateStatus(const QString& status)
{
    return status == STATUS_AVAILABLE ||
           status == STATUS_RENTED ||
           status == STATUS_MAINTENANCE ||
           status == STATUS_BOOKED;
}

bool Equipment::validatePrice(double price)
{
    return price > 0;
}

bool Equipment::validateDeposit(double deposit)
{
    return deposit >= 0;
}

bool Equipment::validateInventoryNumber(const QString& number, int excludeId)
{
    if (number.trimmed().isEmpty()) {
        return false;
    }

    if (!Database::isConnected()) {
        return true; // Не можем проверить, но формально валидно
    }

    QSqlQuery query;
    if (excludeId > 0) {
        query.prepare("SELECT COUNT(*) FROM equipment WHERE inventory_number = :inv_num AND id != :id");
        query.bindValue(":inv_num", number.trimmed());
        query.bindValue(":id", excludeId);
    } else {
        query.prepare("SELECT COUNT(*) FROM equipment WHERE inventory_number = :inv_num");
        query.bindValue(":inv_num", number.trimmed());
    }

    if (!query.exec() || !query.next()) {
        return false;
    }

    return query.value(0).toInt() == 0;
}

// Утилита для очистки памяти
void Equipment::clearEquipmentList(QList<Equipment*>& list)
{
    qDeleteAll(list);
    list.clear();
}
