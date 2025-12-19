#ifndef EQUIPMENT_H
#define EQUIPMENT_H

#include <QString>
#include <QDateTime>
#include <QList>

class Equipment
{
private:
    int m_id;
    QString m_name;
    QString m_type;
    QString m_status;
    double m_pricePerDay;
    QString m_inventoryNumber;
    QDateTime m_createdDate;
    double m_deposit;

    // Вспомогательные методы для работы с БД
    bool insertIntoDatabase();
    bool updateInDatabase();

    // Проверка уникальности инвентарного номера
    bool isInventoryNumberUnique() const;

public:
    // Статические константы для статусов
    static const QString STATUS_AVAILABLE;
    static const QString STATUS_RENTED;
    static const QString STATUS_MAINTENANCE;
    static const QString STATUS_BOOKED;

    // Конструкторы
    Equipment();
    Equipment(int id, const QString& name, const QString& type,
              const QString& status, double pricePerDay,
              const QString& inventoryNumber, double deposit = 0.0,
              const QDateTime& createdDate = QDateTime::currentDateTime());

    // Геттеры (inline для производительности)
    int getId() const { return m_id; }
    QString getName() const { return m_name; }
    QString getType() const { return m_type; }
    QString getStatus() const { return m_status; }
    double getPricePerDay() const { return m_pricePerDay; }
    QString getInventoryNumber() const { return m_inventoryNumber; }
    QDateTime getCreatedDate() const { return m_createdDate; }
    double getDeposit() const { return m_deposit; }

    QString getFormattedPrice() const;
    QString getFormattedDeposit() const;
    QString getFormattedCreatedDate() const;

    // Сеттеры с базовой валидацией
    void setName(const QString& name);
    void setType(const QString& type);
    void setStatus(const QString& status);
    void setPricePerDay(double price);
    void setInventoryNumber(const QString& number);
    void setDeposit(double deposit);

    bool isAvailable() const;
    bool isRented() const;
    bool isUnderMaintenance() const;
    bool isBooked() const;

    double calculateRentalCost(int days) const;
    double calculateTotalWithDeposit(int days) const;
    bool canBeRented() const;

    // Работа с БД
    bool save();
    bool remove();
    bool updateStatus(const QString& newStatus);

    // Статические методы для работы с БД
    static Equipment* findById(int id);
    static QList<Equipment*> findAll();
    static QList<Equipment*> findByStatus(const QString& status);
    static QList<Equipment*> findByType(const QString& type);
    static QList<Equipment*> findByTypeAndStatus(const QString& type, const QString& status);
    static QList<Equipment*> findAvailable();
    static QList<Equipment*> findRented();
    static QList<Equipment*> findUnderMaintenance();
    static QList<QString> getAllTypes();

    // Статистика
    static int getTotalCount();
    static int getAvailableCount();
    static int getRentedCount();
    static int getUnderMaintenanceCount();

    // Валидация
    static bool validateStatus(const QString& status);
    static bool validatePrice(double price);
    static bool validateDeposit(double deposit);
    static bool validateInventoryNumber(const QString& number, int excludeId = 0);

    // Утилиты для освобождения памяти
    static void clearEquipmentList(QList<Equipment*>& list);

};

#endif // EQUIPMENT_H
