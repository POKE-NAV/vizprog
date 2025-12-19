#include <QMenuBar>
#include <QMenu>
#include <QStatusBar>
#include <QLabel>
#include <QDebug>
#include "adminwindow.h"
#include "AdminWindow.h"
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QHeaderView>
#include <QMessageBox>
#include <QInputDialog>
#include <QDate>
#include <QPushButton>
#include <QGroupBox>
#include <QFormLayout>
#include <QSpacerItem>
#include <QIcon>
#include "logindialog.h"
#include <QApplication>
#include <QStatusBar>
#include "addeditdialog.h"
#include "adminwindow.h"
#include "userwindow.h"
#include "QFile"
#include "QFileDevice"
#include "QDir"
#include "QDesktopServices"
#include "QTextStream"

AdminWindow::AdminWindow(User* user, QWidget* parent)
    : MainWindow(user, parent)
{

        setWindowTitle("Сервис аренды техники - " + m_user->getFullName());
        setMinimumSize(1000, 700);
        setupUI();

}

AdminWindow::~AdminWindow()
{
    qDebug() << "AdminWindow уничтожен";
}

void AdminWindow::setupUI()
{
    mainTabWidget = new QTabWidget(this);
    setCentralWidget(mainTabWidget);

    createDashboardTab();
    createEquipmentTab();
    createRentalsTab();
}

void AdminWindow::createDashboardTab()
{
    dashboardTab = new QWidget();
    QVBoxLayout *mainLayout = new QVBoxLayout(dashboardTab);

    QGroupBox *userInfoGroup = new QGroupBox("Информация о пользователе");
    QHBoxLayout *userInfoLayout = new QHBoxLayout(userInfoGroup);
    userInfoGroup->setMaximumHeight(120); // Ограничиваем высоту
    userInfoGroup->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Fixed);

    QVBoxLayout *infoLayout = new QVBoxLayout();

    QString userInfo = QString("👤 <b>%1</b><br>"
                               "📧 Логин: %2<br>"
                               "👑 Роль: %3<br>"
                               "📱 Телефон: %4")
                           .arg(m_user->getFullName())
                           .arg(m_user->getLogin())
                           .arg(m_user->getRole())
                           .arg(m_user->getPhone());

    QLabel *userInfoLabel = new QLabel(userInfo);
    userInfoLabel->setTextFormat(Qt::RichText);

    infoLayout->addWidget(userInfoLabel);
    infoLayout->addStretch();

    QVBoxLayout *buttonLayout = new QVBoxLayout();
    buttonLayout->setAlignment(Qt::AlignTop);

    QPushButton *logoutButton = new QPushButton("🚪 Выйти из аккаунта");
    logoutButton->setStyleSheet("QPushButton {"
                                "    background-color: #f44336;"
                                "    color: white;"
                                "    padding: 10px 20px;"
                                "    border-radius: 5px;"
                                "    font-weight: bold;"
                                "}"
                                "QPushButton:hover {"
                                "    background-color: #d32f2f;"
                                "}");
    logoutButton->setFixedWidth(200);

    connect(logoutButton, &QPushButton::clicked, this, &AdminWindow::onLogoutClicked);

    buttonLayout->addWidget(logoutButton);
    buttonLayout->addStretch();

    userInfoLayout->addLayout(infoLayout);
    userInfoLayout->addLayout(buttonLayout);

    QGroupBox *statsGroup = new QGroupBox("Статистика");
    QHBoxLayout *statsLayout = new QHBoxLayout(statsGroup);

    QList<Rental*> userRentals = Rental::findAllPendingRentals();
    activeRentalsCount = userRentals.size();

    activeRequestsLabel = new QLabel(QString("Заявки в обработке: %1").arg(activeRentalsCount));

    QFont statsFont = activeRequestsLabel->font();
    statsFont.setPointSize(10);
    statsFont.setBold(true);
    activeRequestsLabel->setFont(statsFont);
    activeRequestsLabel->setStyleSheet("color: #2196F3;");

    statsLayout->addWidget(activeRequestsLabel);
    statsLayout->addStretch();

    QGroupBox *processingRequestsGroup = new QGroupBox("Заявки в обработке");
    QVBoxLayout *requestsLayout = new QVBoxLayout(processingRequestsGroup);

    requestsTable = new QTableWidget();
    requestsTable->setColumnCount(7);
    QStringList headers = {
        "ID",
        "Пользователь",
        "Техника",
        "Дата начала",
        "Дата конца",
        "Принять",
        "Отклонить"
    };
    requestsTable->setHorizontalHeaderLabels(headers);

    // Настраиваем ширину колонок
    requestsTable->horizontalHeader()->setSectionResizeMode(0, QHeaderView::ResizeToContents); // ID
    requestsTable->horizontalHeader()->setSectionResizeMode(1, QHeaderView::Stretch); // Пользователь
    requestsTable->horizontalHeader()->setSectionResizeMode(2, QHeaderView::Stretch); // Техника
    requestsTable->horizontalHeader()->setSectionResizeMode(3, QHeaderView::ResizeToContents); // Дата начала
    requestsTable->horizontalHeader()->setSectionResizeMode(4, QHeaderView::ResizeToContents); // Дата конца
    requestsTable->horizontalHeader()->setSectionResizeMode(5, QHeaderView::Fixed); // Принять
    requestsTable->horizontalHeader()->setSectionResizeMode(6, QHeaderView::Fixed); // Отклонить

    requestsTable->setColumnWidth(5, 100);
    requestsTable->setColumnWidth(6, 100);

    // Настройка стиля таблицы
    requestsTable->setAlternatingRowColors(true);
    requestsTable->setSelectionBehavior(QAbstractItemView::SelectRows);
    requestsTable->setEditTriggers(QAbstractItemView::NoEditTriggers);
    requestsTable->setSelectionMode(QAbstractItemView::SingleSelection);

    requestsTable->verticalHeader()->setDefaultSectionSize(50);

    requestsLayout->addWidget(requestsTable);

    loadProcessingRequests();

    mainLayout->addWidget(userInfoGroup);
    mainLayout->addWidget(statsGroup);
    mainLayout->addWidget(processingRequestsGroup);

    mainTabWidget->addTab(dashboardTab, "🏠 Главная");
}

