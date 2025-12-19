#include "rental.h"
#include "equipment.h"
#include "user.h"
#include <QSqlDatabase>
#include <QSqlQuery>
#include <QSqlError>
#include <QDebug>
#include <cmath>
#include <database.h>
#include "adminwindow.h"
#include <QMessageBox>
const QString Rental::STATUS_ACTIVE = "Активный";
const QString Rental::STATUS_COMPLETED = "Завершенный";
const QString Rental::STATUS_CANCELED = "Отмененный";
const QString Rental::STATUS_RESERVED = "Зарезервированный";
const QString Rental::STATUS_IN_PROCESSING  = "В обработке";

const QString Rental::APPLICATION_STATUS_ACCEPTED  = "Принят";
const QString Rental::APPLICATION_STATUS_REJECTED  = "Отклонено";
const QString Rental::APPLICATION_STATUS_IN_PROCESSING  = "В обработке";

Rental::Rental()
    : m_id(0)
    , m_clientId(0)
    , m_clientLogin("")
    , m_equipmentId(0)
    , m_equipmentName("")
    , m_startDate(QDate::currentDate())
    , m_endDate(QDate::currentDate())
    , m_totalPrice(0.0)
    , m_deposit(0.0)
    , m_depositReturned(false)
    , m_status("")
    , m_applicationStatus("") {}


Rental::Rental(int id, int clientId, const QString& clientLogin,
           int equipmentId, const QString& equipmentName,
           const QDate& startDate, const QDate& endDate,
           double totalPrice, double deposit,
           bool depositReturned, const QString& status,
           const QString& applicationStatus )
    : m_id(id)
    , m_clientId(clientId)
    , m_clientLogin(clientLogin)
    , m_equipmentId(equipmentId)
    , m_equipmentName(equipmentName)
    , m_startDate(startDate)
    , m_endDate(endDate)
    , m_totalPrice(totalPrice)
    , m_deposit(deposit)
    , m_depositReturned(depositReturned)
    , m_status(status)
    , m_applicationStatus(applicationStatus) {}

void Rental::setClientId(int clientId)
{
    m_clientId = clientId;
}

void Rental::setClientLogin(const QString& clientLogin)
{
    if(!clientLogin.trimmed().isEmpty())
        m_clientLogin = clientLogin;
}

void Rental::setEquipmentId(int equipmentId)
{
    m_equipmentId = equipmentId;
}

void Rental::setEquipmentName(const QString& equipmentName)
{
    if(!equipmentName.trimmed().isEmpty())
        m_equipmentName = equipmentName;
}

void Rental::setStartDate(const QDate& startDate)
{
    m_startDate = startDate;
}

void Rental::setEndDate(const QDate& endDate)
{
    m_endDate = endDate;
}

void Rental::setTotalPrice(double totalPrice)
{
    m_totalPrice = totalPrice;
}

void Rental::setDeposit(double deposit)
{
    m_deposit = deposit;
}

void Rental::setDepositReturned(bool depositReturned)
{
    m_depositReturned = depositReturned;
}

void Rental::setStatus(const QString& status)
{
    if(validateStatus(status))
        m_status = status;
}

void Rental::setApplicationStatus(const QString& applicationStatus)
{
    if(validateApplicationStatus(applicationStatus))
        m_applicationStatus = applicationStatus;
}

bool Rental::validateStatus(const QString& status)
{
    return status == STATUS_ACTIVE ||
           status == STATUS_COMPLETED ||
           status == STATUS_CANCELED ||
           status == STATUS_RESERVED ||
           status == STATUS_IN_PROCESSING;
}
bool Rental::validateApplicationStatus(const QString& status)
{
    return status == APPLICATION_STATUS_ACCEPTED ||
           status == APPLICATION_STATUS_REJECTED ||
           status == APPLICATION_STATUS_IN_PROCESSING;
}

bool Rental::isActive() const
{
    return m_status ==  STATUS_ACTIVE;
}

bool Rental::isCompleted() const
{
    return m_status == STATUS_COMPLETED;
}

bool Rental::isCanceled() const
{
    return m_status == STATUS_CANCELED;
}

bool Rental::isReserved() const
{
    return m_status == STATUS_RESERVED;
}

bool Rental::isProcessing() const
{
    return m_status == STATUS_IN_PROCESSING;
}

bool Rental::isApplicationAccepted() const
{
    return m_applicationStatus == APPLICATION_STATUS_ACCEPTED;
}

