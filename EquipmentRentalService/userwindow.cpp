#include "userwindow.h"
#include <QMenuBar>
#include <QMenu>
#include <QStatusBar>
#include <QLabel>
#include <QDebug>
#include "mainwindow.h"
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
#include "UserWindow.h"
#include "userwindow.h"
#include <QMainWindow>
#include <rentaldialog.h>
#include <rental.h>


UserWindow::UserWindow(User* user, QWidget* parent)
    : MainWindow(user, parent)
{
    setWindowTitle("Сервис аренды техники - " + m_user->getFullName());
    setMinimumSize(1100, 700);
    setupUI();

}

UserWindow::~UserWindow()
{
    qDebug() << "UserWindow уничтожен";
}

void UserWindow::setupUI()
{
    mainTabWidget = new QTabWidget(this);
    setCentralWidget(mainTabWidget);


    createDashboardTab();
    createEquipmentTab();
    createRentalsTab();
}

void UserWindow::createDashboardTab()
{
    dashboardTab = new QWidget();
    QVBoxLayout *mainLayout = new QVBoxLayout(dashboardTab);

    QGroupBox *userInfoGroup = new QGroupBox("Информация о пользователе");
    QHBoxLayout *userInfoLayout = new QHBoxLayout(userInfoGroup);
    userInfoGroup->setMaximumHeight(120);
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

    connect(logoutButton, &QPushButton::clicked, this, &UserWindow::onLogoutClicked);

    buttonLayout->addWidget(logoutButton);
    buttonLayout->addStretch();

    userInfoLayout->addLayout(infoLayout);
    userInfoLayout->addLayout(buttonLayout);

    QGroupBox *statsGroup = new QGroupBox("Статистика");
    QHBoxLayout *statsLayout = new QHBoxLayout(statsGroup);

    QList<Rental*> userRentals = Rental::findMyPendingRentals(m_user->getId());


    activeRentalsCount = Rental::filterReservedRentals(userRentals).size();

    activeRequestsLabel = new QLabel(QString("Заявки: %1").arg(activeRentalsCount));

    QFont statsFont = activeRequestsLabel->font();
    statsFont.setPointSize(10);
    statsFont.setBold(true);

    activeRequestsLabel->setFont(statsFont);

    activeRequestsLabel->setStyleSheet("color: #2196F3;");


    statsLayout->addWidget(activeRequestsLabel);

    QGroupBox *processingRequestsGroup = new QGroupBox("Заявки в обработке");
    QVBoxLayout *requestsLayout = new QVBoxLayout(processingRequestsGroup);

    requestsTable = new QTableWidget();
    requestsTable->setColumnCount(5); // Изменил на 5 колонок
    requestsTable->setHorizontalHeaderLabels({"ID", "Техника", "Дата начала", "Дата конца", "Статус"});
    requestsTable->horizontalHeader()->setStretchLastSection(true);

    // Настройка стиля таблицы
    requestsTable->setAlternatingRowColors(true);
    requestsTable->setSelectionBehavior(QAbstractItemView::SelectRows);
    requestsTable->setEditTriggers(QAbstractItemView::NoEditTriggers); // Только чтение

    loadProcessingRequests();

    requestsLayout->addWidget(requestsTable);

    mainLayout->addWidget(userInfoGroup);
    mainLayout->addWidget(statsGroup);
    mainLayout->addWidget(processingRequestsGroup);

    mainTabWidget->addTab(dashboardTab, "🏠 Главная");
}

