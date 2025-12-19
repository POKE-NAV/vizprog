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
#include "adminwindow.h"
#include "userwindow.h"

MainWindow::MainWindow(User* user, QWidget *parent)
    : QMainWindow(parent), m_user(user)
{

}

MainWindow* MainWindow::createWindow(User* user, QWidget* parent)
{
    // Проверяем, что пользователь существует и валиден
    if (!user || !user->isValid()) {
        QMessageBox::critical(nullptr, "Ошибка входа",
                              "Не удалось войти в систему. Пользователь не валиден.");
        return nullptr;
    }

    if (user->isAdmin()) {
        return new AdminWindow(user, parent);
    } else {
        return new UserWindow(user, parent);
    }
}

MainWindow::~MainWindow()
{

}


void MainWindow::onLogoutClicked()
{
    QMessageBox::StandardButton reply;
    reply = QMessageBox::question(this, "Подтверждение выхода",
                                  "Вы уверены, что хотите выйти из аккаунта?",
                                  QMessageBox::Yes | QMessageBox::No);

    if (reply == QMessageBox::Yes) {
        // Очищаем память
        if (m_user) {
            delete m_user;
            m_user = nullptr;
        }

        emit restartApplication(); // Отправляем сигнал в main.cpp

        // Закрываем окно
        this->close();
    }
}
