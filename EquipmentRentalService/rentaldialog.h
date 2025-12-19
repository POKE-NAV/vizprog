#ifndef RENTALDIALOG_H
#define RENTALDIALOG_H

#include <QDialog>
#include <QDate>

class QLabel;
class QDateEdit;
class QSpinBox;
class QPushButton;

class RentalDialog : public QDialog
{
    Q_OBJECT

public:
    // Конструктор для новой заявки
    RentalDialog(QWidget *parent, int clientId, int equipmentId);
    ~RentalDialog();

    // Статический метод для создания новой заявки
    static bool createNewRental(QWidget *parent, int clientId, int equipmentId);

private slots:
    void onDateChanged();
    void calculatePrice();
    void onConfirmClicked();
    void onCancelClicked();
    void validateDates();

private:
    void setupUI();
    void loadEquipmentData();
    void loadClientData();
    bool validateInput();
    void updateStatusMessages();
    bool saveRental();
    void showAvailabilityError();

    // Данные
    int m_clientId;
    int m_equipmentId;

    // Кэшированные данные
    double m_dailyPrice;
    double m_depositAmount;
    QString m_equipmentName;
    QString m_clientName;

    // Текущие значения
    QDate m_startDate;
    QDate m_endDate;
    int m_days;
    double m_totalPrice;
    double m_totalWithDeposit;
    double m_discount;

    // Виджеты
    QLabel *m_equipmentInfoLabel;
    QLabel *m_clientInfoLabel;
    QLabel *m_availabilityLabel;
    QLabel *m_statusMessageLabel;

    QDateEdit *m_startDateEdit;
    QDateEdit *m_endDateEdit;
    QSpinBox *m_daysSpinBox;

    QLabel *m_dailyPriceLabel;
    QLabel *m_totalPriceLabel;
    QLabel *m_depositLabel;
    QLabel *m_totalWithDepositLabel;
    QLabel *m_discountLabel;
    QPushButton *m_confirmButton;
    QPushButton *m_cancelButton;

    double m_discountPercent;
    double m_discountedPrice;
    QLabel *m_discountValueLabel;


    double calculateDiscount(int days) const;
    void updateDiscountedPrice();

    // Состояние
    bool m_isAvailable;
};

#endif // RENTALDIALOG_H
