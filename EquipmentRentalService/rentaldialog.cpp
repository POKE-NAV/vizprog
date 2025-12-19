#include "rentaldialog.h"
#include "rental.h"
#include "equipment.h"
#include "user.h"
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QFormLayout>
#include <QGroupBox>
#include <QDateEdit>
#include <QSpinBox>
#include <QPushButton>
#include <QLabel>
#include <QMessageBox>
#include <QDebug>
#include <QCalendarWidget>

// Конструктор для новой заявки
RentalDialog::RentalDialog(QWidget *parent, int clientId, int equipmentId)
    : QDialog(parent)
    , m_clientId(clientId)
    , m_equipmentId(equipmentId)
    , m_dailyPrice(0.0)
    , m_depositAmount(0.0)
    , m_days(1)
    , m_totalPrice(0.0)
    , m_totalWithDeposit(0.0)
    , m_discountPercent(0.0)
    , m_discountedPrice(0.0)
    , m_isAvailable(false)
{
    qDebug() << "[RentalDialog] Создание диалога:"
             << "Client ID:" << m_clientId
             << "Equipment ID:" << equipmentId;

    setWindowTitle("Новая заявка на аренду");
    setMinimumWidth(500);
    setModal(true);

    setupUI();
    loadEquipmentData();
    loadClientData();

    // Устанавливаем даты по умолчанию
    QDate today = QDate::currentDate();
    m_startDateEdit->setDate(today);
    m_endDateEdit->setDate(today.addDays(1));

    calculatePrice();
    validateDates();
    updateStatusMessages();
}

RentalDialog::~RentalDialog()
{
    // Автоматическое освобождение памяти через parent-child систему Qt
}

