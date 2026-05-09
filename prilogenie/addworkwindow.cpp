#include "addworkwindow.h"
#include "ui_addworkwindow.h"

#include <QMessageBox>
#include <QCompleter>

AddWorkWindow::AddWorkWindow(std::shared_ptr<app::App> app,
                             std::optional<std::vector<model::SeparateWork>>& pool_work,
                             QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::AddWorkWindow)
    , app_(app)
    , pool_work_(pool_work) {
    ui->setupUi(this);    
    setWindowTitle("Добавление работы");

    SetCompleter(ui->le_name, app_->GetBaseWork());
    SetCompleter(ui->le_responsible_employee_1, app_->GetBaseEmployee());
    SetCompleter(ui->le_responsible_employee_2, app_->GetBaseEmployee());
    SetCompleter(ui->le_responsible_employee_3, app_->GetBaseEmployee());
    SetCompleter(ui->le_responsible_employee_4, app_->GetBaseEmployee());
    SetCompleter(ui->le_responsible_employee_5, app_->GetBaseEmployee());
}

AddWorkWindow::~AddWorkWindow() {
    delete ui;
}

void AddWorkWindow::on_pb_add_work_clicked() {

    model::Date date_deadline = model::Date{
            .day_ = ui->sb_deadline_dd->value(),
            .month_ = ui->sb_deadline_mm->value(),
            .year_ = ui->sb_deadline_yyyy->value()};

    std::vector<std::string> employees;
    if (!(ui->le_responsible_employee_1->text().isEmpty())) {
        employees.push_back(ui->le_responsible_employee_1->text().toStdString());
    }
    if (!(ui->le_responsible_employee_2->text().isEmpty())) {
        employees.push_back(ui->le_responsible_employee_2->text().toStdString());
    }
    if (!(ui->le_responsible_employee_3->text().isEmpty())) {
        employees.push_back(ui->le_responsible_employee_3->text().toStdString());
    }
    if (!(ui->le_responsible_employee_4->text().isEmpty())) {
        employees.push_back(ui->le_responsible_employee_4->text().toStdString());
    }
    if (!(ui->le_responsible_employee_5->text().isEmpty())) {
        employees.push_back(ui->le_responsible_employee_5->text().toStdString());
    }

    model::SeparateWork new_work{
        .name_ = ui->le_name->text().toStdString(),
        .names_responsible_employees_ = std::move(employees),
        .date_deadline_ = date_deadline,
        .info_ = ui->le_info->text().toStdString()};

    // Добавляем в pool_work_
    if (!pool_work_.has_value()) {
        pool_work_ = std::vector<model::SeparateWork>();
    }
    pool_work_->push_back(new_work);

    emit UpdateTable();

    QMessageBox::information(this, "Добавление работы", "Работа добавлена!");
}

void AddWorkWindow::on_pb_add_employee_1_clicked() {
    ui->le_responsible_employee_2->setEnabled(true);
    ui->pb_add_employee_2->setEnabled(true);
}


void AddWorkWindow::on_pb_add_employee_2_clicked() {
    ui->le_responsible_employee_3->setEnabled(true);
    ui->pb_add_employee_3->setEnabled(true);
}


void AddWorkWindow::on_pb_add_employee_3_clicked() {
    ui->le_responsible_employee_4->setEnabled(true);
    ui->pb_add_employee_4->setEnabled(true);
}


void AddWorkWindow::on_pb_add_employee_4_clicked() {
    ui->le_responsible_employee_5->setEnabled(true);
    ui->pb_add_employee_5->setEnabled(true);
}

void AddWorkWindow::on_pb_add_employee_5_clicked() {
    QMessageBox::warning(this, "Добавление сотрудника", "Имей совесть! И так уже пол отдела поставил трудиться над этой работёнкой");
}

void AddWorkWindow::SetCompleter(QLineEdit *le, const std::set<std::string>& base) {
    // 1. Наша база слов для автодополнения le_name
    QStringList base_qsl;
    base_qsl.reserve(base.size());
    for (const auto& item : base) {
        base_qsl << QString::fromStdString(item);
    }
    // 2. Создаем QCompleter на основе нашего списка
    QCompleter *completer = new QCompleter(base_qsl, this);
    // Дополнительно: сделаем поиск нечувствительным к регистру (чтобы "Я" и "я" считались одинаковыми)
    completer->setCaseSensitivity(Qt::CaseInsensitive);
    // Искать вхождение, а не только с начала
    completer->setFilterMode(Qt::MatchContains);
    // 3. Устанавливаем в le_name completer
    le->setCompleter(completer);
}
