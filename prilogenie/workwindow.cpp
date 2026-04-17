#include "workwindow.h"
#include "ui_workwindow.h"

#include "handler_docx.h"
#include "magic_defs.h"

#include <QFileDialog>
#include <QMessageBox>

WorkWindow::WorkWindow(std::shared_ptr<app::App> app, QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::WorkWindow)
    , app_(app)
{
    ui->setupUi(this);
    ui->lb_authorization->setText(QString("Добро пожаловать, вы авторизовались как %1!").arg(QString::fromStdString(app->GetActivUserName())));
    setWindowTitle("Менеджер работ БИТ");
}

WorkWindow::~WorkWindow()
{
    delete ui;
}

void WorkWindow::on_pb_open_plan_month_clicked()
{
    QString fileName = QFileDialog::getOpenFileName(
        this,
        QString("Открыть файл с планом на месяц"),
        QDir::currentPath(), // QDir::currentPath — текущая папка.
        "*.doc;*.docx;*.odt"
        );

    ui->le_plan_month->setText(fileName);
    app_->SetPathPlanMonth(fileName.toStdString());

    // Дальше нужен актион

}


void WorkWindow::on_pb_create_plan_month_clicked()
{
    QString selected_filter;
    QString fileName = QFileDialog::getSaveFileName(this,
                                                    QString("Создать файл"),
                                                    QDir::currentPath(),
                                                    QString("%1;;%2;;%3").arg(TypeFile::ODT).arg(TypeFile::DOCX).arg(TypeFile::PDF),
                                                    &selected_filter);              // сюда запишется выбранный фильтр
    ui->le_plan_month->setText(fileName);
    app_->SetPathPlanMonth(fileName.toStdString());

    if (selected_filter == TypeFile::ODT) {

    }
    else if (selected_filter == TypeFile::DOCX) {

    }
    else if (selected_filter == TypeFile::PDF) {

    }
    else {
        QMessageBox::critical(this, "Ошибка", QString("Не могу создать файл формата %1").arg(selected_filter));
    }

}

void WorkWindow::on_pb_add_new_contract_clicked() {
    if (app_->HasValuePathPlanMonth()) {
        QMessageBox::StandardButton button = QMessageBox::question(
            this, "Добавление договора",
            "Не выбран файл с планом на месяц для добавления договора. Добавить договор только в базу?",
            QMessageBox::Ok | QMessageBox::Cancel
            );

        if (button == QMessageBox::Cancel) {
            return;
        }
    }
    emit AddContract();
}


void WorkWindow::on_pb_create_plan_month_default_clicked() {
    CreateDocxWithWord(app_->GetContracts());
    QMessageBox::information(this, "Успех", "План на месяц создан");
}

