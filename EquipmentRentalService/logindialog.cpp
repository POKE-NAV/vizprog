#include "logindialog.h"
#include "database.h"
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QFormLayout>
#include <QMessageBox>
#include <QLineEdit>
#include <QPushButton>
#include <QLabel>
#include <QStackedWidget>
#include <QRegularExpression>
#include <QRegularExpressionValidator>
#include <QDebug>

LoginDialog::LoginDialog(QWidget *parent) : QDialog(parent)
{
    setWindowTitle("Вход в систему аренды техники");
    setFixedSize(400, 385);

    if (!Database::isConnected()) {
        Database::connectToDatabase();
    }

    QVBoxLayout *mainLayout = new QVBoxLayout(this);
    mainLayout->setContentsMargins(20, 20, 20, 20);
    mainLayout->setSpacing(15);

    // Создаем QStackedWidget для переключения
    stackedWidget = new QStackedWidget();

    // ВИДЖЕТ ВХОДА
    loginWidget = new QWidget();
    QVBoxLayout *loginLayout = new QVBoxLayout(loginWidget);
    loginLayout->setContentsMargins(0, 0, 0, 0);
    loginLayout->setSpacing(15);

    // Заголовок
    QLabel *loginTitle = new QLabel("Вход в систему");
    loginTitle->setStyleSheet("font-size: 16px; font-weight: bold; color: #333;");
    loginTitle->setAlignment(Qt::AlignCenter);
    loginLayout->addWidget(loginTitle);

    // Форма входа
    QFormLayout *loginForm = new QFormLayout();
    loginForm->setSpacing(10);

    loginUsernameEdit = new QLineEdit();
    loginUsernameEdit->setPlaceholderText("Введите логин");
    loginUsernameEdit->setMinimumHeight(35);
    loginForm->addRow("Логин:", loginUsernameEdit);

    loginPasswordEdit = new QLineEdit();
    loginPasswordEdit->setEchoMode(QLineEdit::Password);
    loginPasswordEdit->setPlaceholderText("Введите пароль");
    loginPasswordEdit->setMinimumHeight(35);
    loginForm->addRow("Пароль:", loginPasswordEdit);

    loginLayout->addLayout(loginForm);
    loginLayout->addStretch();

    // Кнопки входа
    loginButton = new QPushButton("Войти");
    loginButton->setDefault(true);
    loginButton->setMinimumHeight(40);
    loginButton->setStyleSheet(
        "QPushButton {"
        "   background-color: #4CAF50;"
        "   color: white;"
        "   border: none;"
        "   border-radius: 5px;"
        "   font-size: 14px;"
        "   font-weight: bold;"
        "}"
        "QPushButton:hover {"
        "   background-color: #45a049;"
        "}"
        "QPushButton:pressed {"
        "   background-color: #3d8b40;"
        "}"
        );
    loginLayout->addWidget(loginButton);

    // Кнопка регистрации
    toRegisterButton = new QPushButton("Регистрация");
    toRegisterButton->setMinimumHeight(40);
    toRegisterButton->setStyleSheet(
        "QPushButton {"
        "   background-color: #2196F3;"
        "   color: white;"
        "   border: none;"
        "   border-radius: 5px;"
        "   font-size: 14px;"
        "   font-weight: bold;"
        "}"
        "QPushButton:hover {"
        "   background-color: #1976D2;"
        "}"
        "QPushButton:pressed {"
        "   background-color: #1565C0;"
        "}"
        );
    loginLayout->addWidget(toRegisterButton);

    // ВИДЖЕТ РЕГИСТРАЦИИ
    registerWidget = new QWidget();
    QVBoxLayout *registerLayout = new QVBoxLayout(registerWidget);
    registerLayout->setContentsMargins(0, 0, 0, 0);
    registerLayout->setSpacing(15);

    // Заголовок
    QLabel *registerTitle = new QLabel("Регистрация нового клиента");
    registerTitle->setStyleSheet("font-size: 16px; font-weight: bold; color: #333;");
    registerTitle->setAlignment(Qt::AlignCenter);
    registerLayout->addWidget(registerTitle);

    // Форма регистрации
    QFormLayout *registerForm = new QFormLayout();
    registerForm->setSpacing(10);

    // Логин
    regLoginEdit = new QLineEdit();
    regLoginEdit->setPlaceholderText("Придумайте логин");
    regLoginEdit->setMinimumHeight(35);
    registerForm->addRow("Логин*:", regLoginEdit);

    // ФИО
    regFioEdit = new QLineEdit();
    regFioEdit->setPlaceholderText("Иванов Иван Иванович");
    regFioEdit->setMinimumHeight(35);
    registerForm->addRow("ФИО*:", regFioEdit);

    // Номер телефона
    regPhoneEdit = new QLineEdit();
    regPhoneEdit->setPlaceholderText("+7 (999) 123-45-67");
    regPhoneEdit->setMinimumHeight(35);
    registerForm->addRow("Телефон:", regPhoneEdit);

    // Пароль
    regPasswordEdit = new QLineEdit();
    regPasswordEdit->setEchoMode(QLineEdit::Password);
    regPasswordEdit->setPlaceholderText("Минимум 6 символов");
    regPasswordEdit->setMinimumHeight(35);
    registerForm->addRow("Пароль*:", regPasswordEdit);

    registerLayout->addLayout(registerForm);

    // Инфо текст
    QLabel *infoLabel = new QLabel("* - обязательные поля");
    infoLabel->setStyleSheet("color: #666; font-size: 12px; font-style: italic;");
    infoLabel->setAlignment(Qt::AlignLeft);
    registerLayout->addWidget(infoLabel);
    registerLayout->addStretch();

    // Кнопка регистрации
    registerButton = new QPushButton("Зарегистрироваться");
    registerButton->setDefault(true);
    registerButton->setMinimumHeight(40);
    registerButton->setStyleSheet(
        "QPushButton {"
        "   background-color: #4CAF50;"
        "   color: white;"
        "   border: none;"
        "   border-radius: 5px;"
        "   font-size: 14px;"
        "   font-weight: bold;"
        "}"
        "QPushButton:hover {"
        "   background-color: #45a049;"
        "}"
        "QPushButton:pressed {"
        "   background-color: #3d8b40;"
        "}"
        );
    registerLayout->addWidget(registerButton);

    // Кнопка возврата - НИЖНЯЯ КНОПКА "Назад ко входу"
    toLoginButton = new QPushButton("Назад ко входу");
    toLoginButton->setMinimumHeight(40);
    toLoginButton->setStyleSheet(
        "QPushButton {"
        "   background-color: #2196F3;"
        "   color: white;"
        "   border: none;"
        "   border-radius: 5px;"
        "   font-size: 14px;"
        "   font-weight: bold;"
        "}"
        "QPushButton:hover {"
        "   background-color: #1976D2;"
        "}"
        "QPushButton:pressed {"
        "   background-color: #1565C0;"
        "}"
        );
    registerLayout->addWidget(toLoginButton);

    // Добавляем оба виджета в stackedWidget
    stackedWidget->addWidget(loginWidget);
    stackedWidget->addWidget(registerWidget);

    mainLayout->addWidget(stackedWidget);

    // Для формы входа
    connect(loginButton, &QPushButton::clicked, this, &LoginDialog::onLoginClicked);
    connect(toRegisterButton, &QPushButton::clicked, this, &LoginDialog::switchToRegistration);
    connect(loginPasswordEdit, &QLineEdit::returnPressed, this, &LoginDialog::onLoginClicked);

    // Для формы регистрации
    connect(registerButton, &QPushButton::clicked, this, &LoginDialog::onRegistrationClicked);
    connect(toLoginButton, &QPushButton::clicked, this, &LoginDialog::switchToLogin);
    connect(regPasswordEdit, &QLineEdit::returnPressed, this, &LoginDialog::onRegistrationClicked);

    loginUsernameEdit->setFocus();
}
LoginDialog::~LoginDialog()
{

}