bool Rental::isApplicationRejected() const
{
    return m_applicationStatus == APPLICATION_STATUS_REJECTED;
}

bool Rental::isApplicationInProcessing() const
{
    return m_applicationStatus == APPLICATION_STATUS_IN_PROCESSING;
}

int Rental::getRentalDays() const
{
    return m_startDate.daysTo(m_endDate);
}

bool Rental::canBeCanceled() const
{
    QDate currentDate = QDate::currentDate();
    return m_startDate > currentDate &&
           m_status != STATUS_CANCELED &&
           m_status != STATUS_COMPLETED &&
           m_applicationStatus != APPLICATION_STATUS_REJECTED;
}

bool Rental::isInProgress() const
{
    QDate currentDate = QDate::currentDate();
    return currentDate >= m_startDate &&
           currentDate <= m_endDate &&
           m_status == STATUS_ACTIVE &&
           m_applicationStatus == APPLICATION_STATUS_ACCEPTED;
}

bool Rental::isEquipmentAvailableForDates(int equipmentId, const QDate& startDate, const QDate& endDate, int excludeRentalId)
{
    if (!Database::isConnected()) {
        return false;
    }

    if (equipmentId <= 0 || !startDate.isValid() || !endDate.isValid() || startDate >= endDate) {
        return false;
    }
    Equipment* equipment = Equipment::findById(equipmentId);
    if(!equipment)
    {
        return false;
    }else
    {
        if(equipment->getStatus() == Equipment::STATUS_MAINTENANCE){
            qCritical() << "Оборудование на тех. обслуживании:";
            return false;
        }
    }
    QSqlQuery query;

    query.prepare(
        "SELECT COUNT(*) FROM rentals "
        "WHERE equipment_id = :equipment_id "
        "AND id != :exclude_id "
        "AND status IN (:status_active, :status_reserved) "
        "AND NOT (end_date < :start_date OR start_date > :end_date)"
        );

    query.bindValue(":equipment_id", equipmentId);
    query.bindValue(":exclude_id", excludeRentalId);
    query.bindValue(":status_active", STATUS_ACTIVE);
    query.bindValue(":status_reserved", STATUS_RESERVED);
    query.bindValue(":start_date", startDate);
    query.bindValue(":end_date", endDate);

    if (!query.exec()) {
        qCritical() << "Ошибка проверки доступности оборудования:" << query.lastError().text();
        return false;
    }

    if (query.next()) {
        int conflictCount = query.value(0).toInt();
        return conflictCount == 0;
    }

    return false;
}

void Rental::checkAndUpdateStatus(Rental* rental)
{
    if (!rental) return;

    QDate currentDate = QDate::currentDate();

    if (rental->isActive() && rental->getEndDate() < currentDate) {
        rental->setStatus(STATUS_COMPLETED);

        QSqlQuery query;
        query.prepare("UPDATE rentals SET status = :status WHERE id = :id");
        query.bindValue(":id", rental->getId());
        query.bindValue(":status", STATUS_COMPLETED);

        if (query.exec()) {
            updateEquipmentStatusAfterRental(rental);
        } else {
            qWarning() << "Не удалось обновить статус аренды:" << query.lastError().text();
        }
    }
}

void Rental::updateEquipmentStatusAfterRental(Rental* rental)
{
    Equipment* equipment = Equipment::findById(rental->getEquipmentId());
    if (equipment) {
        equipment->setStatus(Equipment::STATUS_AVAILABLE);
        equipment->save();
        delete equipment;
    }
}

