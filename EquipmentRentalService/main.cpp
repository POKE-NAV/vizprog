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

        LoginDialog loginDialog;

        if (loginDialog.exec() == QDialog::Accepted) {
            User* user = loginDialog.getUser();

            if (user && user->isValid()) {

                MainWindow *mainWindow = MainWindow::createWindow(user);

                if(mainWindow)
                {
                    QObject::connect(mainWindow, &MainWindow::restartApplication,
                                     [&app, &restartNeeded]() {
                                         restartNeeded = true;
                                         app.quit();
                                     });

                    mainWindow->show();

                    app.exec();

                    delete mainWindow;
                }else {
                    QMessageBox::warning(nullptr, "Ошибка",
                                         "Не удалось создать главное окно");
                }
            }
        }

    } while (restartNeeded);

    return 0;
}