// ПЕРЕКЛЮЧЕНИЕ ФОРМ
void LoginDialog::switchToRegistration()
{
    stackedWidget->setCurrentIndex(1);
    setWindowTitle("Регистрация - Аренда техники");
    regLoginEdit->setFocus();
}

void LoginDialog::switchToLogin()
{
    stackedWidget->setCurrentIndex(0);
    setWindowTitle("Вход - Аренда техники");
    loginUsernameEdit->setFocus();
}

// ВАЛИДАЦИЯ ФОРМЫ РЕГИСТРАЦИИ
bool LoginDialog::validateRegistrationForm()
{
    QString login = regLoginEdit->text().trimmed();
    QString fio = regFioEdit->text().trimmed();
    QString phone = regPhoneEdit->text().trimmed();
    QString password = regPasswordEdit->text();

    // Проверка обязательных полей
    if (login.isEmpty()) {
        QMessageBox::warning(this, "Ошибка", "Введите логин");
        regLoginEdit->setFocus();
        return false;
    }

    if (fio.isEmpty()) {
        QMessageBox::warning(this, "Ошибка", "Введите ФИО");
        regFioEdit->setFocus();
        return false;
    }

    if (password.isEmpty()) {
        QMessageBox::warning(this, "Ошибка", "Введите пароль");
        regPasswordEdit->setFocus();
        return false;
    }

    // Проверка логина =
    QRegularExpression loginRegex("^[a-zA-Z0-9_]+$");
    if (!loginRegex.match(login).hasMatch()) {
        QMessageBox::warning(this, "Ошибка",
                             "Логин может содержать только латинские буквы, цифры и символ подчеркивания");
        regLoginEdit->setFocus();
        return false;
    }

    // Длина логина
    if (login.length() < 3 || login.length() > 50) {
        QMessageBox::warning(this, "Ошибка",
                             "Логин должен быть от 3 до 50 символов");
        regLoginEdit->setFocus();
        return false;
    }

    // Длина пароля
    if (password.length() < 6) {
        QMessageBox::warning(this, "Ошибка",
                             "Пароль должен содержать минимум 6 символов");
        regPasswordEdit->clear();
        regPasswordEdit->setFocus();
        return false;
    }

    // Проверка телефона
    if (!phone.isEmpty()) {
        QRegularExpression phoneRegex("^\\+7\\s?\\(?\\d{3}\\)?\\s?\\d{3}[\\s-]?\\d{2}[\\s-]?\\d{2}$");
        if (!phoneRegex.match(phone).hasMatch()) {
            QMessageBox::warning(this, "Ошибка",
                                 "Введите корректный номер телефона в формате: +7 (999) 123-45-67");
            regPhoneEdit->setFocus();
            return false;
        }
    }

    return true;
}