bool Rental::insertIntoDatabase()
{
    if (!Database::isConnected()) {
        qDebug() << "База данных не подключена";
        return false;
    }

    if (m_clientId <= 0 || m_equipmentId <= 0 || !m_startDate.isValid() || !m_endDate.isValid()) {
        qDebug() << "Недостаточно данных для создания заявки";
        return false;
    }

    if (m_startDate >= m_endDate) {
        qDebug() << "Дата начала должна быть раньше даты окончания";
        return false;
    }

    if (!isEquipmentAvailableForDates(m_equipmentId, m_startDate, m_endDate, 0)) {
        return false;
    }

    QSqlQuery query;
    query.prepare(
        "INSERT INTO rentals (client_id, client_login, equipment_id, equipment_name, "
        "start_date, end_date, total_price, deposit, deposit_returned, "
        "status, application_status) "
        "VALUES (:client_id, :client_login, :equipment_id, :equipment_name, "
        ":start_date, :end_date, :total_price, :deposit, :deposit_returned, "
        ":status, :application_status)"
        );

    query.bindValue(":client_id", m_clientId);
    query.bindValue(":client_login", m_clientLogin);
    query.bindValue(":equipment_id", m_equipmentId);
    query.bindValue(":equipment_name", m_equipmentName);
    query.bindValue(":start_date", m_startDate);
    query.bindValue(":end_date", m_endDate);
    query.bindValue(":total_price", m_totalPrice);
    query.bindValue(":deposit", m_deposit);
    query.bindValue(":deposit_returned", m_depositReturned);
    query.bindValue(":status", m_status);
    query.bindValue(":application_status", m_applicationStatus);

    if (!query.exec()) {
        qDebug() << "Ошибка при создании заявки:" << query.lastError().text();
        return false;
    }

    m_id = query.lastInsertId().toInt();
    qDebug() << "Заявка создана успешно, ID:" << m_id;

    return true;
}

QList<Rental*> Rental::findMyRentals(int clientId)
{
    QList<Rental*> result;

    if (!Database::isConnected()) {
        return result;
    }

    if (clientId <= 0) {
        qWarning() << "Неверный ID клиента:" << clientId;
        return result;
    }

    QSqlQuery query;
    query.prepare(
        "SELECT * FROM rentals WHERE client_id = :client_id AND status != :status AND application_status != :app_status ORDER BY start_date DESC"
        );
    query.bindValue(":client_id", clientId);
    query.bindValue(":status", STATUS_IN_PROCESSING);
    query.bindValue(":app_status", APPLICATION_STATUS_IN_PROCESSING);

    if (!query.exec()) {
        qCritical() << "Ошибка поиска аренд клиента:" << query.lastError().text();
        return result;
    }

    while (query.next()) {
        Rental* rental = new Rental(
            query.value("id").toInt(),
            query.value("client_id").toInt(),
            query.value("client_login").toString(),
            query.value("equipment_id").toInt(),
            query.value("equipment_name").toString(),
            query.value("start_date").toDate(),
            query.value("end_date").toDate(),
            query.value("total_price").toDouble(),
            query.value("deposit").toDouble(),
            query.value("deposit_returned").toBool(),
            query.value("status").toString(),
            query.value("application_status").toString()
            );

        if (rental) {
            checkAndUpdateStatus(rental);
            result.append(rental);
        }
    }

    return result;
}

QList<Rental*> Rental::findMyPendingRentals(int clientId)
{
    QList<Rental*> result;

    if (!Database::isConnected()) {
        return result;
    }

    if (clientId <= 0) {
        qWarning() << "Неверный ID клиента:" << clientId;
        return result;
    }

    QSqlQuery query;
    query.prepare("SELECT * FROM rentals WHERE client_id = :client_id AND status = :status AND application_status = :app_status ORDER BY start_date DESC");
    query.bindValue(":client_id", clientId);
    query.bindValue(":status", STATUS_IN_PROCESSING);
    query.bindValue(":app_status", APPLICATION_STATUS_IN_PROCESSING);

    if(!query.exec()){
        qCritical() << "Ошибка поиска заявок клиента:" << query.lastError().text();
        return result;
    }

    while(query.next())
    {
        Rental* rental = new Rental(
            query.value("id").toInt(),
            query.value("client_id").toInt(),
            query.value("client_login").toString(),
            query.value("equipment_id").toInt(),
            query.value("equipment_name").toString(),
            query.value("start_date").toDate(),
            query.value("end_date").toDate(),
            query.value("total_price").toDouble(),
            query.value("deposit").toDouble(),
            query.value("deposit_returned").toBool(),
            query.value("status").toString(),
            query.value("application_status").toString()
            );

        if (rental && rental->isProcessing()) {
            checkAndUpdateStatus(rental);
            result.append(rental);
        } else {
            delete rental;
        }
    }

    return result;
}


QList<Rental*> Rental::filterByStatus(const QList<Rental*>& rentals, const QString& status)
{
    QList<Rental*> filtered;

    if (!validateStatus(status)) {
        qWarning() << "Попытка фильтрации по невалидному статусу:" << status;
        return filtered;
    }

    for (Rental* rental : rentals) {
            filtered.append(rental);
    }

    qDebug() << "Отфильтровано по статусу" << status << ":"
             << filtered.size() << "из" << rentals.size();
    return filtered;
}


