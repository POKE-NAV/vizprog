#ifndef ADDEDITDIALOG_H
#define ADDEDITDIALOG_H

#include <QDialog>
#include <QLineEdit>
#include <QComboBox>
#include <QDoubleValidator>
#include <QDialogButtonBox>
#include <QFormLayout>
#include <QLabel>
#include <QGroupBox>
#include <QVBoxLayout>
#include "equipment.h"

class AddEditDialog : public QDialog
{
    Q_OBJECT

public:
    // Для добавления нового оборудования
    explicit AddEditDialog(QWidget *parent = nullptr);
    // Для редактирования существующего
    explicit AddEditDialog(Equipment* equipment, QWidget *parent = nullptr);
    ~AddEditDialog();

    Equipment getEquipment() const;

private slots:
    void onSaveClicked();
    void validateInputs();

private:
    // Виджеты
    QLineEdit* m_nameEdit;
    QComboBox* m_typeCombo;
    QComboBox* m_statusCombo;
    QLineEdit* m_priceEdit;
    QLineEdit* m_depositEdit;
    QLineEdit* m_inventoryEdit;
    QDialogButtonBox* m_buttonBox;

    Equipment m_equipment;
    bool m_isEditMode = false;
    int m_editId = 0;

    void setupUi();
    void loadEquipmentData(const Equipment& equipment);
};

#endif // ADDEDITDIALOG_H
