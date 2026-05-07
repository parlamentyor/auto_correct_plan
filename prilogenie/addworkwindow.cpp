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

    // 1. Наша база слов для автодополнения le_name
    QStringList base_work;
    base_work.reserve(app_->GetBaseWork().size());
    for (const auto& work : app_->GetBaseWork()) {
        base_work << QString::fromStdString(work);
    }

    // 2. Создаем QCompleter на основе нашего списка
    QCompleter *completer = new QCompleter(base_work, this);

    // Дополнительно: сделаем поиск нечувствительным к регистру (чтобы "Я" и "я" считались одинаковыми)
    completer->setCaseSensitivity(Qt::CaseInsensitive);

    // Искать вхождение, а не только с начала
    completer->setFilterMode(Qt::MatchContains);

    // 3. Устанавливаем в le_name completer
    ui->le_name->setCompleter(completer);
}

AddWorkWindow::~AddWorkWindow() {
    delete ui;
}

void AddWorkWindow::on_pb_add_work_clicked() {
    model::Date date_deadline = model::Date{
            .day_ = ui->sb_deadline_dd->value(),
            .month_ = ui->sb_deadline_mm->value(),
            .year_ = ui->sb_deadline_yyyy->value()};

    model::SeparateWork new_work{
        .name_ = ui->le_name->text().toStdString(),
        .names_responsible_employees_ = {ui->le_responsible_employee->text().toStdString()},
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