QList<Rental*> Rental::filterActiveRentals(const QList<Rental*>& rentals)
{
    return filterByStatus(rentals, STATUS_ACTIVE);
}


QList<Rental*> Rental::filterCompletedRentals(const QList<Rental*>& rentals)
{
    return filterByStatus(rentals, STATUS_COMPLETED);
}


QList<Rental*> Rental::filterCanceledRentals(const QList<Rental*>& rentals)
{
    return filterByStatus(rentals, STATUS_CANCELED);
}


QList<Rental*> Rental::filterReservedRentals(const QList<Rental*>& rentals)
{
    return filterByStatus(rentals, STATUS_RESERVED);
}

QList<Rental*> Rental::findAllRentals()
{
    QList<Rental*> result;

    if (!Database::isConnected()) {
        return result;
    }

    QSqlQuery query;
    query.prepare(
        "SELECT * FROM rentals WHERE status != :status AND application_status != :app_status ORDER BY start_date DESC"
        );
    query.bindValue(":status", STATUS_IN_PROCESSING);
    query.bindValue(":app_status", APPLICATION_STATUS_IN_PROCESSING);

    if (!query.exec()) {
        qCritical() << "Ошибка поиска аренд клиентов:" << query.lastError().text();
        return result;
    }

    while (query.next()) {
        Rental* rental = new Rental(
            query.value("id").toInt(),
            query.value("client_id").toInt(),
            query.value("client_login").toString(),
            query.value("equipment_id").toInt(),
            query.value("equipment_name").toString(),
            query.value("start_date").toDate(),
            query.value("end_date").toDate(),
            query.value("total_price").toDouble(),
            query.value("deposit").toDouble(),
            query.value("deposit_returned").toBool(),
            query.value("status").toString(),
            query.value("application_status").toString()
            );

        if (rental) {
            checkAndUpdateStatus(rental);
            result.append(rental);
        }
    }

    return result;
}

QList<Rental*> Rental::findAllPendingRentals()
{
    QList<Rental*> result;

    if (!Database::isConnected()) {
        return result;
    }

    QSqlQuery query;
    query.prepare("SELECT * FROM rentals WHERE status = :status AND application_status = :app_status ORDER BY start_date DESC");
    query.bindValue(":status", STATUS_IN_PROCESSING);
    query.bindValue(":app_status", APPLICATION_STATUS_IN_PROCESSING);

    if(!query.exec()){
        qCritical() << "Ошибка поиска заявок клиентов:" << query.lastError().text();
        return result;
    }

    while(query.next())
    {
        Rental* rental = new Rental(
            query.value("id").toInt(),
            query.value("client_id").toInt(),
            query.value("client_login").toString(),
            query.value("equipment_id").toInt(),
            query.value("equipment_name").toString(),
            query.value("start_date").toDate(),
            query.value("end_date").toDate(),
            query.value("total_price").toDouble(),
            query.value("deposit").toDouble(),
            query.value("deposit_returned").toBool(),
            query.value("status").toString(),
            query.value("application_status").toString()
            );

        if (rental && rental->isProcessing()) {
            checkAndUpdateStatus(rental);
            result.append(rental);
        } else {
            delete rental;
        }
    }

    return result;
}

QList<Rental*> Rental::findRentalsByStatus(const QList<Rental*>& rentals, const QString& status)
{
    QList<Rental*> filtered;

    if (!Rental::validateStatus(status)) {
        qWarning() << "Попытка фильтрации по невалидному статусу:" << status;
        return filtered;
    }

    for (Rental* rental : rentals) {
        if (rental && rental->getStatus() == status) {
            filtered.append(rental);
        }
    }

    qDebug() << "Отфильтровано по статусу" << status << ":"
             << filtered.size() << "из" << rentals.size();
    return filtered;
}

QList<Rental*> Rental::findActiveRentals(const QList<Rental*>& rentals)
{
    return findRentalsByStatus(rentals, STATUS_ACTIVE);
}


QList<Rental*> Rental::findCompletedRentals(const QList<Rental*>& rentals)
{
    return findRentalsByStatus(rentals, STATUS_COMPLETED);
}


QList<Rental*> Rental::findCanceledRentals(const QList<Rental*>& rentals)
{
    return findRentalsByStatus(rentals, STATUS_CANCELED);
}