void LoginDialog::onRegistrationClicked()
{
    if (!Database::connectToDatabase()) {
    }
    // Валидация формы
    if (!validateRegistrationForm()) {
        return;
    }

    QString login = regLoginEdit->text().trimmed();
    QString fio = regFioEdit->text().trimmed();
    QString phone = regPhoneEdit->text().trimmed();
    QString password = regPasswordEdit->text();

    // Проверяем, не занят ли логин
    if (Database::isLoginTaken(login)) {
        QMessageBox::warning(this, "Ошибка",
                             "Этот логин уже занят. Выберите другой.");
        regLoginEdit->selectAll();
        regLoginEdit->setFocus();
        return;
    }

    // Регистрация пользователя (роль "client" по умолчанию)
    bool success = Database::registerUser(login, password, fio, phone, "client");

    if (success) {
        QMessageBox::information(this, "Успешная регистрация",
                                 "Аккаунт успешно создан!\nТеперь вы можете войти в систему.");

        // Автозаполняем логин в форме входа
        loginUsernameEdit->setText(login);
        loginPasswordEdit->clear();
        loginPasswordEdit->setFocus();

        // Очищаем форму регистрации
        regLoginEdit->clear();
        regFioEdit->clear();
        regPhoneEdit->clear();
        regPasswordEdit->clear();

        // Переключаемся обратно на вход
        switchToLogin();
    } else {
        QMessageBox::warning(this, "Ошибка",
                             "Не удалось зарегистрироваться. Попробуйте позже.");
    }
}

// ОБРАБОТКА ВХОДА
void LoginDialog::onLoginClicked()
{
    QString username = loginUsernameEdit->text().trimmed();
    QString password = loginPasswordEdit->text().trimmed();

    // Проверка на пустые поля
    if (username.isEmpty()) {
        QMessageBox::warning(this, "Ошибка", "Введите логин");
        loginUsernameEdit->setFocus();
        return;
    }

    if (password.isEmpty()) {
        QMessageBox::warning(this, "Ошибка", "Введите пароль");
        loginPasswordEdit->setFocus();
        return;
    }

    // Используем User для аутентификации
    m_user = User::authenticate(username, password);

    if (m_user && m_user->isValid()) {
        accept();
    } else {
        QMessageBox::warning(this, "Ошибка авторизации",
                             "Неверный логин или пароль\n\n");

        loginPasswordEdit->clear();
        loginPasswordEdit->setFocus();

        delete m_user;
        m_user = nullptr;
    }
}
