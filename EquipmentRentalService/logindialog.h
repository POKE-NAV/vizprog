#ifndef LOGINDIALOG_H
#define LOGINDIALOG_H

#include <QDialog>
#include "user.h"
#include <QStackedWidget>

class QLineEdit;
class QPushButton;

class LoginDialog : public QDialog
{
    Q_OBJECT

public:
    explicit LoginDialog(QWidget *parent = nullptr);
    ~LoginDialog();

    User* getUser() const { return m_user; }

private slots:
    void onLoginClicked();          // Для кнопки "Войти"
    void onRegistrationClicked();   // Для кнопки "Зарегистрироваться"
    void switchToLogin();           // Переключить на форму входа
    void switchToRegistration();    // Переключить на форму регистрации

private:
    // QStackedWidget для переключения форм
    QStackedWidget *stackedWidget;

    // Виджеты для входа
    QWidget *loginWidget;
    QLineEdit *loginUsernameEdit;
    QLineEdit *loginPasswordEdit;
    QPushButton *loginButton;
    QPushButton *toRegisterButton;

    // Виджеты для регистрации
    QWidget *registerWidget;
    QLineEdit *regLoginEdit;
    QLineEdit *regPasswordEdit;
    QLineEdit *regFioEdit;
    QLineEdit *regPhoneEdit;
    QPushButton *registerButton;
    QPushButton *toLoginButton;

    // Общее
    QPushButton *cancelButton;
    User* m_user = nullptr;

    bool validateRegistrationForm();
};

#endif // LOGINDIALOG_H