QList<Rental*> Rental::findReservedRentals(const QList<Rental*>& rentals)
{
    return findRentalsByStatus(rentals, STATUS_RESERVED);
}

QList<Rental*> findRentalsByClientLogin(const QList<Rental*>& rentals, const QString& login)
{
    QList<Rental*> filtered;

    for (Rental* rental : rentals) {
        if (rental && rental->getClientLogin() == login) {
            filtered.append(rental);
        }
    }

    return filtered;
}
QList<Rental*> findRentalsByEquipment(const QList<Rental*>& rentals, const QString& equipmentName)
{
    QList<Rental*> filtered;

    for (Rental* rental : rentals) {
        if (rental && rental->getEquipmentName() == equipmentName) {
            filtered.append(rental);
        }
    }

    return filtered;
}
QList<Rental*> findRentalsByDateRange(const QList<Rental*>& rentals, const QDate& from, const QDate& to)
{
    QList<Rental*> filtered;

    for (Rental* rental : rentals) {
        if (rental && rental->getStartDate().isValid() && rental->getEndDate().isValid() ) {
            filtered.append(rental);
        }
    }

    return filtered;
}

Rental* Rental::findById(int id)
{
    if (id <= 0) {
        return nullptr;
    }

    QSqlQuery query;
    query.prepare(
        "SELECT id, client_id, client_login, equipment_id, equipment_name, "
        "start_date, end_date, total_price, deposit, deposit_returned, "
        "status, application_status "
        "FROM rentals WHERE id = :id"
        );
    query.bindValue(":id", id);

    if (!query.exec()) {
        qCritical() << "[Rental] Ошибка поиска аренды по ID:" << query.lastError().text();
        return nullptr;
    }

    if (query.next()) {
        return new Rental(
            query.value("id").toInt(),
            query.value("client_id").toInt(),
            query.value("client_login").toString(),
            query.value("equipment_id").toInt(),
            query.value("equipment_name").toString(),
            query.value("start_date").toDate(),
            query.value("end_date").toDate(),
            query.value("total_price").toDouble(),
            query.value("deposit").toDouble(),
            query.value("deposit_returned").toBool(),
            query.value("status").toString(),
            query.value("application_status").toString()
            );
    }

    return nullptr;
}

bool Rental::acceptApplication(int applicationId)
{
    // 1. Получаем заявку из базы
    Rental* rental = findById(applicationId);
    if (!rental) {
        qWarning() << "[Rental] Заявка с ID" << applicationId << "не найдена";
        return false;
    }

    // 2. Проверяем, что это заявка (статус "В обработке")
    if (!rental->isProcessing()) {
        qWarning() << "[Rental] ID" << applicationId
                   << "не является заявкой. Статус:" << rental->getStatus();
        delete rental;
        return false;
    }

    // 3. Проверяем, что заявка еще в обработке
    if (!rental->isApplicationInProcessing()) {
        qWarning() << "[Rental] Заявка" << applicationId
                   << "уже обработана. Статус:" << rental->getApplicationStatus();
        delete rental;
        return false;
    }

    // 4. Проверяем доступность оборудования
    if (!isEquipmentAvailableForDates(rental->getEquipmentId(),
                                      rental->getStartDate(),
                                      rental->getEndDate(),
                                      applicationId)) {
        // qWarning() << "[Rental] Оборудование недоступно в указанные даты";
        QMessageBox::warning(nullptr,
                             "Ошибка создания заявки",
                             "Оборудование недоступно в указанные даты.\n"
                             "Пожалуйста, выберите другой период или оборудование.");
        delete rental;
        return false;
    }

    // 5. Определяем финальный статус аренды
    QDate currentDate = QDate::currentDate();
    QString finalStatus;

    if (currentDate >= rental->getStartDate() && currentDate <= rental->getEndDate()) {
        finalStatus = STATUS_ACTIVE;
    } else if (currentDate < rental->getStartDate()) {
        finalStatus = STATUS_RESERVED;
    } else {
        qWarning() << "[Rental] Дата начала аренды уже прошла:"
                   << rental->getStartDate().toString("dd.MM.yyyy");
        delete rental;
        return false;
    }

    // 6. Начинаем транзакцию
    QSqlDatabase db = QSqlDatabase::database();
    if (!db.transaction()) {
        qCritical() << "[Rental] Не удалось начать транзакцию";
        delete rental;
        return false;
    }

    bool success = false;

    try {
        // 7. Обновляем статус заявки в базе
        QSqlQuery query;
        query.prepare(
            "UPDATE rentals SET "
            "application_status = :app_status, "
            "status = :status "
            "WHERE id = :id"
            );
        query.bindValue(":app_status", APPLICATION_STATUS_ACCEPTED);
        query.bindValue(":status", finalStatus);
        query.bindValue(":id", applicationId);

        if (!query.exec()) {
            qCritical() << "[Rental] Ошибка обновления статуса заявки:" << query.lastError().text();
            db.rollback();
            delete rental;
            return false;
        }

        // 8. Обновляем статус оборудования
        Equipment* equipment = Equipment::findById(rental->getEquipmentId());
        if (equipment) {
            if (finalStatus == STATUS_ACTIVE) {
                equipment->setStatus(Equipment::STATUS_RENTED);
            } else if (finalStatus == STATUS_RESERVED) {
                equipment->setStatus(Equipment::STATUS_BOOKED);
            }

            if (!equipment->save()) {
                qWarning() << "[Rental] Не удалось обновить статус оборудования";
                delete equipment;
                db.rollback();
                delete rental;
                return false;
            }
            delete equipment;
        }

        // 9. Фиксируем транзакцию
        if (!db.commit()) {
            qCritical() << "[Rental] Не удалось зафиксировать транзакцию";
            db.rollback();
            delete rental;
            return false;
        }

        success = true;
        qDebug() << "[Rental] Заявка" << applicationId << "принята. Новый статус:" << finalStatus;

    } catch (const std::exception& e) {
        qCritical() << "[Rental] Исключение при принятии заявки:" << e.what();
        db.rollback();
        success = false;
    } catch (...) {
        qCritical() << "[Rental] Неизвестная ошибка при принятии заявки";
        db.rollback();
        success = false;
    }
    delete rental;
    return success;
}