void AdminWindow::loadProcessingRequests()
{
    requestsTable->setRowCount(0);

    QList<Rental*> userRentals = Rental::findAllPendingRentals();

    int row = 0;
    for (Rental* rental : userRentals) {
        if (rental) {
            requestsTable->insertRow(row);

            // ID аренды
            QTableWidgetItem *idItem = new QTableWidgetItem(QString::number(rental->getId()));
            idItem->setTextAlignment(Qt::AlignCenter);
            requestsTable->setItem(row, 0, idItem);

            QString clientName = rental->getClientLogin();
            requestsTable->setItem(row, 1, new QTableWidgetItem(clientName));

            // Название техники
            QString equipName = rental->getEquipmentName();
            requestsTable->setItem(row, 2, new QTableWidgetItem(equipName));

            // Дата начала
            requestsTable->setItem(row, 3, new QTableWidgetItem(rental->getStartDate().toString("dd.MM.yyyy")));

            // Дата окончания
            requestsTable->setItem(row, 4, new QTableWidgetItem(rental->getEndDate().toString("dd.MM.yyyy")));


            QPushButton *acceptButton = new QPushButton("✓ Принять");
            acceptButton->setStyleSheet(
                "QPushButton {"
                "    background-color: #4CAF50;"
                "    color: white;"
                "    border: none;"
                "    border-radius: 3px;"
                "    padding: 5px 10px;"
                "    font-weight: bold;"
                "}"
                "QPushButton:hover {"
                "    background-color: #45a049;"
                "}"
                "QPushButton:pressed {"
                "    background-color: #3d8b40;"
                "}"
                "QPushButton:disabled {"
                "    background-color: #cccccc;"
                "}"
                );
            acceptButton->setProperty("applicationId", rental->getId());
            acceptButton->setEnabled(rental->getStatus() == Rental::STATUS_IN_PROCESSING);

            connect(acceptButton, &QPushButton::clicked, this, [this, rental]() {
                Rental::acceptApplication(rental->getId());
                refreshDashboard();
            });


            QPushButton *rejectButton = new QPushButton("✗ Отклонить");
            rejectButton->setStyleSheet(
                "QPushButton {"
                "    background-color: #f44336;"
                "    color: white;"
                "    border: none;"
                "    border-radius: 3px;"
                "    padding: 5px 10px;"
                "    font-weight: bold;"
                "}"
                "QPushButton:hover {"
                "    background-color: #d32f2f;"
                "}"
                "QPushButton:pressed {"
                "    background-color: #b71c1c;"
                "}"
                "QPushButton:disabled {"
                "    background-color: #cccccc;"
                "}"
                );
            rejectButton->setProperty("applicationId", rental->getId());
            rejectButton->setEnabled(rental->getStatus() == Rental::STATUS_IN_PROCESSING);

            connect(rejectButton, &QPushButton::clicked, this, [this, rental]() {
                Rental::rejectApplication(rental->getId());
                refreshDashboard();
            });

            QWidget *acceptWidget = new QWidget();
            QHBoxLayout *acceptLayout = new QHBoxLayout(acceptWidget);
            acceptLayout->setContentsMargins(2, 2, 2, 2);
            acceptLayout->setAlignment(Qt::AlignCenter);
            acceptLayout->addWidget(acceptButton);
            requestsTable->setCellWidget(row, 5, acceptWidget);

            QWidget *rejectWidget = new QWidget();
            QHBoxLayout *rejectLayout = new QHBoxLayout(rejectWidget);
            rejectLayout->setContentsMargins(2, 2, 2, 2);
            rejectLayout->setAlignment(Qt::AlignCenter);
            rejectLayout->addWidget(rejectButton);
            requestsTable->setCellWidget(row, 6, rejectWidget);

            row++;
        }
    }


    if (row == 0) {
        requestsTable->setRowCount(1);
        QTableWidgetItem *noDataItem = new QTableWidgetItem("Нет заявок в обработке");
        noDataItem->setTextAlignment(Qt::AlignCenter);
        noDataItem->setForeground(Qt::gray);
        requestsTable->setItem(0, 0, noDataItem);
        requestsTable->setSpan(0, 0, 1, 7);
    }

    requestsTable->resizeColumnsToContents();
}

