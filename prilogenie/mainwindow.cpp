#include "mainwindow.h"
#include "ui_mainwindow.h"

#include "handler_odt.h"
#include "handler_pdf.h"
#include "handler_docx.h"

#include <QFileDialog>
#include <QMessageBox>

MainWindow::MainWindow(std::shared_ptr<app::App> app, QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
    , app_(app)
{
    ui->setupUi(this);
    ui->lb_authorization->setText(QString("Добро пожаловать, вы авторизовались как %1!").arg(QString::fromStdString(app->GetActivUserName())));
    setWindowTitle("Менеджер работ БИТ");
}

MainWindow::~MainWindow()
{
    delete ui;
}


void MainWindow::on_pb_odt_clicked()
{
    CreateOdtWithTable();
}


void MainWindow::on_pb_pdf_clicked()
{
    CreatePdfReport();
}


void MainWindow::on_pb_docx_clicked()
{
    CreateDocxWithWord();
}


void MainWindow::on_pb_open_plan_month_clicked()
{
    QString fileName = QFileDialog::getOpenFileName(
        this,
        QString("Открыть файл с планом на месяц"),
        QDir::currentPath(), // QDir::currentPath — текущая папка.
        "*.doc;*.docx;*.odt"
        );

    ui->le_plan_month->setText(fileName);

    // Дальше нужен актион

}


void MainWindow::on_pb_create_plan_month_clicked()
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
        CreateOdtWithTable(fileName);
    }
    else if (selected_filter == TypeFile::DOCX) {

    }
    else if (selected_filter == TypeFile::PDF) {

    }
    else {
        QMessageBox::critical(this, "Ошибка", QString("Не могу создать файл формата %1").arg(selected_filter));
    }


}

