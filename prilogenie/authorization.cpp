#include "authorization.h"
#include "ui_authorization.h"

#include <QMessageBox>

Authorization::Authorization(std::shared_ptr<app::App> app, QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::Authorization)
    , app_(app)
{
    ui->setupUi(this);
}

Authorization::~Authorization()
{
    delete ui;
}

void Authorization::on_pb_authorization_clicked()
{
    if (ui->le_login->text().isEmpty() || ui->le_pass->text().isEmpty()) {
        QMessageBox::critical(this, "Ошибка", "Логин или пароль не введен");
    }
    else if (!(app_->AvailableUser(ui->le_login->text().toStdString()))) {
        QMessageBox::critical(this, "Ошибка", "Пользователь с таким логином отсутствует");
    }
    else if (!(app_->IsCorrectPass(ui->le_login->text().toStdString(), ui->le_pass->text().toStdString()))) {
        QMessageBox::critical(this, "Ошибка", "Некорректный пароль");
    }
    else {
        app_->SetActivUserId(ui->le_login->text().toStdString());
        QMessageBox::information(this, "Успех", "Авторизация прошла успешно"); // добавить в вывод имя пользователя
        close();
        //нужно допилить - закрыть окно с кодом 0
    }
}


void Authorization::on_pb_create_user_clicked()
{
    if (ui->le_login->text().isEmpty() || ui->le_pass->text().isEmpty()) {
        QMessageBox::critical(this, "Ошибка", "Логин или пароль не введен");
    }
    else if (app_->AvailableUser(ui->le_login->text().toStdString())) {
        QMessageBox::critical(this, "Ошибка", "Пользователь с таким логином уже существует");
    }
    else {
        app_->AddUser(ui->le_login->text().toStdString(), ui->le_pass->text().toStdString());
        QMessageBox::information(this, "Успех", "Пользователь добавлен"); // добавить в вывод имя пользователя
    }
}