void AdminWindow::refreshDashboard()
{
    QList<Rental*> userRentals = Rental::findAllPendingRentals();
    int pendingCount = userRentals.size();
    Rental::clearRentalList(userRentals);

    activeRequestsLabel->setText(QString("Заявки в обработке: %1").arg(pendingCount));

    QList<Equipment*> equipment = Equipment::findAll();
    QList<Rental*> rental = Rental::findAllRentals();
    loadProcessingRequests();
    updateEquipmentTable(equipment);
    updateRentalsTable(rental);
}

void AdminWindow::createEquipmentTab()
{
    equipmentTab = new QWidget();
    QVBoxLayout *mainLayout = new QVBoxLayout(equipmentTab);

    QHBoxLayout *filterLayout = new QHBoxLayout();

    searchEquipmentEdit = new QLineEdit();
    searchEquipmentEdit->setPlaceholderText("Поиск по названию...");
    searchEquipmentEdit->setClearButtonEnabled(true);

    typeFilterCombo = new QComboBox();
    typeFilterCombo->addItem("Все типы");
    typeFilterCombo->addItems(Equipment::getAllTypes());

    statusFilterCombo = new QComboBox();
    statusFilterCombo->addItems({"Все статусы", "Свободно", "Арендованно", "Обслуживается"});

    QPushButton *addEquipmentBtn = new QPushButton(QIcon(":/icons/add.png"), "Добавить");
    addEquipmentBtn->setToolTip("Добавить новое оборудование");

    filterLayout->addWidget(new QLabel("Поиск:"));
    filterLayout->addWidget(searchEquipmentEdit, 2);
    filterLayout->addWidget(new QLabel("Тип:"));
    filterLayout->addWidget(typeFilterCombo);
    filterLayout->addWidget(new QLabel("Статус:"));
    filterLayout->addWidget(statusFilterCombo);
    filterLayout->addStretch();
    filterLayout->addWidget(addEquipmentBtn);

    // Таблица оборудования
    equipmentTable = new QTableWidget();
    equipmentTable->setColumnCount(8);
    equipmentTable->setHorizontalHeaderLabels({
        "ID",
        "Название",
        "Тип",
        "Статус",
        "Цена/сутки",
        "Залог",
        "Инв. номер",
        "Действия"
    });

    // Настройка таблицы
    equipmentTable->verticalHeader()->setVisible(false);
    equipmentTable->horizontalHeader()->setStretchLastSection(true);
    equipmentTable->setSelectionBehavior(QAbstractItemView::SelectRows);
    equipmentTable->setEditTriggers(QAbstractItemView::NoEditTriggers);
    equipmentTable->setAlternatingRowColors(true);

    // Скрываем колонку ID
    equipmentTable->setColumnHidden(0, true);

    // Настройка ширины колонок
    equipmentTable->setColumnWidth(1, 250); // Название
    equipmentTable->setColumnWidth(2, 100); // Тип
    equipmentTable->setColumnWidth(3, 120); // Статус
    equipmentTable->setColumnWidth(4, 100); // Цена
    equipmentTable->setColumnWidth(5, 100); // Залог
    equipmentTable->setColumnWidth(6, 120); // Инв. номер


    connect(searchEquipmentEdit, &QLineEdit::textChanged,
            this, &AdminWindow::onEquipmentSearchTextChanged);
    connect(statusFilterCombo, QOverload<int>::of(&QComboBox::currentIndexChanged),
            this, &AdminWindow::onEquipmentStatusFilterChanged);
    connect(typeFilterCombo, QOverload<int>::of(&QComboBox::currentIndexChanged),
            this, &AdminWindow::onEquipmentTypeFilterChanged);
    connect(addEquipmentBtn, &QPushButton::clicked,
            this, &AdminWindow::onAddNewEquipmentClicked);

    mainLayout->addLayout(filterLayout);
    mainLayout->addWidget(equipmentTable);

    mainTabWidget->addTab(equipmentTab, "📋 Оборудование");

    loadEquipmentData();
}