bool Rental::rejectApplication(int applicationId)
{
    // 1. Получаем заявку из базы
    Rental* rental = findById(applicationId);
    if (!rental) {
        qWarning() << "[Rental] Заявка с ID" << applicationId << "не найдена";
        return false;
    }

    // 2. Проверяем, что это заявка
    if (!rental->isProcessing()) {
        qWarning() << "[Rental] ID" << applicationId
                   << "не является заявкой. Статус:" << rental->getStatus();
        delete rental;
        return false;
    }

    // 3. Проверяем, что заявка еще в обработке
    if (!rental->isApplicationInProcessing()) {
        qWarning() << "[Rental] Заявка" << applicationId
                   << "уже обработана. Статус:" << rental->getApplicationStatus();
        delete rental;
        return false;
    }

    // 4. Начинаем транзакцию
    QSqlDatabase db = QSqlDatabase::database();
    if (!db.transaction()) {
        qCritical() << "[Rental] Не удалось начать транзакцию";
        delete rental;
        return false;
    }

    bool success = false;

    try {
        // 5. Обновляем статусы в базе
        QSqlQuery query;
        query.prepare(
            "UPDATE rentals SET "
            "application_status = :app_status, "
            "status = :status "
            "WHERE id = :id"
            );
        query.bindValue(":app_status", APPLICATION_STATUS_REJECTED);
        query.bindValue(":status", STATUS_CANCELED);
        query.bindValue(":id", applicationId);

        if (!query.exec()) {
            qCritical() << "[Rental] Ошибка отклонения заявки:" << query.lastError().text();
            db.rollback();
            delete rental;
            return false;
        }

        // 7. Фиксируем транзакцию
        if (!db.commit()) {
            qCritical() << "[Rental] Не удалось зафиксировать транзакцию";
            db.rollback();
            delete rental;
            return false;
        }

        success = true;
        qDebug() << "[Rental] Заявка" << applicationId << "отклонена.";

    } catch (const std::exception& e) {
        qCritical() << "[Rental] Исключение при отклонении заявки:" << e.what();
        db.rollback();
        success = false;
    } catch (...) {
        qCritical() << "[Rental] Неизвестная ошибка при отклонении заявки";
        db.rollback();
        success = false;
    }

    delete rental;
    return success;
}

void Rental::clearRentalList(QList<Rental*>& rentals)
{
    qDeleteAll(rentals);
    rentals.clear();
}