void UserWindow::loadProcessingRequests()
{
    requestsTable->setRowCount(0);

    QList<Rental*> userRentals = Rental::findMyPendingRentals(m_user->getId());

    int row = 0;
    for (Rental* rental : userRentals) {
        if (rental ) {
            requestsTable->insertRow(row);

            // ID аренды
            QTableWidgetItem *idItem = new QTableWidgetItem(QString::number(rental->getId()));
            idItem->setTextAlignment(Qt::AlignCenter);
            requestsTable->setItem(row, 0, idItem);

            // Название техники
            QString equipName = rental->Rental::getEquipmentName();
            requestsTable->setItem(row, 1, new QTableWidgetItem(equipName));

            // Дата начала
            requestsTable->setItem(row, 2, new QTableWidgetItem(rental->getStartDate().toString("dd.MM.yyyy")));

            // Дата окончания
            requestsTable->setItem(row, 3, new QTableWidgetItem(rental->getEndDate().toString("dd.MM.yyyy")));

            // Статус с цветовым оформлением
            QTableWidgetItem *statusItem = new QTableWidgetItem(rental->getStatus());
            statusItem->setTextAlignment(Qt::AlignCenter);

            // Устанавливаем цвет статуса
            if (rental->getStatus() == "active") {
                statusItem->setBackground(QColor(33, 150, 243, 50)); // Синий
            } else if (rental->getStatus() == "reserved") {
                statusItem->setBackground(QColor(255, 152, 0, 50)); // Оранжевый
            } else if (rental->getStatus() == "processing") {
                statusItem->setBackground(QColor(156, 39, 176, 50)); // Фиолетовый
            }

            requestsTable->setItem(row, 4, statusItem);

            row++;
        }
    }


    requestsTable->resizeColumnsToContents();

    if (row == 0) {
        requestsTable->setRowCount(1);
        QTableWidgetItem *noDataItem = new QTableWidgetItem("Нет заявок в обработке");
        noDataItem->setTextAlignment(Qt::AlignCenter);
        noDataItem->setForeground(Qt::gray);
        requestsTable->setItem(0, 0, noDataItem);
        requestsTable->setSpan(0, 0, 1, 5);
    }
}

void UserWindow::createEquipmentTab()
{
    equipmentTab = new QWidget();
    QVBoxLayout *mainLayout = new QVBoxLayout(equipmentTab);

    // Панель поиска и фильтров
    QHBoxLayout *filterLayout = new QHBoxLayout();

    searchEquipmentEdit = new QLineEdit();
    searchEquipmentEdit->setPlaceholderText("Поиск по названию...");
    searchEquipmentEdit->setClearButtonEnabled(true);

    typeFilterCombo = new QComboBox();
    typeFilterCombo->addItem("Все типы");
    typeFilterCombo->addItems(Equipment::getAllTypes());

    statusFilterCombo = new QComboBox();
    statusFilterCombo->addItems({"Все статусы", "Свободно", "Арендованно", "Обслуживается"});

    filterLayout->addWidget(new QLabel("Поиск:"));
    filterLayout->addWidget(searchEquipmentEdit, 2);
    filterLayout->addWidget(new QLabel("Тип:"));
    filterLayout->addWidget(typeFilterCombo);
    filterLayout->addWidget(new QLabel("Статус:"));
    filterLayout->addWidget(statusFilterCombo);
    filterLayout->addStretch();


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
            this, &UserWindow::onEquipmentSearchTextChanged);
    connect(statusFilterCombo, QOverload<int>::of(&QComboBox::currentIndexChanged),
            this, &UserWindow::onEquipmentStatusFilterChanged);
    connect(typeFilterCombo, QOverload<int>::of(&QComboBox::currentIndexChanged),
            this, &UserWindow::onEquipmentTypeFilterChanged);


    mainLayout->addLayout(filterLayout);
    mainLayout->addWidget(equipmentTable);

    mainTabWidget->addTab(equipmentTab, "📋 Оборудование");

    loadEquipmentData();
}

void UserWindow::loadEquipmentData()
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

    // Восстанавливаем выбранный тип, если он есть
    int index = typeFilterCombo->findText(currentType);
    if (index >= 0) {
        typeFilterCombo->setCurrentIndex(index);
    }

    // Применяем текущие фильтры
    applyEquipmentFilters();
}