void AdminWindow::loadEquipmentData()
{
    // Очищаем предыдущие данные
    clearEquipmentTable();
    Equipment::clearEquipmentList(m_allEquipment);

    // Загружаем все оборудование
    m_allEquipment = Equipment::findAll();

    // Обновляем фильтр типов
    QString currentType = typeFilterCombo->currentText();
    typeFilterCombo->clear();
    typeFilterCombo->addItem("Все типы");
    typeFilterCombo->addItems(Equipment::getAllTypes());

    int index = typeFilterCombo->findText(currentType);
    if (index >= 0) {
        typeFilterCombo->setCurrentIndex(index);
    }

    applyEquipmentFilters();
}

void AdminWindow::updateEquipmentTable(const QList<Equipment*>& equipmentList)
{
    clearEquipmentTable();

    equipmentTable->setRowCount(equipmentList.size());

    for (int i = 0; i < equipmentList.size(); ++i) {
        Equipment* equipment = equipmentList[i];

        // ID (скрытая колонка)
        QTableWidgetItem* idItem = new QTableWidgetItem(QString::number(equipment->getId()));
        equipmentTable->setItem(i, 0, idItem);

        // Название
        equipmentTable->setItem(i, 1, new QTableWidgetItem(equipment->getName()));

        // Тип
        equipmentTable->setItem(i, 2, new QTableWidgetItem(equipment->getType()));

        // Статус
        QTableWidgetItem* statusItem = new QTableWidgetItem(equipment->getStatus());

        if (equipment->isAvailable()) {
            statusItem->setBackground(QColor(220, 255, 220)); // Зеленый
        } else if (equipment->isRented()) {
            statusItem->setBackground(QColor(255, 220, 220)); // Красный
        } else if (equipment->isUnderMaintenance()) {
            statusItem->setBackground(QColor(255, 255, 200)); // Желтый
        }

        equipmentTable->setItem(i, 3, statusItem);

        // Цена/сутки
        equipmentTable->setItem(i, 4, new QTableWidgetItem(equipment->getFormattedPrice()));

        // Залог
        equipmentTable->setItem(i, 5, new QTableWidgetItem(equipment->getFormattedDeposit()));

        // Инвентарный номер
        equipmentTable->setItem(i, 6, new QTableWidgetItem(equipment->getInventoryNumber()));

        // Действия
        QWidget* actionsWidget = new QWidget();
        QHBoxLayout* actionsLayout = new QHBoxLayout(actionsWidget);
        actionsLayout->setContentsMargins(2, 2, 2, 2);
        actionsLayout->setSpacing(2);

        // Кнопка редактирования
        QPushButton* editBtn = new QPushButton("✏");
        editBtn->setToolTip("Редактировать");
        editBtn->setFixedSize(30, 25);
        editBtn->setProperty("equipmentId", equipment->getId());
        connect(editBtn, &QPushButton::clicked, this, [this, equipment]() {
            onEditEquipmentClicked(equipment->getId());
        });
        // Кнопка удаления

        QPushButton* deleteBtn = new QPushButton("🗑");
        if (equipment->isAvailable()){
            deleteBtn->setToolTip("Удалить");
            deleteBtn->setFixedSize(30, 25);
            deleteBtn->setProperty("equipmentId", equipment->getId());
            connect(deleteBtn, &QPushButton::clicked, this, [this, equipment]() {
                onDeleteEquipmentClicked(equipment->getId());
            });
        }

        actionsLayout->addWidget(editBtn);
        actionsLayout->addWidget(deleteBtn);
        actionsLayout->addStretch();

        equipmentTable->setCellWidget(i, 7, actionsWidget);
    }

}