void RentalDialog::setupUI()
{
    QVBoxLayout *mainLayout = new QVBoxLayout(this);
    mainLayout->setSpacing(10);
    mainLayout->setContentsMargins(15, 15, 15, 15);

    // Заголовок
    QLabel *titleLabel = new QLabel("Новая заявка на аренду");
    titleLabel->setAlignment(Qt::AlignCenter);
    titleLabel->setStyleSheet(
        "QLabel {"
        "    font-size: 18px;"
        "    font-weight: bold;"
        "    color: #2c3e50;"
        "    padding: 10px;"
        "    border-bottom: 2px solid #3498db;"
        "}"
        );
    mainLayout->addWidget(titleLabel);

    // Информация об оборудовании и клиенте
    QGroupBox *infoGroup = new QGroupBox("Информация");
    infoGroup->setStyleSheet(
        "QGroupBox {"
        "    font-weight: bold;"
        "    border: 1px solid #bdc3c7;"
        "    border-radius: 5px;"
        "    margin-top: 10px;"
        "}"
        "QGroupBox::title {"
        "    subcontrol-origin: margin;"
        "    left: 10px;"
        "    padding: 0 5px 0 5px;"
        "}"
        );

    QFormLayout *infoLayout = new QFormLayout(infoGroup);
    infoLayout->setSpacing(8);

    m_equipmentInfoLabel = new QLabel();
    m_clientInfoLabel = new QLabel();
    m_availabilityLabel = new QLabel("Проверка доступности...");
    m_statusMessageLabel = new QLabel();

    m_equipmentInfoLabel->setStyleSheet("font-weight: normal;");
    m_clientInfoLabel->setStyleSheet("font-weight: normal;");
    m_availabilityLabel->setStyleSheet("color: #e67e22; font-weight: bold;");
    m_statusMessageLabel->setStyleSheet("color: #7f8c8d; font-style: italic;");

    infoLayout->addRow("Оборудование:", m_equipmentInfoLabel);
    infoLayout->addRow("Клиент:", m_clientInfoLabel);
    infoLayout->addRow("Доступность:", m_availabilityLabel);
    infoLayout->addRow("", m_statusMessageLabel);

    mainLayout->addWidget(infoGroup);

    // Даты аренды
    QGroupBox *datesGroup = new QGroupBox("Период аренды");
    datesGroup->setStyleSheet(infoGroup->styleSheet());

    QFormLayout *datesLayout = new QFormLayout(datesGroup);
    datesLayout->setSpacing(8);

    m_startDateEdit = new QDateEdit();
    m_startDateEdit->setCalendarPopup(true);
    m_startDateEdit->setDisplayFormat("dd.MM.yyyy");
    m_startDateEdit->setMinimumDate(QDate::currentDate());

    m_endDateEdit = new QDateEdit();
    m_endDateEdit->setCalendarPopup(true);
    m_endDateEdit->setDisplayFormat("dd.MM.yyyy");
    // m_endDateEdit->setMinimumDate(QDate::currentDate().addDays(1));
    m_endDateEdit->setMinimumDate(QDate::currentDate());

    m_daysSpinBox = new QSpinBox();
    m_daysSpinBox->setMinimum(1);
    m_daysSpinBox->setMaximum(365);
    m_daysSpinBox->setValue(1);
    m_daysSpinBox->setSuffix(" дней");

    // Календарь для выбора дат
    QCalendarWidget *start = new QCalendarWidget();
    start->setFirstDayOfWeek(Qt::Monday);
    QCalendarWidget *end = new QCalendarWidget();
    end->setFirstDayOfWeek(Qt::Monday);
    m_startDateEdit->setCalendarWidget(start);
    m_endDateEdit->setCalendarWidget(end);

    datesLayout->addRow("Дата начала:", m_startDateEdit);
    datesLayout->addRow("Дата окончания:", m_endDateEdit);
    datesLayout->addRow("Количество дней:", m_daysSpinBox);

    mainLayout->addWidget(datesGroup);

    // Стоимость
    QGroupBox *priceGroup = new QGroupBox("Расчет стоимости");
    priceGroup->setStyleSheet(infoGroup->styleSheet());

    QFormLayout *priceLayout = new QFormLayout(priceGroup);
    priceLayout->setSpacing(8);

    m_dailyPriceLabel = new QLabel("0.00 ₽");
    m_discountLabel = new QLabel("0%");
    m_discountValueLabel = new QLabel("0.00 ₽");
    m_totalPriceLabel = new QLabel("0.00 ₽");
    m_depositLabel = new QLabel("0.00 ₽");
    m_totalWithDepositLabel = new QLabel("0.00 ₽");

    priceLayout->addRow("Цена за сутки:", m_dailyPriceLabel);
    priceLayout->addRow("Скидка (%):", m_discountLabel);
    priceLayout->addRow("Сумма скидки:", m_discountValueLabel);
    priceLayout->addRow("Стоимость аренды:", m_totalPriceLabel);
    priceLayout->addRow("Залог (депозит):", m_depositLabel);
    priceLayout->addRow("ИТОГО к оплате:", m_totalWithDepositLabel);

    mainLayout->addWidget(priceGroup);

    // Кнопки
    QHBoxLayout *buttonLayout = new QHBoxLayout();
    buttonLayout->addStretch();

    m_cancelButton = new QPushButton("Отмена");
    m_cancelButton->setStyleSheet(
        "QPushButton {"
        "    background-color: #e74c3c;"
        "    color: white;"
        "    font-weight: bold;"
        "    padding: 8px 20px;"
        "    border: none;"
        "    border-radius: 4px;"
        "    min-width: 100px;"
        "}"
        "QPushButton:hover { background-color: #c0392b; }"
        "QPushButton:pressed { background-color: #a93226; }"
        );

    m_confirmButton = new QPushButton("Создать заявку");
    m_confirmButton->setStyleSheet(
        "QPushButton {"
        "    background-color: #2ecc71;"
        "    color: white;"
        "    font-weight: bold;"
        "    padding: 8px 20px;"
        "    border: none;"
        "    border-radius: 4px;"
        "    min-width: 150px;"
        "}"
        "QPushButton:hover { background-color: #27ae60; }"
        "QPushButton:pressed { background-color: #229954; }"
        "QPushButton:disabled { background-color: #bdc3c7; }"
        );

    buttonLayout->addWidget(m_cancelButton);
    buttonLayout->addWidget(m_confirmButton);

    mainLayout->addLayout(buttonLayout);

    // Подключаем сигналы
    connect(m_startDateEdit, &QDateEdit::dateChanged, this, &RentalDialog::onDateChanged);
    connect(m_endDateEdit, &QDateEdit::dateChanged, this, &RentalDialog::onDateChanged);
    connect(m_daysSpinBox, QOverload<int>::of(&QSpinBox::valueChanged),
            this, &RentalDialog::onDateChanged);

    connect(m_confirmButton, &QPushButton::clicked, this, &RentalDialog::onConfirmClicked);
    connect(m_cancelButton, &QPushButton::clicked, this, &RentalDialog::onCancelClicked);
}