void UserWindow::updateEquipmentTable(const QList<Equipment*>& equipmentList)
{
    clearEquipmentTable();

    equipmentTable->setRowCount(equipmentList.size());

    // Определяем общие размеры для всех элементов
    const int BUTTON_WIDTH = 120;
    const int BUTTON_HEIGHT = 28;
    const int REASON_WIDTH = 120; // Такая же ширина как у кнопок

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

        // Раскрашиваем статусы
        if (equipment->isAvailable()) {
            statusItem->setBackground(QColor(220, 255, 220)); // Зеленый
            statusItem->setForeground(QColor(0, 100, 0));
        } else if (equipment->isRented()) {
            statusItem->setBackground(QColor(255, 220, 220)); // Красный
            statusItem->setForeground(QColor(150, 0, 0));
        } else if (equipment->isUnderMaintenance()) {
            statusItem->setBackground(QColor(255, 255, 200)); // Желтый
            statusItem->setForeground(QColor(120, 120, 0));
        } else if (equipment->isBooked()) {
            statusItem->setBackground(QColor(255, 235, 156)); // Оранжевый
            statusItem->setForeground(QColor(153, 101, 21));
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
        actionsLayout->setSpacing(6); // Немного увеличили расстояние между элементами

        // Кнопки аренды/бронирования (только для доступного оборудования)
        if (equipment->isAvailable()) {
            // Кнопка "Арендовать"
            QPushButton* rentBtn = new QPushButton("Арендовать");
            rentBtn->setToolTip("Арендовать сейчас");
            rentBtn->setFixedSize(BUTTON_WIDTH, BUTTON_HEIGHT);
            rentBtn->setStyleSheet("QPushButton {"
                                   "    background-color: #4CAF50;"
                                   "    color: white;"
                                   "    border: none;"
                                   "    border-radius: 3px;"
                                   "    padding: 5px;"
                                   "    font-size: 11px;"
                                   "}"
                                   "QPushButton:hover {"
                                   "    background-color: #45a049;"
                                   "}"
                                   "QPushButton:pressed {"
                                   "    background-color: #3d8b40;"
                                   "}");
            rentBtn->setProperty("equipmentId", equipment->getId());
            rentBtn->setProperty("actionType", "rent");
            connect(rentBtn, &QPushButton::clicked, this, [this, equipment]() {
                onRentEquipmentClicked(equipment->getId(), equipment->getName());
            });

            // Кнопка "Забронировать"
            QPushButton* reserveBtn = new QPushButton("Забронировать");
            reserveBtn->setToolTip("Забронировать на будущее");
            reserveBtn->setFixedSize(BUTTON_WIDTH, BUTTON_HEIGHT);
            reserveBtn->setStyleSheet("QPushButton {"
                                      "    background-color: #2196F3;"
                                      "    color: white;"
                                      "    border: none;"
                                      "    border-radius: 3px;"
                                      "    padding: 5px;"
                                      "    font-size: 11px;"
                                      "}"
                                      "QPushButton:hover {"
                                      "    background-color: #0b7dda;"
                                      "}"
                                      "QPushButton:pressed {"
                                      "    background-color: #0a68b4;"
                                      "}");
            reserveBtn->setProperty("equipmentId", equipment->getId());
            reserveBtn->setProperty("actionType", "reserve");
            connect(reserveBtn, &QPushButton::clicked, this, [this, equipment]() {
                onReserveEquipmentClicked(equipment->getId(), equipment->getName());
            });

            actionsLayout->addWidget(rentBtn);
            actionsLayout->addWidget(reserveBtn);
            actionsLayout->addStretch();
        } else {
            // Для недоступного оборудования показываем причину
            QString reason;
            QString backgroundColor;
            QString textColor = "#666666";

            if (equipment->isRented()) {
                reason = "В аренде";
                backgroundColor = "#ffcccc"; // Светло-красный
            } else if (equipment->isUnderMaintenance()) {
                reason = "На обслуживании";
                backgroundColor = "#ffffcc"; // Светло-желтый
            } else if (equipment->isBooked()) {
                reason = "Забронировано";
                backgroundColor = "#e6f3ff"; // Светло-синий
                textColor = "#0066cc";
            }

            QLabel* unavailableLabel = new QLabel(reason);
            unavailableLabel->setFixedSize(REASON_WIDTH, BUTTON_HEIGHT);
            unavailableLabel->setAlignment(Qt::AlignCenter);
            unavailableLabel->setStyleSheet(QString(
                                                "QLabel {"
                                                "    color: %1;"
                                                "    font-weight: 500;"
                                                "    background-color: %2;"
                                                "    padding: 4px 8px;"
                                                "    border-radius: 3px;"
                                                "    border: 1px solid #ddd;"
                                                "    font-size: 11px;"
                                                "}"
                                                ).arg(textColor).arg(backgroundColor));

            // Кнопка "Забронировать"
            QPushButton* reserveBtn = new QPushButton("Забронировать");
            reserveBtn->setToolTip("Забронировать на будущее");
            reserveBtn->setFixedSize(BUTTON_WIDTH, BUTTON_HEIGHT);
            reserveBtn->setStyleSheet("QPushButton {"
                                      "    background-color: #2196F3;"
                                      "    color: white;"
                                      "    border: none;"
                                      "    border-radius: 3px;"
                                      "    padding: 5px;"
                                      "    font-size: 11px;"
                                      "}"
                                      "QPushButton:hover {"
                                      "    background-color: #0b7dda;"
                                      "}"
                                      "QPushButton:pressed {"
                                      "    background-color: #0a68b4;"
                                      "}");
            reserveBtn->setProperty("equipmentId", equipment->getId());
            reserveBtn->setProperty("actionType", "reserve");
            connect(reserveBtn, &QPushButton::clicked, this, [this, equipment]() {
                onReserveEquipmentClicked(equipment->getId(), equipment->getName());
            });

            actionsLayout->addWidget(unavailableLabel);
            actionsLayout->addWidget(reserveBtn);
            actionsLayout->addStretch();
        }

        equipmentTable->setCellWidget(i, 7, actionsWidget);
    }

    equipmentTable->resizeRowsToContents();

    for (int i = 0; i < equipmentTable->rowCount(); ++i) {
        equipmentTable->setRowHeight(i, BUTTON_HEIGHT + 8);
    }
}