void AdminWindow::onEditEquipmentClicked(int equipmentId)
{
    Equipment* equipment = Equipment::findById(equipmentId);
    if (!equipment) {
        QMessageBox::warning(this, "Ошибка", "Оборудование не найдено!");
        return;
    }

    AddEditDialog dialog(equipment, this);

    if (dialog.exec() == QDialog::Accepted) {
        Equipment updatedEquipment = dialog.getEquipment();

        // Проверяем уникальность инвентарного номера
        if (!Equipment::validateInventoryNumber(updatedEquipment.getInventoryNumber(), equipmentId)) {
            QMessageBox::critical(this, "Ошибка",
                                  "Инвентарный номер уже существует. Пожалуйста, введите другой номер.");
        } else {
            if (updatedEquipment.save()) {
                QMessageBox::information(this, "Успех", "Изменения сохранены!");
                loadEquipmentData();
            } else {
                QMessageBox::critical(this, "Ошибка",
                                      "Не удалось сохранить изменения.");
            }
        }
    }

    delete equipment;
}

void AdminWindow::clearEquipmentTable()
{
    equipmentTable->setRowCount(0);
}

void AdminWindow::applyEquipmentFilters()
{
    QString searchText = searchEquipmentEdit->text().trimmed().toLower();
    QString statusFilter = statusFilterCombo->currentText();
    QString typeFilter = typeFilterCombo->currentText();

    QList<Equipment*> filteredList;

    for (Equipment* equipment : m_allEquipment) {
        bool matches = true;

        if (!searchText.isEmpty()) {
            QString name = equipment->getName().toLower();
            QString type = equipment->getType().toLower();
            QString invNum = equipment->getInventoryNumber().toLower();

            if (!name.contains(searchText) &&
                !type.contains(searchText) &&
                !invNum.contains(searchText)) {
                matches = false;
            }
        }

        if (matches && statusFilter != "Все статусы") {
            QString status = equipment->getStatus();
            if (statusFilter == "Свободно" && !equipment->isAvailable()) matches = false;
            else if (statusFilter == "Арендованно" && !equipment->isRented()) matches = false;
            else if (statusFilter == "Обслуживается" && !equipment->isUnderMaintenance()) matches = false;
        }

        if (matches && typeFilter != "Все типы") {
            if (equipment->getType() != typeFilter) {
                matches = false;
            }
        }

        if (matches) {
            filteredList.append(equipment);
        }
    }

    updateEquipmentTable(filteredList);
}

void AdminWindow::onEquipmentSearchTextChanged(const QString& text)
{
    Q_UNUSED(text);
    applyEquipmentFilters();
}

void AdminWindow::onEquipmentStatusFilterChanged(int index)
{
    Q_UNUSED(index);
    applyEquipmentFilters();
}

void AdminWindow::onEquipmentTypeFilterChanged(int index)
{
    Q_UNUSED(index);
    applyEquipmentFilters();
}

void AdminWindow::onAddNewEquipmentClicked()
{
    AddEditDialog dialog(this);

    if (dialog.exec() == QDialog::Accepted) {
        Equipment equipment = dialog.getEquipment();

        if (!Equipment::validateInventoryNumber(equipment.getInventoryNumber())) {
            QMessageBox::critical(this, "Ошибка",
                                  "Инвентарный номер уже существует. Пожалуйста, введите другой номер.");
            return;
        }

        if (equipment.save()) {
            QMessageBox::information(this, "Успех", "Оборудование успешно добавлено!");
            loadEquipmentData(); // Перезагружаем список
        } else {
            QMessageBox::critical(this, "Ошибка",
                                  "Не удалось добавить оборудование.");
        }
    }
}

void AdminWindow::onDeleteEquipmentClicked(int equipmentId)
{
    Equipment* equipment = Equipment::findById(equipmentId);
    if (!equipment) {
        QMessageBox::warning(this, "Ошибка", "Оборудование не найдено!");
        return;
    }

    if (equipment->isRented()) {
        QMessageBox::warning(this, "Ошибка",
                             "Нельзя удалить арендованное оборудование!");
        delete equipment;
        return;
    }

    QMessageBox::StandardButton reply = QMessageBox::question(
        this, "Подтверждение",
        QString("Удалить оборудование \"%1\"?\nИнв. номер: %2")
            .arg(equipment->getName())
            .arg(equipment->getInventoryNumber()),
        QMessageBox::Yes | QMessageBox::No
        );

    if (reply == QMessageBox::Yes) {
        if (equipment->remove()) {
            QMessageBox::information(this, "Успех", "Оборудование удалено!");
            loadEquipmentData();
        } else {
            QMessageBox::critical(this, "Ошибка", "Не удалось удалить оборудование!");
        }
    }

    delete equipment;
}

