/********************************************************************************
** Form generated from reading UI file 'addeditdialog.ui'
**
** Created by: Qt User Interface Compiler version 6.9.2
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_ADDEDITDIALOG_H
#define UI_ADDEDITDIALOG_H

#include <QtCore/QVariant>
#include <QtWidgets/QApplication>
#include <QtWidgets/QDialog>

QT_BEGIN_NAMESPACE

class Ui_Addeditdialog
{
public:

    void setupUi(QDialog *Addeditdialog)
    {
        if (Addeditdialog->objectName().isEmpty())
            Addeditdialog->setObjectName("Addeditdialog");
        Addeditdialog->resize(400, 300);

        retranslateUi(Addeditdialog);

        QMetaObject::connectSlotsByName(Addeditdialog);
    } // setupUi

    void retranslateUi(QDialog *Addeditdialog)
    {
        Addeditdialog->setWindowTitle(QCoreApplication::translate("Addeditdialog", "Dialog", nullptr));
    } // retranslateUi

};

namespace Ui {
    class Addeditdialog: public Ui_Addeditdialog {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_ADDEDITDIALOG_H
