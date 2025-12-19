#ifndef RENTAL_H
#define RENTAL_H

#include <QString>
#include <QDate>
#include <QList>
#include <equipment.h>
#include <user.h>
class Rental {
private:
    int m_id;
    int m_clientId;
    QString m_clientLogin;
    int m_equipmentId;
    QString m_equipmentName;
    QDate m_startDate;
    QDate m_endDate;
    double m_totalPrice;
    double m_deposit;
    bool m_depositReturned;
    QString m_status;
    QString m_applicationStatus;

public:
    // Конструкторы
    Rental();

    Rental(int id, int clientId, const QString& clientLogin,
           int equipmentId, const QString& equipmentName,
           const QDate& startDate, const QDate& endDate,
           double totalPrice, double deposit,
           bool depositReturned, const QString& status,
           const QString& applicationStatus );

    // Статические константы для статуса аренды и заявки
    static const QString STATUS_ACTIVE;
    static const QString STATUS_COMPLETED;
    static const QString STATUS_CANCELED;
    static const QString STATUS_RESERVED;
    static const QString STATUS_IN_PROCESSING ;

    static const QString APPLICATION_STATUS_ACCEPTED ;
    static const QString APPLICATION_STATUS_REJECTED ;
    static const QString APPLICATION_STATUS_IN_PROCESSING ;

    // Геттеры
    int getId()const { return m_id; }
    int getClientId() const { return m_clientId; }
    QString getClientLogin() const { return m_clientLogin; }
    int getEquipmentId() const { return m_equipmentId; }
    QString getEquipmentName() const { return m_equipmentName; }
    QDate getStartDate() const { return m_startDate; }
    QDate getEndDate() const { return m_endDate; }
    double getTotalPrice() const { return m_totalPrice; }
    double getDeposit() const { return m_deposit; }
    bool getDepositReturned() const { return m_depositReturned; }
    QString getStatus() const { return m_status; }
    QString getApplicationStatus() const { return m_applicationStatus; }

    // Форматирование
    QString getFormatStartDate() const { return m_startDate.toString("dd.MM.yyyy"); }
    QString getFormatEndDate() const { return m_endDate.toString("dd.MM.yyyy"); }
    QString getFormatTotalPrice() const { return QString("%1 руб./день").arg(m_totalPrice, 0, 'f', 2); }

    // Сеттеры
    void setClientId(int clientId);
    void setClientLogin(const QString& clientLogin);
    void setEquipmentId(int equipmentId);
    void setEquipmentName(const QString& equipmentName);
    void setStartDate(const QDate& startDate);
    void setEndDate(const QDate& endDate);
    void setTotalPrice(double totalPrice);
    void setDeposit(double deposit);
    void setDepositReturned(bool depositReturned);
    void setStatus(const QString& status);
    void setApplicationStatus(const QString& applicationStatus);

    // Валидация статусов
    static bool validateStatus(const QString& status);
    static bool validateApplicationStatus(const QString& status);

    // Установка определенного статуса
    bool isActive() const;
    bool isCompleted() const;
    bool isCanceled() const;
    bool isReserved() const;
    bool isProcessing() const;
    bool isApplicationAccepted() const;
    bool isApplicationRejected() const;
    bool isApplicationInProcessing() const;

    int getRentalDays() const;
    bool canBeCanceled() const;
    bool isInProgress() const;

    // Получение аренд и заявок для клиента
    static QList<Rental*> findMyRentals(int clientId);
    static QList<Rental*> findMyPendingRentals(int clientId);

    // Фильтрация аренд
    static QList<Rental*> filterByStatus(const QList<Rental*>& rentals, const QString& status);
    static QList<Rental*> filterActiveRentals(const QList<Rental*>& rentals);
    static QList<Rental*> filterCompletedRentals(const QList<Rental*>& rentals);
    static QList<Rental*> filterCanceledRentals(const QList<Rental*>& rentals);
    static QList<Rental*> filterReservedRentals(const QList<Rental*>& rentals);

    // Создание заявки
    bool insertIntoDatabase();

    // Проверка срока аренды и обновление статусов
    static void updateEquipmentStatusAfterRental(Rental* rental);
    static void checkAndUpdateStatus(Rental* rental);
    static bool isEquipmentAvailableForDates(int equipmentId, const QDate& startDate, const QDate& endDate, int excludeRentalId);

    // Получение аренд и заявок для администратора
    static Rental* findById(int id);
    static QList<Rental*> findAllRentals();
    static QList<Rental*> findAllPendingRentals();
    static QList<Rental*> findRentalsByStatus(const QList<Rental*>& rentals, const QString& status);
    static QList<Rental*> findActiveRentals(const QList<Rental*>& rentals);
    static QList<Rental*> findCompletedRentals(const QList<Rental*>& rentals);
    static QList<Rental*> findCanceledRentals(const QList<Rental*>& rentals);
    static QList<Rental*> findReservedRentals(const QList<Rental*>& rentals);
    static QList<Rental*> findRentalsByClientLogin(const QList<Rental*>& rentals, const QString& login);
    static QList<Rental*> findRentalsByEquipment(const QList<Rental*>& rentals, const QString& equipmentName);
    static QList<Rental*> findRentalsByDateRange(const QList<Rental*>& rentals, const QDate& from, const QDate& to);

    // Принятие или отклонение заявки
    static bool acceptApplication(int applicationId);
    static bool rejectApplication(int applicationId);

    static void clearRentalList(QList<Rental*>& rentals);

};

#endif // RENTAL_H
