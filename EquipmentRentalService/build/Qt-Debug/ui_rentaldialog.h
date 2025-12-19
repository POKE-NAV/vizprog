/********************************************************************************
** Form generated from reading UI file 'rentaldialog.ui'
**
** Created by: Qt User Interface Compiler version 6.9.2
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_RENTALDIALOG_H
#define UI_RENTALDIALOG_H

#include <QtCore/QVariant>
#include <QtWidgets/QApplication>
#include <QtWidgets/QDialog>

QT_BEGIN_NAMESPACE

class Ui_RentalDialog
{
public:

    void setupUi(QDialog *RentalDialog)
    {
        if (RentalDialog->objectName().isEmpty())
            RentalDialog->setObjectName("RentalDialog");
        RentalDialog->resize(400, 300);

        retranslateUi(RentalDialog);

        QMetaObject::connectSlotsByName(RentalDialog);
    } // setupUi

    void retranslateUi(QDialog *RentalDialog)
    {
        RentalDialog->setWindowTitle(QCoreApplication::translate("RentalDialog", "Dialog", nullptr));
    } // retranslateUi

};

namespace Ui {
    class RentalDialog: public Ui_RentalDialog {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_RENTALDIALOG_H