void RentalDialog::loadEquipmentData()
{
    Equipment* equipment = Equipment::findById(m_equipmentId);
    if (!equipment) {
        QMessageBox::critical(this, "Ошибка",
                              "Оборудование не найдено. Возможно, оно было удалено.");
        reject();
        return;
    }

    m_equipmentName = equipment->getName();
    m_dailyPrice = equipment->getPricePerDay();
    m_depositAmount = equipment->getDeposit();

    QString equipmentInfo = QString("%1\nТип: %2 | Инв. №: %3")
                                .arg(equipment->getName())
                                .arg(equipment->getType())
                                .arg(equipment->getInventoryNumber());

    m_equipmentInfoLabel->setText(equipmentInfo);
    m_dailyPriceLabel->setText(QString("%1 ₽").arg(m_dailyPrice, 0, 'f', 2));
    m_depositLabel->setText(QString("%1 ₽").arg(m_depositAmount, 0, 'f', 2));

    // Проверяем статус оборудования
    if (!equipment->isAvailable()) {
        m_availabilityLabel->setText("Недоступно для аренды");
        m_availabilityLabel->setStyleSheet("color: #e74c3c; font-weight: bold;");
        m_statusMessageLabel->setText(tr("Статус оборудования: %1").arg(equipment->getStatus()));
        m_confirmButton->setEnabled(false);
    }

    delete equipment;
}

void RentalDialog::loadClientData()
{
    qDebug() << "Загрузка клиента ID:" << m_clientId;
    User* client = User::getUserById(m_clientId);
    if (!client) {
        QMessageBox::critical(this, "Ошибка", "Клиент не найден.");
        reject();
        return;
    }

    m_clientName = client->getFullName();
    QString clientInfo = QString("%1\nЛогин: %2 | Телефон: %3")
                             .arg(client->getFullName())
                             .arg(client->getLogin())
                             .arg(client->getPhone());

    m_clientInfoLabel->setText(clientInfo);
    delete client;
}

void RentalDialog::onDateChanged()
{
    QDate startDate = m_startDateEdit->date();
    QDate endDate = m_endDateEdit->date();

    // Синхронизация дат и количества дней
    if (startDate > endDate) {
        if (sender() == m_startDateEdit) {
            endDate = startDate.addDays(m_daysSpinBox->value() - 1);
            m_endDateEdit->setDate(endDate);
        } else {
            startDate = endDate.addDays(-m_daysSpinBox->value() + 1);
            m_startDateEdit->setDate(startDate);
        }
    } else {
        m_days = startDate.daysTo(endDate) + 1;
        if (m_daysSpinBox->value() != m_days) {
            m_daysSpinBox->blockSignals(true);
            m_daysSpinBox->setValue(m_days);
            m_daysSpinBox->blockSignals(false);
        }
    }

    m_startDate = startDate;
    m_endDate = endDate;

    calculatePrice();
    validateDates();
    updateStatusMessages();
}

void RentalDialog::calculatePrice()
{
    // Используем новую функцию для расчета со скидкой
    updateDiscountedPrice();

    // Обновляем отображение
    m_dailyPriceLabel->setText(QString("%1 ₽").arg(m_dailyPrice, 0, 'f', 2));

    // Отображаем процент скидки
    if (m_discountPercent > 0) {
        m_discountLabel->setText(QString("%1%").arg(m_discountPercent, 0, 'f', 1));
        m_discountLabel->setStyleSheet("color: #27ae60; font-weight: bold;");
    } else {
        m_discountLabel->setText("0%");
        m_discountLabel->setStyleSheet("");
    }

    // Отображаем сумму скидки и общую стоимость
    double discountAmount = m_totalPrice - m_discountedPrice;
    m_discountValueLabel->setText(QString("-%1 ₽").arg(discountAmount, 0, 'f', 2));

    // Показываем оригинальную и скидочную цену
    if (m_discountPercent > 0) {
        QString originalPrice = QString("<s>%1 ₽</s>").arg(m_totalPrice, 0, 'f', 2);
        QString discountedPrice = QString("%1 ₽").arg(m_discountedPrice, 0, 'f', 2);
        m_totalPriceLabel->setText(originalPrice + " → " + discountedPrice);
    } else {
        m_totalPriceLabel->setText(QString("%1 ₽").arg(m_totalPrice, 0, 'f', 2));
    }

    // Обновляем итоговую сумму с депозитом
    m_totalWithDepositLabel->setText(QString("%1 ₽").arg(m_totalWithDeposit, 0, 'f', 2));
}