void UserWindow::refreshDashboard()
{
    // Обновляем статистику
    QList<Rental*> userRentals = Rental::findMyPendingRentals(m_user->getId());
    int pendingCount = userRentals.size();
    Rental::clearRentalList(userRentals);

    activeRequestsLabel->setText(QString("Заявки в обработке: %1").arg(pendingCount));

    // Обновляем таблицу
    loadProcessingRequests();
}

void UserWindow::onRentEquipmentClicked(int equipmentId, const QString& status)
{
    bool success = RentalDialog::createNewRental(this, m_user->getId(), equipmentId);

    if (success) {
        // Обновляем таблицу оборудования
        loadEquipmentData();

        QMessageBox::information(this, "Успех",
                                 "Заявка на аренду создана успешно!\n\n"
                                 "Заявка отправлена на рассмотрение администратору. "
                                 "Статус заявки можно отслеживать во вкладке 'Мои заявки'.");
    }
    refreshDashboard();
}

void UserWindow::onReserveEquipmentClicked(int equipmentId, const QString& status)
{
    bool success = RentalDialog::createNewRental(this, m_user->getId(), equipmentId);

    if (success) {
        // Обновляем таблицу оборудования
        loadEquipmentData();

        QMessageBox::information(this, "Успех",
                                 "Заявка на бронирование создана успешно!\n\n"
                                 "Заявка отправлена на рассмотрение администратору. "
                                 "Статус заявки можно отслеживать во вкладке 'Мои заявки'.");
    }
    refreshDashboard();
}

void UserWindow::onEditEquipmentClicked(int equipmentId)
{
    Equipment* equipment = Equipment::findById(equipmentId);
    if (!equipment) {
        QMessageBox::warning(this, "Ошибка", "Оборудование не найдено!");
        return;
    }

    AddEditDialog dialog(equipment, this);

    if (dialog.exec() == QDialog::Accepted) {
        Equipment updatedEquipment = dialog.getEquipment();

        if (!Equipment::validateInventoryNumber(updatedEquipment.getInventoryNumber(), equipmentId)) {
            QMessageBox::critical(this, "Ошибка",
                                  "Инвентарный номер уже существует. Пожалуйста, введите другой номер.");
        } else {
            // Сохраняем изменения
            if (updatedEquipment.save()) {
                QMessageBox::information(this, "Успех", "Изменения сохранены!");
                loadEquipmentData(); // Перезагружаем список
            } else {
                QMessageBox::critical(this, "Ошибка",
                                      "Не удалось сохранить изменения.");
            }
        }
    }

    delete equipment;
}

void UserWindow::clearEquipmentTable()
{
    equipmentTable->setRowCount(0);
}