void AdminWindow::createRentalsTab()
{
    rentalsTab = new QWidget();
    QVBoxLayout *mainLayout = new QVBoxLayout(rentalsTab);

    QHBoxLayout *topPanelLayout = new QHBoxLayout();

    // Поиск по оборудованию/клиенту
    searchRentalsEdit = new QLineEdit();
    searchRentalsEdit->setPlaceholderText("Поиск по оборудованию или клиенту...");
    searchRentalsEdit->setClearButtonEnabled(true);

    // Фильтр по статусу аренды
    statusFilterCombo = new QComboBox();
    statusFilterCombo->addItem("Все статусы");
    statusFilterCombo->addItem(Rental::STATUS_ACTIVE);
    statusFilterCombo->addItem(Rental::STATUS_COMPLETED);
    statusFilterCombo->addItem(Rental::STATUS_CANCELED);
    statusFilterCombo->addItem(Rental::STATUS_RESERVED);

    startDateEdit = new QDateEdit();
    startDateEdit->setCalendarPopup(true);
    startDateEdit->setDate(QDate(QDate::currentDate().year(), QDate::currentDate().month(), 1));
    startDateEdit->setSpecialValueText("С :");

    // Фильтр по дате окончания
    endDateEdit = new QDateEdit();
    endDateEdit->setCalendarPopup(true);
    endDateEdit->setDate(QDate(QDate::currentDate().year(), QDate::currentDate().month(),
                               QDate::currentDate().daysInMonth()));
    endDateEdit->setSpecialValueText("По :");

    reportTypeCombo = new QComboBox();
    reportTypeCombo->addItem("Отчёт по доходам");
    reportTypeCombo->addItem("Отчёт по загруженности");
    reportTypeCombo->addItem("Отчёт по популярности");
    reportTypeCombo->addItem("Полный отчёт по арендам");

    QPushButton *generateReportBtn = new QPushButton("📊 Сформировать отчёт");
    generateReportBtn->setFixedWidth(180);
    connect(generateReportBtn, &QPushButton::clicked, this, &AdminWindow::onGenerateReportClicked);

    topPanelLayout->addWidget(new QLabel("Поиск:"));
    topPanelLayout->addWidget(searchRentalsEdit, 2);
    topPanelLayout->addWidget(new QLabel("Статус:"));
    topPanelLayout->addWidget(statusFilterCombo);
    topPanelLayout->addWidget(new QLabel("Дата с:"));
    topPanelLayout->addWidget(startDateEdit);
    topPanelLayout->addWidget(new QLabel("По:"));
    topPanelLayout->addWidget(endDateEdit);
    topPanelLayout->addWidget(new QLabel("Тип отчёта:"));
    topPanelLayout->addWidget(reportTypeCombo);
    topPanelLayout->addStretch();
    topPanelLayout->addWidget(generateReportBtn);

    activeRentalsTable = new QTableWidget();
    activeRentalsTable->setColumnCount(8);
    activeRentalsTable->setHorizontalHeaderLabels({
        "ID",
        "Клиент",
        "Оборудование",
        "Дата начала",
        "Дата конца",
        "Количество дней",
        "Итоговая сумма",
        "Статус"
    });

    // Настройка таблицы
    activeRentalsTable->verticalHeader()->setVisible(false);
    activeRentalsTable->setSelectionBehavior(QAbstractItemView::SelectRows);
    activeRentalsTable->setEditTriggers(QAbstractItemView::NoEditTriggers);
    activeRentalsTable->setAlternatingRowColors(true);
    activeRentalsTable->horizontalHeader()->setStretchLastSection(false);

    // Скрываем колонку ID
    activeRentalsTable->setColumnHidden(0, true);

    // Настройка ширины колонок
    activeRentalsTable->setColumnWidth(1, 180);
    activeRentalsTable->setColumnWidth(2, 220);
    activeRentalsTable->setColumnWidth(3, 100);
    activeRentalsTable->setColumnWidth(4, 100);
    activeRentalsTable->setColumnWidth(5, 120);
    activeRentalsTable->setColumnWidth(6, 120);
    activeRentalsTable->setColumnWidth(7, 120);
    activeRentalsTable->horizontalHeader()->setStretchLastSection(true);

    connect(searchRentalsEdit, &QLineEdit::textChanged,
            this, &AdminWindow::onRentalsSearchTextChanged);
    connect(statusFilterCombo, QOverload<int>::of(&QComboBox::currentIndexChanged),
            this, &AdminWindow::onRentalsStatusFilterChanged);
    connect(startDateEdit, &QDateEdit::dateChanged,
            this, &AdminWindow::onRentalsDateFilterChanged);
    connect(endDateEdit, &QDateEdit::dateChanged,
            this, &AdminWindow::onRentalsDateFilterChanged);

    m_reportManager = new ReportManager(this);
    connect(m_reportManager, &ReportManager::reportGenerated,
            this, &AdminWindow::onReportGenerated);
    connect(m_reportManager, &ReportManager::reportGenerationFailed,
            this, &AdminWindow::onReportGenerationFailed);

    mainLayout->addLayout(topPanelLayout);
    mainLayout->addWidget(new QLabel("Все аренды:"));
    mainLayout->addWidget(activeRentalsTable, 1);

    mainTabWidget->addTab(rentalsTab, "📄 Аренды и отчёты");

    loadRentalsData();
}


