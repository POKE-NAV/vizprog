#include <QApplication>
#include <QMessageBox>
#include "logindialog.h"
#include "mainwindow.h"
#include <QDebug>

int main(int argc, char *argv[])
{
    QApplication app(argc, argv);

    bool restartNeeded = false;

    do {
        restartNeeded = false;

        // Показываем окно входа
        LoginDialog loginDialog;

        if (loginDialog.exec() == QDialog::Accepted) {
            User* user = loginDialog.getUser();

            if (user && user->isValid()) {

                // Создаем главное окно
                MainWindow *mainWindow = MainWindow::createWindow(user);

                if(mainWindow)
                {
                    // ПОДКЛЮЧАЕМ СИГНАЛ перезапуска
                    QObject::connect(mainWindow, &MainWindow::restartApplication,
                                     [&app, &restartNeeded]() {
                                         restartNeeded = true;
                                         app.quit(); // Завершаем текущий event loop
                                     });

                    mainWindow->show();

                    // Запускаем event loop для этого окна
                    app.exec();

                    // После закрытия MainWindow
                    delete mainWindow; // Освобождаем память
                }else {
                    QMessageBox::warning(nullptr, "Ошибка",
                                         "Не удалось создать главное окно");
                }
            }
        }

    } while (restartNeeded); // Повторяем, если нужен перезапуск

    return 0;
}
