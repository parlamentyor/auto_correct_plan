#include "addworkwindow.h"
#include "ui_addworkwindow.h"

#include <QMessageBox>
#include <QCompleter>
#include <QCalendarWidget>

AddWorkWindow::AddWorkWindow(std::shared_ptr<app::App> app,
                             std::optional<std::vector<model::SeparateWork>>& pool_work,
                             QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::AddWorkWindow)
    , app_(app)
    , pool_work_(pool_work)
    , date_({QDate::currentDate().day(),
             QDate::currentDate().month(),
             QDate::currentDate().year()})
    , index_(-1) {
    ui->setupUi(this);    
    setWindowTitle("Добавление работы");

    ui->de_deadline_data->setDate(QDate::currentDate());
    ui->de_deadline_data->setDisplayFormat("dd.MM.yyyy");

    ui->pb_correct->setVisible(false);

    SetCompleter(ui->le_name, app_->GetBaseWork());
    SetCompleter(ui->le_responsible_employee_1, app_->GetBaseEmployee());
    SetCompleter(ui->le_responsible_employee_2, app_->GetBaseEmployee());
    SetCompleter(ui->le_responsible_employee_3, app_->GetBaseEmployee());
    SetCompleter(ui->le_responsible_employee_4, app_->GetBaseEmployee());
    SetCompleter(ui->le_responsible_employee_5, app_->GetBaseEmployee());

    connect(ui->de_deadline_data, &QDateEdit::dateChanged,
            this, &AddWorkWindow::on_de_deadline_data_dateChanged);
}

AddWorkWindow::AddWorkWindow(std::shared_ptr<app::App> app,
                             std::optional<std::vector<model::SeparateWork> > &pool_work,
                             int pos,
                             QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::AddWorkWindow)
    , app_(app)
    , pool_work_(pool_work)
    , index_(pos) {
    ui->setupUi(this);
    setWindowTitle("Корректировка работы");

    model::SeparateWork work = pool_work_.value()[index_];

    if (work.date_deadline_.has_value()) {
        date_ = work.date_deadline_.value();
        QDate date = QDate(date_.value().year_, date_.value().month_, date_.value().day_);
        ui->de_deadline_data->setDate(date);

    }
    else {
        date_= std::nullopt;
        ui->de_deadline_data->setDate(QDate());
        ui->pb_edit_deadline_data->setEnabled(false);
        ui->de_deadline_data->setEnabled(false);
        ui->cb_with_deadline_data->setCheckState(Qt::CheckState::Checked);
    }

    ui->de_deadline_data->setDisplayFormat("dd.MM.yyyy");

    ui->le_name->setText(QString::fromStdString(work.name_));

    size_t count_employees = work.names_responsible_employees_.size();
    switch (count_employees) {
        case 0:
            break;
        case 1: {
            ui->le_responsible_employee_1->setText(QString::fromStdString(work.names_responsible_employees_[0]));
            break;
        }
        case 2: {
            ui->pb_add_employee_2->setEnabled(true);
            ui->le_responsible_employee_2->setEnabled(true);
            ui->le_responsible_employee_1->setText(QString::fromStdString(work.names_responsible_employees_[0]));
            ui->le_responsible_employee_2->setText(QString::fromStdString(work.names_responsible_employees_[1]));
            break;
        }
        case 3: {
            ui->pb_add_employee_2->setEnabled(true);
            ui->pb_add_employee_3->setEnabled(true);
            ui->le_responsible_employee_2->setEnabled(true);
            ui->le_responsible_employee_3->setEnabled(true);
            ui->le_responsible_employee_1->setText(QString::fromStdString(work.names_responsible_employees_[0]));
            ui->le_responsible_employee_2->setText(QString::fromStdString(work.names_responsible_employees_[1]));
            ui->le_responsible_employee_3->setText(QString::fromStdString(work.names_responsible_employees_[2]));
            break;
        }
        case 4: {
            ui->pb_add_employee_2->setEnabled(true);
            ui->pb_add_employee_3->setEnabled(true);
            ui->pb_add_employee_4->setEnabled(true);
            ui->le_responsible_employee_2->setEnabled(true);
            ui->le_responsible_employee_3->setEnabled(true);
            ui->le_responsible_employee_4->setEnabled(true);
            ui->le_responsible_employee_1->setText(QString::fromStdString(work.names_responsible_employees_[0]));
            ui->le_responsible_employee_2->setText(QString::fromStdString(work.names_responsible_employees_[1]));
            ui->le_responsible_employee_3->setText(QString::fromStdString(work.names_responsible_employees_[2]));
            ui->le_responsible_employee_3->setText(QString::fromStdString(work.names_responsible_employees_[3]));
            break;
        }
        case 5: {
            ui->pb_add_employee_2->setEnabled(true);
            ui->pb_add_employee_3->setEnabled(true);
            ui->pb_add_employee_4->setEnabled(true);
            ui->pb_add_employee_5->setEnabled(true);
            ui->le_responsible_employee_2->setEnabled(true);
            ui->le_responsible_employee_3->setEnabled(true);
            ui->le_responsible_employee_4->setEnabled(true);
            ui->le_responsible_employee_5->setEnabled(true);
            ui->le_responsible_employee_1->setText(QString::fromStdString(work.names_responsible_employees_[0]));
            ui->le_responsible_employee_2->setText(QString::fromStdString(work.names_responsible_employees_[1]));
            ui->le_responsible_employee_3->setText(QString::fromStdString(work.names_responsible_employees_[2]));
            ui->le_responsible_employee_3->setText(QString::fromStdString(work.names_responsible_employees_[3]));
            ui->le_responsible_employee_3->setText(QString::fromStdString(work.names_responsible_employees_[4]));
            break;
        }
    }

    if (work.info_.has_value()) {
        ui->le_info->setText(QString::fromStdString(work.info_.value()));
    }

    ui->pb_add_work->setVisible(false);

    SetCompleter(ui->le_name, app_->GetBaseWork());
    SetCompleter(ui->le_responsible_employee_1, app_->GetBaseEmployee());
    SetCompleter(ui->le_responsible_employee_2, app_->GetBaseEmployee());
    SetCompleter(ui->le_responsible_employee_3, app_->GetBaseEmployee());
    SetCompleter(ui->le_responsible_employee_4, app_->GetBaseEmployee());
    SetCompleter(ui->le_responsible_employee_5, app_->GetBaseEmployee());

    connect(ui->de_deadline_data, &QDateEdit::dateChanged,
            this, &AddWorkWindow::on_de_deadline_data_dateChanged);
}