void AdminWindow::onGenerateReportClicked()
{
    if (!m_reportManager) {
        QMessageBox::critical(this, "Ошибка", "Менеджер отчётов не инициализирован");
        return;
    }

    // Получаем параметры
    QDate startDate = startDateEdit->date();
    QDate endDate = endDateEdit->date();

    // Валидация дат
    if (!startDate.isValid() || !endDate.isValid()) {
        QMessageBox::warning(this, "Ошибка", "Невалидные даты");
        return;
    }

    if (startDate > endDate) {
        QMessageBox::warning(this, "Ошибка",
                             "Дата начала не может быть позже даты окончания");
        return;
    }

    // Определяем тип отчёта
    ReportManager::ReportType reportType;
    QString typeText = reportTypeCombo->currentText();

    if (typeText == "Отчёт по доходам") {
        reportType = ReportManager::IncomeReport;
    } else if (typeText == "Отчёт по загруженности") {
        reportType = ReportManager::EquipmentLoadReport;
    } else if (typeText == "Отчёт по популярности") {
        reportType = ReportManager::PopularityReport;
    } else {
        reportType = ReportManager::FullRentalReport;
    }

    statusBar()->showMessage("Формирование отчёта...");

    // Генерируем отчёт
    bool success = m_reportManager->generateReport(reportType, startDate, endDate, m_allRentals);

    QApplication::restoreOverrideCursor();
    statusBar()->clearMessage();

    if (!success) {
        return;
    }
}

void AdminWindow::onReportGenerated(const QString &filePath)
{
    // Показываем отчёт в текстовом поле
    QFile file(filePath);
    if (file.open(QIODevice::ReadOnly | QIODevice::Text)) {
        // Читаем как UTF-8
        QByteArray data = file.readAll();
        QString content = QString::fromUtf8(data);
        file.close();
    } else {
        QMessageBox::warning(this, "Ошибка",
                             QString("Не удалось открыть файл: %1").arg(file.errorString()));
        return;
    }

    QString message = QString("Отчёт успешно сохранён:\n%1")
                          .arg(QDir::toNativeSeparators(filePath));

    QMessageBox::information(this, "Отчёт сформирован", message);

    // Предложить открыть файл
    QMessageBox::StandardButton reply;
    reply = QMessageBox::question(this, "Открыть отчёт",
                                  "Хотите открыть файл с отчётом?",
                                  QMessageBox::Yes | QMessageBox::No);

    if (reply == QMessageBox::Yes) {
        QDesktopServices::openUrl(QUrl::fromLocalFile(filePath));
    }
}

void AdminWindow::onReportGenerationFailed(const QString &error)
{
    QMessageBox::critical(this, "Ошибка формирования отчёта", error);
}

void AdminWindow::onRentalsDateFilterChanged()
{
    applyRentalsFilters();
}

void AdminWindow::onRentalsSearchTextChanged(const QString& text)
{
    applyRentalsFilters();
}

void AdminWindow::onRentalsStatusFilterChanged(int index)
{
    applyRentalsFilters();
}

void AdminWindow::loadRentalsData()
{
    // Очищаем предыдущие данные
    clearRentalsTable();
    Rental::clearRentalList(m_allRentals);

    // Загружаем все аренды пользователя
    m_allRentals = Rental::findAllRentals();

    applyRentalsFilters();
}