void UserWindow::applyEquipmentFilters()
{
    QString searchText = searchEquipmentEdit->text().trimmed().toLower();
    QString statusFilter = statusFilterCombo->currentText();
    QString typeFilter = typeFilterCombo->currentText();

    QList<Equipment*> filteredList;

    for (Equipment* equipment : m_allEquipment) {
        bool matches = true;

        // Фильтр по поиску
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

        // Фильтр по статусу
        if (matches && statusFilter != "Все статусы") {
            QString status = equipment->getStatus();
            if (statusFilter == "Свободно" && !equipment->isAvailable()) matches = false;
            else if (statusFilter == "Арендованно" && !equipment->isRented()) matches = false;
            else if (statusFilter == "Обслуживается" && !equipment->isUnderMaintenance()) matches = false;
        }

        // Фильтр по типу
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

void UserWindow::onEquipmentSearchTextChanged(const QString& text)
{
    Q_UNUSED(text);
    applyEquipmentFilters();
}

void UserWindow::onEquipmentStatusFilterChanged(int index)
{
    Q_UNUSED(index);
    applyEquipmentFilters();
}

void UserWindow::onEquipmentTypeFilterChanged(int index)
{
    Q_UNUSED(index);
    applyEquipmentFilters();
}

void UserWindow::onAddNewEquipmentClicked()
{
    AddEditDialog dialog(this);

    if (dialog.exec() == QDialog::Accepted) {
        Equipment equipment = dialog.getEquipment();

        // Проверяем уникальность инвентарного номера
        if (!Equipment::validateInventoryNumber(equipment.getInventoryNumber())) {
            QMessageBox::critical(this, "Ошибка",
                                  "Инвентарный номер уже существует. Пожалуйста, введите другой номер.");
            return;
        }

        // Сохраняем в БД
        if (equipment.save()) {
            QMessageBox::information(this, "Успех", "Оборудование успешно добавлено!");
            loadEquipmentData(); // Перезагружаем список
        } else {
            QMessageBox::critical(this, "Ошибка",
                                  "Не удалось добавить оборудование.");
        }
    }
}

void UserWindow::onDeleteEquipmentClicked(int equipmentId)
{
    Equipment* equipment = Equipment::findById(equipmentId);
    if (!equipment) {
        QMessageBox::warning(this, "Ошибка", "Оборудование не найдено!");
        return;
    }

    // Проверяем, можно ли удалить
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
            loadEquipmentData(); // Перезагружаем данные
        } else {
            QMessageBox::critical(this, "Ошибка", "Не удалось удалить оборудование!");
        }
    }

    delete equipment;
}

void UserWindow::createRentalsTab()
{
    rentalsTab = new QWidget();
    QVBoxLayout *mainLayout = new QVBoxLayout(rentalsTab);

    // Панель фильтров
    QHBoxLayout *filterLayout = new QHBoxLayout();

    // Поиск по названию оборудования
    searchRentalsEdit = new QLineEdit();
    searchRentalsEdit->setPlaceholderText("Поиск по оборудованию...");
    searchRentalsEdit->setClearButtonEnabled(true);

    // Фильтр по статусу аренды
    statusFilterCombo = new QComboBox();
    statusFilterCombo->addItem("Все статусы");
    statusFilterCombo->addItem(Rental::STATUS_ACTIVE);
    statusFilterCombo->addItem(Rental::STATUS_COMPLETED);
    statusFilterCombo->addItem(Rental::STATUS_CANCELED);
    statusFilterCombo->addItem(Rental::STATUS_RESERVED);

    filterLayout->addWidget(new QLabel("Поиск:"));
    filterLayout->addWidget(searchRentalsEdit, 2);
    filterLayout->addWidget(new QLabel("Статус:"));
    filterLayout->addWidget(statusFilterCombo);
    filterLayout->addStretch();

    // Таблица аренд
    rentalsTable = new QTableWidget();
    rentalsTable->setColumnCount(7);
    rentalsTable->setHorizontalHeaderLabels({
        "ID",
        "Оборудование",
        "Дата начала",
        "Дата окончания",
        "Дней",
        "Сумма",
        "Статус"
    });

    // Настройка таблицы
    rentalsTable->verticalHeader()->setVisible(false);
    rentalsTable->horizontalHeader()->setStretchLastSection(false);
    rentalsTable->setSelectionBehavior(QAbstractItemView::SelectRows);
    rentalsTable->setEditTriggers(QAbstractItemView::NoEditTriggers);
    rentalsTable->setAlternatingRowColors(true);

    // Скрываем колонку ID
    rentalsTable->setColumnHidden(0, true);

    // Настройка ширины колонок
    rentalsTable->setColumnWidth(1, 200); // Оборудование
    rentalsTable->setColumnWidth(2, 100); // Дата начала
    rentalsTable->setColumnWidth(3, 100); // Дата окончания
    rentalsTable->setColumnWidth(4, 70);  // Дней
    rentalsTable->setColumnWidth(5, 100); // Сумма
    rentalsTable->setColumnWidth(6, 120); // Статус
    rentalsTable->horizontalHeader()->setStretchLastSection(true);

    // Подключаем сигналы
    connect(searchRentalsEdit, &QLineEdit::textChanged,
            this, &UserWindow::onRentalsSearchTextChanged);
    connect(statusFilterCombo, QOverload<int>::of(&QComboBox::currentIndexChanged),
            this, &UserWindow::onRentalsStatusFilterChanged);

    mainLayout->addLayout(filterLayout);
    mainLayout->addWidget(rentalsTable);

    mainTabWidget->addTab(rentalsTab, "📜 Мои аренды");

    loadRentalsData();
}

