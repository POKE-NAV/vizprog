#ifndef ADMINWINDOW_H
#define ADMINWINDOW_H

#include "mainwindow.h"
#include "rental.h"
#include "reportmanager.h"
class AdminWindow : public MainWindow
{
    Q_OBJECT

public:
    // Конструктор - будет вызывать protected конструктор MainWindow
    explicit AdminWindow(User* user, QWidget* parent = nullptr);

    // Деструктор
    ~AdminWindow() override;
    int activeRentalsCount;
    void refreshDashboard();
    void loadProcessingRequests();

private:
    QTableWidget *requestsTable;
    int m_currentUserId;
    QLabel *activeRequestsLabel;
    QLabel *reservedLabel;
    QLabel *completedLabel;
    QLabel *canceledLabel;
    QWidget *dashboardTab;

    QLineEdit *searchRentalsEdit;
    QComboBox *statusFilterCombo;
    QComboBox *reportTypeCombo;
    QDateEdit *startDateEdit;
    QDateEdit *endDateEdit;
    QTextEdit *reportOutput;
    QTableWidget *activeRentalsTable;
    QList<Rental*> m_allRentals;

    ReportManager* m_reportManager;
    // QComboBox* reportTypeCombo;

    void onRentalsDateFilterChanged();
    void loadRentalsData();
    void applyRentalsFilters();
    void updateRentalsTable(const QList<Rental*>& rentalsList);
    void clearRentalsTable();
    void onRentalsSearchTextChanged(const QString& text);
    void onRentalsStatusFilterChanged(int index);
protected slots:
    void onGenerateReportClicked();
    void onReportGenerated(const QString &filePath);
    void onReportGenerationFailed(const QString &error);

    // void onGenerateReportClicked() ;

    // Слоты для вкладки оборудования
    void onEquipmentSearchTextChanged(const QString &text) override;
    void onEquipmentStatusFilterChanged(int index) override;
    void onEquipmentTypeFilterChanged(int index) override;
    void onAddNewEquipmentClicked() override;
    void onEditEquipmentClicked(int equipmentId) override;
    void onDeleteEquipmentClicked(int equipmentId) override;

protected:
    void setupUI() override;

    // Методы создания вкладок
    void createDashboardTab() override;
    void createEquipmentTab() override;
    void createRentalsTab() override;
    // void createClientsTab() ;
    // void createReportsTab() ;

    // Методы для работы с оборудованием
    void loadEquipmentData() override;
    void updateEquipmentTable(const QList<Equipment*>& equipmentList)  override;
    void clearEquipmentTable() override;
    void applyEquipmentFilters() override;
    // void updateEquipmentStats() override;
    // Equipment* findEquipmentInList(int id) override;
};

#endif // ADMINWINDOW_H