void AdminWindow::applyRentalsFilters()
{
    if (!searchRentalsEdit || !statusFilterCombo) return;

    QList<Rental*> filteredList;

    QString searchText = searchRentalsEdit->text().trimmed().toLower();
    QString statusFilter = statusFilterCombo->currentText();
    QDate startDateFilter = startDateEdit->date();
    QDate endDateFilter = endDateEdit->date();

    if (startDateFilter > endDateFilter) {
        QMessageBox::warning(this, "Ошибка фильтра",
                             "Дата начала фильтра не может быть позже даты окончания");
        return;
    }

    for (Rental* rental : m_allRentals) {
        if (!rental) continue;

        if (!searchText.isEmpty()) {
            QString equipmentName = rental->getEquipmentName().toLower();
            QString clientName = rental->getClientLogin().toLower(); // Предполагаем наличие метода
            if (!equipmentName.contains(searchText) && !clientName.contains(searchText)) {
                continue;
            }
        }

        if (statusFilter != "Все статусы" && rental->getStatus() != statusFilter) {
            continue;
        }

        if (startDateFilter.isValid() && startDateFilter > QDate(2000, 1, 1)) {
            if (rental->getStartDate() < startDateFilter) {
                continue;
            }
        }

        if (endDateFilter.isValid() && endDateFilter < QDate::currentDate().addYears(1)) {
            if (rental->getEndDate() > endDateFilter) {
                continue;
            }
        }

        filteredList.append(rental);
    }

    updateRentalsTable(filteredList);
}

void AdminWindow::updateRentalsTable(const QList<Rental*>& rentalsList)
{
    clearRentalsTable();

    if (!activeRentalsTable) return;

    activeRentalsTable->setRowCount(rentalsList.size());

    for (int i = 0; i < rentalsList.size(); ++i) {
        Rental* rental = rentalsList[i];
        if (!rental) continue;

        // ID (скрытая колонка)
        QTableWidgetItem* idItem = new QTableWidgetItem(QString::number(rental->getId()));
        activeRentalsTable->setItem(i, 0, idItem);

        // Клиент
        activeRentalsTable->setItem(i, 1, new QTableWidgetItem(rental->getClientLogin()));

        // Оборудование
        activeRentalsTable->setItem(i, 2, new QTableWidgetItem(rental->getEquipmentName()));

        // Дата начала
        activeRentalsTable->setItem(i, 3, new QTableWidgetItem(
                                              rental->getStartDate().toString("dd.MM.yyyy")));

        // Дата окончания
        activeRentalsTable->setItem(i, 4, new QTableWidgetItem(
                                              rental->getEndDate().toString("dd.MM.yyyy")));

        // Количество дней
        int days = rental->getRentalDays();
        activeRentalsTable->setItem(i, 5, new QTableWidgetItem(QString::number(days)));

        // Сумма
        QString priceText = QString("%1 ₽").arg(rental->getTotalPrice(), 0, 'f', 2);
        activeRentalsTable->setItem(i, 6, new QTableWidgetItem(priceText));

        // Статус
        QTableWidgetItem* statusItem = new QTableWidgetItem(rental->getStatus());

        // Раскрашиваем статусы (нужно проверить константы)
        if (rental->getStatus() == "Активна") {
            statusItem->setBackground(QColor(220, 255, 220));
            statusItem->setForeground(QColor(0, 100, 0));
        } else if (rental->getStatus() == "Завершена") {
            statusItem->setBackground(QColor(240, 240, 240));
            statusItem->setForeground(QColor(100, 100, 100));
        } else if (rental->getStatus() == "Отменена") {
            statusItem->setBackground(QColor(255, 220, 220));
            statusItem->setForeground(QColor(150, 0, 0));
        } else if (rental->getStatus() == "Забронирована") {
            statusItem->setBackground(QColor(255, 255, 200));
            statusItem->setForeground(QColor(120, 120, 0));
        }

        activeRentalsTable->setItem(i, 7, statusItem);
    }

    activeRentalsTable->resizeRowsToContents();
}

void AdminWindow::clearRentalsTable()
{
    if (!activeRentalsTable) return;

    for (int i = 0; i < activeRentalsTable->rowCount(); ++i) {
        for (int j = 0; j < activeRentalsTable->columnCount(); ++j) {
            QTableWidgetItem* item = activeRentalsTable->takeItem(i, j);
            if (item) delete item;
        }
    }
    activeRentalsTable->setRowCount(0);
}

