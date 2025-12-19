#include "addeditdialog.h"
#include <QMessageBox>
#include <QDebug>
#include <QPushButton>
#include "equipment.h"

AddEditDialog::AddEditDialog(QWidget *parent) :
    QDialog(parent),
    m_isEditMode(false)
{
    setupUi();
    setWindowTitle("Добавить оборудование");
    setMinimumWidth(400);
}

AddEditDialog::AddEditDialog(Equipment* equipment, QWidget *parent) :
    QDialog(parent),
    m_isEditMode(true),
    m_editId(equipment->getId())
{
    setupUi();
    setWindowTitle("Редактировать оборудование");
    setMinimumWidth(400);
    loadEquipmentData(*equipment);
}

AddEditDialog::~AddEditDialog()
{
}

void AddEditDialog::setupUi()
{
    QVBoxLayout* mainLayout = new QVBoxLayout(this);

    // Группа для полей ввода
    QGroupBox* groupBox = new QGroupBox("Информация об оборудовании");
    QFormLayout* formLayout = new QFormLayout(groupBox);

    // Название
    m_nameEdit = new QLineEdit();
    m_nameEdit->setPlaceholderText("Введите название");
    formLayout->addRow("Название:", m_nameEdit);

    // Тип
    m_typeCombo = new QComboBox();
    m_typeCombo->setEditable(true);
    m_typeCombo->addItems(Equipment::getAllTypes());
    m_typeCombo->addItem("Другой");
    formLayout->addRow("Тип:", m_typeCombo);

    // Статус
    m_statusCombo = new QComboBox();
    m_statusCombo->addItem(Equipment::STATUS_AVAILABLE);
    m_statusCombo->addItem(Equipment::STATUS_RENTED);
    m_statusCombo->addItem(Equipment::STATUS_MAINTENANCE);
    formLayout->addRow("Статус:", m_statusCombo);

    // Цена за день
    m_priceEdit = new QLineEdit();
    m_priceEdit->setPlaceholderText("0.00");
    m_priceEdit->setValidator(new QDoubleValidator(0, 1000000, 2, this));
    formLayout->addRow("Цена за день (руб.):", m_priceEdit);

    // Залог
    m_depositEdit = new QLineEdit();
    m_depositEdit->setPlaceholderText("0.00");
    m_depositEdit->setText("0");
    m_depositEdit->setValidator(new QDoubleValidator(0, 1000000, 2, this));
    formLayout->addRow("Залог (руб.):", m_depositEdit);

    // Инвентарный номер
    m_inventoryEdit = new QLineEdit();
    m_inventoryEdit->setPlaceholderText("Уникальный номер");
    formLayout->addRow("Инвентарный номер:", m_inventoryEdit);

    mainLayout->addWidget(groupBox);

    // Кнопки OK/Cancel
    m_buttonBox = new QDialogButtonBox(QDialogButtonBox::Ok | QDialogButtonBox::Cancel);
    mainLayout->addWidget(m_buttonBox);

    // Подключаем сигналы
    connect(m_buttonBox, &QDialogButtonBox::accepted, this, &AddEditDialog::onSaveClicked);
    connect(m_buttonBox, &QDialogButtonBox::rejected, this, &QDialog::reject);

    // Валидация в реальном времени
    connect(m_nameEdit, &QLineEdit::textChanged, this, &AddEditDialog::validateInputs);
    connect(m_inventoryEdit, &QLineEdit::textChanged, this, &AddEditDialog::validateInputs);
    connect(m_priceEdit, &QLineEdit::textChanged, this, &AddEditDialog::validateInputs);

    // Кнопка OK изначально неактивна
    m_buttonBox->button(QDialogButtonBox::Ok)->setEnabled(false);
}

void AddEditDialog::loadEquipmentData(const Equipment& equipment)
{
    m_nameEdit->setText(equipment.getName());
    m_typeCombo->setCurrentText(equipment.getType());
    m_statusCombo->setCurrentText(equipment.getStatus());
    m_priceEdit->setText(QString::number(equipment.getPricePerDay(), 'f', 2));
    m_depositEdit->setText(QString::number(equipment.getDeposit(), 'f', 2));
    m_inventoryEdit->setText(equipment.getInventoryNumber());
}

void AddEditDialog::onSaveClicked()
{
    // Сохраняем данные в объект Equipment
    m_equipment.setName(m_nameEdit->text().trimmed());
    m_equipment.setType(m_typeCombo->currentText().trimmed());
    m_equipment.setStatus(m_statusCombo->currentText());
    m_equipment.setPricePerDay(m_priceEdit->text().toDouble());
    m_equipment.setDeposit(m_depositEdit->text().toDouble());
    m_equipment.setInventoryNumber(m_inventoryEdit->text().trimmed());

    if (m_isEditMode) {
        // Для редактирования устанавливаем ID
        Equipment temp = m_equipment;
        temp = Equipment(m_editId,
                         temp.getName(),
                         temp.getType(),
                         temp.getStatus(),
                         temp.getPricePerDay(),
                         temp.getInventoryNumber(),
                         temp.getDeposit());
        m_equipment = temp;
    }

    accept();
}

void AddEditDialog::validateInputs()
{
    bool isValid = true;

    // Проверка названия
    if (m_nameEdit->text().trimmed().isEmpty()) {
        isValid = false;
    }

    // Проверка инвентарного номера
    if (m_inventoryEdit->text().trimmed().isEmpty()) {
        isValid = false;
    }

    // Проверка цены
    if (m_priceEdit->text().toDouble() <= 0) {
        isValid = false;
    }

    // Проверка уникальности инвентарного номера (только для добавления)
    if (!m_isEditMode && !Equipment::validateInventoryNumber(m_inventoryEdit->text())) {
        isValid = false;
    }

    m_buttonBox->button(QDialogButtonBox::Ok)->setEnabled(isValid);
}

Equipment AddEditDialog::getEquipment() const
{
    return m_equipment;
}