double RentalDialog::calculateDiscount(int days) const
{
    // Правила скидок:
    // - от 7 до 13 дней: 10% скидка
    // - от 14 до 20 дней: 15% скидка
    // - от 21 дня и более: 18% скидка

    if (days >= 21) {
        return 18.0; // 18% скидка
    } else if (days >= 14) {
        return 15.0; // 15% скидка
    } else if (days >= 7) {
        return 10.0; // 10% скидка
    }

    return 0.0; // Нет скидки
}

void RentalDialog::updateDiscountedPrice()
{
    // Рассчитываем процент скидки
    m_discountPercent = calculateDiscount(m_days);

    // Рассчитываем общую стоимость без скидки
    m_totalPrice = m_dailyPrice * m_days;

    // Рассчитываем скидку в деньгах
    double discountAmount = m_totalPrice * (m_discountPercent / 100.0);

    // Рассчитываем стоимость со скидкой
    m_discountedPrice = m_totalPrice - discountAmount;

    // Пересчитываем итоговую сумму с депозитом
    m_totalWithDeposit = m_discountedPrice + m_depositAmount;
}

void RentalDialog::validateDates()
{
    m_isAvailable = false;

    if (!m_startDate.isValid() || !m_endDate.isValid()) {
        m_availabilityLabel->setText("Некорректные даты");
        m_availabilityLabel->setStyleSheet("color: #e74c3c; font-weight: bold;");
        return;
    }

    // Проверяем доступность оборудования через класс Rental
    // Для новой заявки передаем 0 как excludeRentalId
    if (Rental::isEquipmentAvailableForDates(m_equipmentId, m_startDate, m_endDate, 0)) {
        m_isAvailable = true;
        m_availabilityLabel->setText("Доступно для аренды");
        m_availabilityLabel->setStyleSheet("color: #27ae60; font-weight: bold;");
    } else {
        m_availabilityLabel->setText("Недоступно в выбранные даты");
        m_availabilityLabel->setStyleSheet("color: #e74c3c; font-weight: bold;");
    }
}

void RentalDialog::updateStatusMessages()
{
    QDate today = QDate::currentDate();
    QString message;

    if (m_startDate == today) {
        message = "Аренда начинается сегодня";
        m_statusMessageLabel->setStyleSheet("color: #f39c12; font-weight: bold;");
    } else if (m_startDate < today) {
        message = "Дата начала в прошлом (некорректно)";
        m_statusMessageLabel->setStyleSheet("color: #e74c3c; font-weight: bold;");
        m_confirmButton->setEnabled(false);
    } else {
        int daysUntilStart = today.daysTo(m_startDate);
        message = QString("До начала аренды осталось %1 дней").arg(daysUntilStart);
        m_statusMessageLabel->setStyleSheet("color: #3498db;");
        m_confirmButton->setEnabled(m_isAvailable);
    }

    m_statusMessageLabel->setText(message);
}

bool RentalDialog::validateInput()
{
    if (!m_isAvailable) {
        showAvailabilityError();
        return false;
    }

    if (m_startDate < QDate::currentDate()) {
        QMessageBox::warning(this, "Ошибка",
                             "Дата начала не может быть в прошлом.");
        return false;
    }

    if (m_endDate <= m_startDate) {
        QMessageBox::warning(this, "Ошибка",
                             "Дата окончания должна быть позже даты начала.");
        return false;
    }

    if (m_days > 365) {
        QMessageBox::warning(this, "Ошибка",
                             "Максимальный срок аренды - 365 дней.");
        return false;
    }

    if (m_totalPrice <= 0) {
        QMessageBox::warning(this, "Ошибка",
                             "Некорректная стоимость аренды.");
        return false;
    }

    return true;
}

