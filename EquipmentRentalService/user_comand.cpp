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

void MainWindow::setupClientUI()
{
    setupUI();

    // Скрываем вкладки, которые не нужны клиенту
    if (mainTabWidget) {
        // Вкладки 3 (клиенты) и 4 (отчеты) скрываем для клиента
        mainTabWidget->setTabVisible(3, false); // Клиенты
        mainTabWidget->setTabVisible(4, false); // Отчеты

        // Меняем заголовки для клиента
        mainTabWidget->setTabText(0, "🏠 Главная");
        mainTabWidget->setTabText(1, "🔧 Каталог техники");
        mainTabWidget->setTabText(2, "📋 Мои аренды");
    }
}