void UserWindow::loadRentalsData()
{
    // Очищаем предыдущие данные
    clearRentalsTable();
    Rental::clearRentalList(m_allRentals);

    // Загружаем все аренды пользователя (историю, исключая заявки в обработке)
    m_allRentals = Rental::findMyRentals(m_user->getId());

    // Применяем текущие фильтры
    applyRentalsFilters();
}

void UserWindow::applyRentalsFilters()
{
    QList<Rental*> filteredList;

    QString searchText = searchRentalsEdit->text().trimmed().toLower();
    QString statusFilter = statusFilterCombo->currentText();

    // Применяем фильтры
    for (Rental* rental : m_allRentals) {
        if (!rental) continue;

        // Фильтр по поиску
        if (!searchText.isEmpty()) {
            QString equipmentName = rental->getEquipmentName().toLower();
            if (!equipmentName.contains(searchText)) {
                continue;
            }
        }

        // Фильтр по статусу
        if (statusFilter != "Все статусы" && rental->getStatus() != statusFilter) {
            continue;
        }

        filteredList.append(rental);
    }

    // Обновляем таблицу
    updateRentalsTable(filteredList);
}

void UserWindow::updateRentalsTable(const QList<Rental*>& rentalsList)
{
    clearRentalsTable();

    rentalsTable->setRowCount(rentalsList.size());

    for (int i = 0; i < rentalsList.size(); ++i) {
        Rental* rental = rentalsList[i];

        // ID (скрытая колонка)
        QTableWidgetItem* idItem = new QTableWidgetItem(QString::number(rental->getId()));
        rentalsTable->setItem(i, 0, idItem);

        // Оборудование
        rentalsTable->setItem(i, 1, new QTableWidgetItem(rental->getEquipmentName()));

        // Дата начала
        rentalsTable->setItem(i, 2, new QTableWidgetItem(
                                        rental->getStartDate().toString("dd.MM.yyyy")));

        // Дата окончания
        rentalsTable->setItem(i, 3, new QTableWidgetItem(
                                        rental->getEndDate().toString("dd.MM.yyyy")));

        // Количество дней
        int days = rental->getRentalDays();
        rentalsTable->setItem(i, 4, new QTableWidgetItem(QString::number(days)));

        // Сумма
        QString priceText = QString("%1 ₽").arg(rental->getTotalPrice(), 0, 'f', 2);
        rentalsTable->setItem(i, 5, new QTableWidgetItem(priceText));

        // Статус
        QTableWidgetItem* statusItem = new QTableWidgetItem(rental->getStatus());

        // Раскрашиваем статусы
        if (rental->getStatus() == Rental::STATUS_ACTIVE) {
            statusItem->setBackground(QColor(220, 255, 220)); // Зеленый
            statusItem->setForeground(QColor(0, 100, 0));
        } else if (rental->getStatus() == Rental::STATUS_COMPLETED) {
            statusItem->setBackground(QColor(240, 240, 240)); // Серый
            statusItem->setForeground(QColor(100, 100, 100));
        } else if (rental->getStatus() == Rental::STATUS_CANCELED) {
            statusItem->setBackground(QColor(255, 220, 220)); // Красный
            statusItem->setForeground(QColor(150, 0, 0));
        } else if (rental->getStatus() == Rental::STATUS_RESERVED) {
            statusItem->setBackground(QColor(255, 255, 200)); // Желтый
            statusItem->setForeground(QColor(120, 120, 0));
        }

        rentalsTable->setItem(i, 6, statusItem);
    }

    rentalsTable->resizeRowsToContents();
}

void UserWindow::clearRentalsTable()
{
    for (int i = 0; i < rentalsTable->rowCount(); ++i) {
        for (int j = 0; j < rentalsTable->columnCount(); ++j) {
            QTableWidgetItem* item = rentalsTable->takeItem(i, j);
            if (item) delete item;
        }
    }
    rentalsTable->setRowCount(0);
}

void UserWindow::onRentalsSearchTextChanged(const QString& text)
{
    Q_UNUSED(text);
    applyRentalsFilters();
}

void UserWindow::onRentalsStatusFilterChanged(int index)
{
    Q_UNUSED(index);
    applyRentalsFilters();
}



