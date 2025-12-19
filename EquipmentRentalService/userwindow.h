#ifndef USERWINDOW_H
#define USERWINDOW_H

#include "mainwindow.h"
#include "QSqlDatabase"
#include "rental.h"

class UserWindow: public MainWindow {
    Q_OBJECT
public:
    explicit UserWindow(User* user, QWidget* parent = nullptr);
    ~UserWindow() override;
    int activeRentalsCount;
public:
    void refreshDashboard();
    void refreshRentalsData();

protected slots:

    // Слоты для вкладки оборудования
    void onEquipmentSearchTextChanged(const QString &text) override;
    void onEquipmentStatusFilterChanged(int index) override;
    void onEquipmentTypeFilterChanged(int index) override;
    void onAddNewEquipmentClicked() override;
    void onEditEquipmentClicked(int equipmentId) override;
    void onDeleteEquipmentClicked(int equipmentId) override;

    void onRentEquipmentClicked(int equipmentId, const QString& status);
    void onReserveEquipmentClicked(int equipmentId, const QString& status);

    void loadProcessingRequests();
    // void onNewRequestClicked();

    void onRentalsSearchTextChanged(const QString& text);
    void onRentalsStatusFilterChanged(int index);

protected:
    void setupUI() override;

    // Методы создания вкладок
    void createDashboardTab() override;
    void createEquipmentTab() override;
    void createRentalsTab() override;

    // Методы для работы с оборудованием
    void loadEquipmentData() override;
    void updateEquipmentTable(const QList<Equipment*>& equipmentList)  override;
    void clearEquipmentTable() override;
    void applyEquipmentFilters() override;
    void onTabChanged(int index);
    // void updateEquipmentStats() override;
    // Equipment* findEquipmentInList(int id) override;
private:
    QTableWidget *requestsTable;
    int m_currentUserId;
    QLabel *activeRequestsLabel;
    QLabel *reservedLabel;
    QLabel *completedLabel;
    QLabel *canceledLabel;
    QWidget *dashboardTab;

    QWidget *rentalsTab;
    QTableWidget *rentalsTable;
    QLineEdit *searchRentalsEdit;
    QComboBox *statusFilterCombo;

    // Данные
    QList<Rental*> m_allRentals; // Список всех аренд пользователя
    void loadRentalsData();
    void applyRentalsFilters();
    void updateRentalsTable(const QList<Rental*>& rentalsList);
    void clearRentalsTable();

};

#endif // USERWINDOW_H