void RentalDialog::showAvailabilityError()
{
    QString message = QString(
                          "Оборудование '%1' недоступно в выбранный период:\n"
                          "  • С %2 по %3 (%4 дней)\n\n"
                          "Пожалуйста, выберите другие даты."
                          ).arg(m_equipmentName)
                          .arg(m_startDate.toString("dd.MM.yyyy"))
                          .arg(m_endDate.toString("dd.MM.yyyy"))
                          .arg(m_days);

    QMessageBox::warning(this, "Оборудование недоступно", message);
}

bool RentalDialog::saveRental()
{
    // Создаем объект Rental
    Rental* rental = new Rental();
    rental->setClientId(m_clientId);

    // Получаем логин клиента
    User* client = User::getUserById(m_clientId);
    if (client) {
        rental->setClientLogin(client->getLogin());
        delete client;
    } else {
        rental->setClientLogin("unknown");
    }

    rental->setEquipmentId(m_equipmentId);
    rental->setEquipmentName(m_equipmentName);
    rental->setStartDate(m_startDate);
    rental->setEndDate(m_endDate);
    rental->setTotalPrice(m_totalWithDeposit);
    rental->setDeposit(m_depositAmount);
    rental->setDepositReturned(false);

    // Все новые заявки получают статус "В обработке"
    rental->setStatus(Rental::STATUS_IN_PROCESSING);
    rental->setApplicationStatus(Rental::APPLICATION_STATUS_IN_PROCESSING);

    qDebug() << "[RentalDialog] Создание заявки:"
             << "Клиент ID:" << m_clientId
             << "Оборудование:" << m_equipmentName
             << "Даты:" << m_startDate.toString("dd.MM.yyyy") << "-" << m_endDate.toString("dd.MM.yyyy")
             << "Сумма:" << m_totalPrice;

    // Сохраняем в базу данных
    bool success = rental->insertIntoDatabase();

    if (success) {
        qDebug() << "[RentalDialog] Заявка создана успешно, ID:" << rental->getId();
    } else {
        qDebug() << "[RentalDialog] Ошибка создания заявки";
    }

    delete rental;
    return success;
}

void RentalDialog::onConfirmClicked()
{
    if (!validateInput()) {
        return;
    }

    QString confirmMessage = QString("Создать заявку на аренду оборудования?\n\n"
                                     "Оборудование: %1\n"
                                     "Период: %2 - %3 (%4 дней)\n"
                                     "Сумма к оплате: %5 ₽\n"
                                     "Залог: %6 ₽")
                                 .arg(m_equipmentName)
                                 .arg(m_startDate.toString("dd.MM.yyyy"))
                                 .arg(m_endDate.toString("dd.MM.yyyy"))
                                 .arg(m_days)
                                 .arg(m_totalPrice, 0, 'f', 2)
                                 .arg(m_depositAmount, 0, 'f', 2);

    QMessageBox::StandardButton reply = QMessageBox::question(
        this,
        "Подтверждение",
        confirmMessage,
        QMessageBox::Yes | QMessageBox::No,
        QMessageBox::Yes
        );

    if (reply != QMessageBox::Yes) {
        return;
    }

    // Сохраняем заявку
    bool success = saveRental();

    if (success) {
        QString successMessage = "Заявка на аренду создана успешно!\n\n"
                                 "Заявка отправлена на рассмотрение администратору. "
                                 "Вы сможете отслеживать её статус во вкладке 'Мои заявки'.";

        QMessageBox::information(this, "Успех", successMessage);
        accept();
    } else {
        QMessageBox::critical(this, "Ошибка",
                              "Не удалось создать заявку.\n"
                              "Пожалуйста, попробуйте позже или обратитесь к администратору.");
    }
}

void RentalDialog::onCancelClicked()
{
    reject();
}

// Статический метод для удобства
bool RentalDialog::createNewRental(QWidget *parent, int clientId, int equipmentId)
{
    RentalDialog dialog(parent, clientId, equipmentId);
    return dialog.exec() == QDialog::Accepted;
}