AddWorkWindow::~AddWorkWindow() {
    delete ui;
}

void AddWorkWindow::on_pb_add_work_clicked() {
    if (ui->le_name->text().isEmpty()) {
        QMessageBox::warning(this, "Добавление работы", "Отсутстует наименование работы!");
        return;
    }

    if (ui->cb_with_deadline_data->isChecked() == true && ui->le_info->text().isEmpty()) {
        QMessageBox::warning(this, "Добавление работы", "Необходимо в поле Дополнительная информация указать причину отсутствия даты исполнения работы");
        return;
    }

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

    if (employees.empty()) {
        QMessageBox::warning(this, "Добавление работы", "Ниодного работника не добавлено!");
        return;
    }

    model::SeparateWork new_work{
        .name_ = ui->le_name->text().toStdString(),
        .names_responsible_employees_ = std::move(employees),
        .date_deadline_ = date_,
        .info_ = ui->le_info->text().toStdString(),
        .status_complet_ = {false, std::nullopt}};

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
    QMessageBox::warning(this, "Добавление сотрудника", "Имей совесть! И так уже пол отдела поставил трудиться над этой работёнкой!");
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

void AddWorkWindow::on_pb_edit_deadline_data_clicked() {
    // Создаём диалоговое окно
    QDialog *dialog = new QDialog(this);
    dialog->setWindowTitle("Выберите дату дедлайна");
    dialog->setModal(true);

    // Создаём календарь
    QCalendarWidget *calendar = new QCalendarWidget(dialog);

    // Устанавливаем текущую дату из data_ в календарь
    QDate currentDate;
    if (date_.has_value()) {
        currentDate = QDate(date_.value().year_, date_.value().month_, date_.value().day_);
        if (currentDate.isValid()) {
            calendar->setSelectedDate(currentDate);
        } else {
            calendar->setSelectedDate(QDate::currentDate());
        }
    } else {
        calendar->setSelectedDate(QDate::currentDate());
    }

    // Компоновка
    QVBoxLayout *layout = new QVBoxLayout(dialog);
    layout->addWidget(calendar);

    // Подключаем сигнал выбора даты
    connect(calendar, &QCalendarWidget::selectionChanged, [this, dialog, calendar]() {
        QDate selectedDate = calendar->selectedDate();

        // 1. Сохраняем дату в структуру data_
        date_.value().day_ = selectedDate.day();
        date_.value().month_ = selectedDate.month();
        date_.value().year_ = selectedDate.year();

        // 2. Отображаем дату в QDateEdit de_deadline_data
        ui->de_deadline_data->setDate(selectedDate);

        // 3. Закрываем диалог
        dialog->accept();
    });

    // Позиционируем окно в месте курсора мыши
    QPoint cursorPos = QCursor::pos();
    dialog->move(cursorPos);

    // Показываем диалог
    dialog->exec();

    // Удаляем диалог после закрытия
    dialog->deleteLater();
}

void AddWorkWindow::on_de_deadline_data_dateChanged(const QDate &date)
{
    if (date.isValid()) {
        date_.value().day_ = date.day();
        date_.value().month_ = date.month();
        date_.value().year_ = date.year();
    }
}

void AddWorkWindow::on_cb_with_deadline_data_stateChanged(int arg1)
{
    if (arg1 == Qt::Checked) {
        date_= std::nullopt;
        ui->de_deadline_data->setDate(QDate());
        ui->pb_edit_deadline_data->setEnabled(false);
        ui->de_deadline_data->setEnabled(false);
    }
    else {
        ui->pb_edit_deadline_data->setEnabled(true);
        ui->de_deadline_data->setEnabled(true);
        date_ = {QDate::currentDate().day(),
                 QDate::currentDate().month(),
                 QDate::currentDate().year()};
        ui->de_deadline_data->setDate(QDate::currentDate());
    }
}


void AddWorkWindow::on_pb_correct_clicked() {
    if (ui->le_name->text().isEmpty()) {
        QMessageBox::warning(this, "Добавление работы", "Отсутстует наименование работы!");
        return;
    }

    if (ui->cb_with_deadline_data->isChecked() == true && ui->le_info->text().isEmpty()) {
        QMessageBox::warning(this, "Добавление работы", "Необходимо в поле Дополнительная информация указать причину отсутствия даты исполнения работы");
        return;
    }

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

    if (employees.empty()) {
        QMessageBox::warning(this, "Добавление работы", "Ниодного работника не добавлено!");
        return;
    }

    pool_work_.value()[index_].name_ = ui->le_name->text().toStdString();
    pool_work_.value()[index_].names_responsible_employees_ = std::move(employees);
    pool_work_.value()[index_].date_deadline_ = date_;
    pool_work_.value()[index_].info_ = ui->le_info->text().toStdString();

    emit UpdateTable();

    QMessageBox::information(this, "Изменени работы", "Работа именена!");
}

