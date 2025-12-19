#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QTabWidget>
#include <QTableWidget>
#include <QLineEdit>
#include <QComboBox>
#include <QDateEdit>
#include <QTextEdit>
#include <QGroupBox>
#include <QLabel>
#include "user.h"
#include "equipment.h"

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    // Фабричный метод для создания окна
    static MainWindow* createWindow(User* user, QWidget *parent = nullptr);

    virtual ~MainWindow();

signals:
    void restartApplication();

protected slots:


    // Слоты для вкладки оборудования
    virtual void onEquipmentSearchTextChanged(const QString &text) = 0;
    virtual void onEquipmentStatusFilterChanged(int index) = 0;
    virtual void onEquipmentTypeFilterChanged(int index) = 0;
    virtual void onAddNewEquipmentClicked() = 0;
    virtual void onEditEquipmentClicked(int equipmentId) = 0;
    virtual void onDeleteEquipmentClicked(int equipmentId) = 0;

protected:
    explicit MainWindow(User* user, QWidget* parent = nullptr);
    // Приватные методы
    void onLogoutClicked();
    virtual void setupUI() = 0;

    // Методы создания вкладок
    virtual void createDashboardTab() = 0;
    virtual void createEquipmentTab() = 0;
    virtual void createRentalsTab() = 0;


    // Методы для работы с оборудованием
    virtual void loadEquipmentData() = 0;
    virtual void updateEquipmentTable(const QList<Equipment*>& equipmentList)  = 0;
    virtual void clearEquipmentTable() = 0;
    virtual void applyEquipmentFilters() = 0;
    // virtual void updateEquipmentStats() = 0;
    // virtual Equipment* findEquipmentInList(int id) = 0;

    // Данные
    User* m_user = nullptr;
    QList<Equipment*> m_allEquipment;

    // Виджеты главного окна
    QTabWidget *mainTabWidget;
    QWidget *dashboardTab;
    QWidget *rentalsTab;
    QWidget *clientsTab;
    QWidget *reportsTab;

    // Виджеты для Dashboard
    QLabel *activeRentalsLabel;
    QLabel *availableEquipmentLabel;
    QLabel *todayIncomeLabel;
    QTableWidget *activeRentalsTable;
    QTableWidget *recentRentalsTable;

    // Виджеты для вкладки Оборудование
    QWidget *equipmentTab;
    QTableWidget *equipmentTable;
    QLineEdit *searchEquipmentEdit;
    QComboBox *statusFilterCombo;
    QComboBox *typeFilterCombo;

    // Виджеты для вкладки Клиенты
    QTableWidget *clientsTable;

    // Виджеты для вкладки Отчеты
    QComboBox *reportTypeCombo;
    QDateEdit *startDateEdit;
    QDateEdit *endDateEdit;
    QTextEdit *reportOutput;
};

#endif // MAINWINDOW_H
