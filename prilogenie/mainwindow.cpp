#include "mainwindow.h"
#include "ui_mainwindow.h"

#include "handler_odt.h"
#include "handler_pdf.h"
#include "handler_add_work.h"
#include "add_contract_plan_month_docx.h"

#include <QFileDialog>
#include <QMessageBox>

MainWindow::MainWindow(std::shared_ptr<app::App> app, QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
    , app_(app)
    , pool_work_(std::nullopt)
    , pool_stage_(std::nullopt) {
    ui->setupUi(this);
    setWindowTitle("Добавление договора");

    // Подключаем сигнал изменения ячейки таблицы
    connect(ui->table_work, &QTableWidget::cellChanged,
            this, &MainWindow::on_table_work_cellChanged);

    SetTableProperties(ui->table_work);
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::toUpdateTable()
{
    UpdateTable();
}


void MainWindow::on_pb_odt_clicked()
{
    CreateOdtWithTable();
}


void MainWindow::on_pb_pdf_clicked()
{
    CreatePdfReport();
}


void MainWindow::on_pb_add_work_att_as_clicked()
{
    model::SeparateWork razrab_PIM {"Разработка ПиМ", {"Пупкин С.С."}, {11, 11, 2026}, "По готовности объекта"};
    model::SeparateWork att_as {"Аттестация АС", {"Суходрищев В.В."}, {12, 12, 2026}, std::nullopt};
    model::SeparateWork razrab_doc {"Разработка документации после аттестационных испытаний с учетом погрешности, которая появляется в связи с долгой засухой", {"Суходрищев В.В.", "Пупкин С.С.", "Касторкин А.А."}, {1, 2, 2027}, "может быть выполним когда-нибудь"};

    if (!pool_work_.has_value()) {
        pool_work_ = std::vector<model::SeparateWork>{};
    }
    pool_work_.value().push_back(razrab_PIM);
    pool_work_.value().push_back(att_as);
    pool_work_.value().push_back(razrab_doc);

    UpdateTable();
}


void MainWindow::on_pb_add_contract_clicked() {
    model::TypeContract type = model::TypeContract::ATT;
    QString type_qstr = ui->cob_type_contract->currentText();
    if (type_qstr == "ГОЗ") {
        type = model::TypeContract::GOZ;
    }
    else if (type_qstr == "СИ") {
        type = model::TypeContract::SI;
    }
    else if (type_qstr == "Атт") {
        type = model::TypeContract::ATT;
    }
    else {
        type = model::TypeContract::BEK;
    }

    std::optional<model::Date> date_contract = std::nullopt;
    if (!(ui->cb_with_date->isChecked())) {
        date_contract = model::Date{
            .day_ = ui->sb_contract_dd->value(),
            .month_ = ui->sb_contract_mm->value(),
            .year_ = ui->sb_contract_yyyy->value()
        };
    }

    std::optional<model::Date> date_deadline = std::nullopt;
    if (!(ui->cb_with_date->isChecked())) {
        date_deadline = model::Date{
            .day_ = ui->sb_deadline_dd->value(),
            .month_ = ui->sb_deadline_mm->value(),
            .year_ = ui->sb_deadline_yyyy->value()
        };
    }

    model::Contract new_contract {
        ui->le_number->text().toStdString(),
        date_contract,
        ui->le_name_organization->text().toStdString(),
        ui->le_name_short->text().toStdString(),
        ui->le_name_full->text().toStdString(),
        date_deadline,
        ui->le_responsible_employee->text().toStdString(),
        {ui->sb_price_ruble->value(), ui->sb_price_kop->value()},
        {ui->sb_price_other_department_ruble->value(), ui->sb_price_other_department_kop->value()},
        ui->chb_nds->isChecked(),
        ui->sb_stavka_nds->value(),
        type,
        ui->chb_stage->isChecked(),
        pool_work_,
        ui->le_info->text().toStdString(),
        pool_stage_,
        false,
        false,
        std::nullopt,
        std::nullopt,
        ui->le_status_payment->text().toStdString()
    };

    if (app_->HasValuePathPlanMonth()) {
        if (updateContractDocument(QString::fromStdString(app_->GetPathPlanMonth()), new_contract)) {
            qDebug() << "Документ успешно обновлен!";
        }
        else {
            qDebug() << "Ошибка при обновлении документа";
        }
    }

    app_->AddContract(std::move(new_contract));

    QMessageBox::information(this, "Добавление договора", "Договор добавлен!");
}


void MainWindow::on_chb_stage_stateChanged(int arg1)
{
    ui->pb_add_stage->setEnabled(arg1);
    ui->sb_price_kop->setEnabled(!arg1);
    ui->sb_price_ruble->setEnabled(!arg1);
    ui->sb_price_other_department_kop->setEnabled(!arg1);
    ui->sb_price_other_department_ruble->setEnabled(!arg1);
}


void MainWindow::on_pb_add_stage_clicked()
{
    ui->chb_stage->setEnabled(false);

    emit AddStageInContract(pool_stage_);
}


void MainWindow::on_cb_with_date_stateChanged(int arg1)
{
    ui->sb_contract_dd->setEnabled(arg1);
    ui->sb_contract_mm->setEnabled(arg1);
    ui->sb_contract_yyyy->setEnabled(arg1);
}


void MainWindow::on_cb_with_deadline_data_stateChanged(int arg1)
{
    ui->sb_deadline_dd->setEnabled(arg1);
    ui->sb_deadline_mm->setEnabled(arg1);
    ui->sb_deadline_yyyy->setEnabled(arg1);
}


void MainWindow::SetTableProperties(QTableWidget* table) {
    // Настройка растягивания колонок
    table->horizontalHeader()->setSectionResizeMode(0, QHeaderView::Stretch);     // Растягивается
    table->horizontalHeader()->setSectionResizeMode(1, QHeaderView::ResizeToContents);
    table->horizontalHeader()->setSectionResizeMode(2, QHeaderView::ResizeToContents);
    table->horizontalHeader()->setSectionResizeMode(3, QHeaderView::ResizeToContents);
    table->horizontalHeader()->setSectionResizeMode(4, QHeaderView::Stretch);

    // Включаем перенос текста для всех ячеек
    table->setWordWrap(true);

    // Настройка автоматической высоты строк
    table->verticalHeader()->setSectionResizeMode(QHeaderView::ResizeToContents);
}

void MainWindow::UpdateTable()
{
    // Отключаем сигнал временно, чтобы избежать рекурсии
    ui->table_work->blockSignals(true);
    ui->table_work->setRowCount(0);

    if (pool_stage_.has_value()) {
        for (const auto& stage : pool_stage_.value()) {
            details::AddStageToTable(ui->table_work, stage);
        }
    }

    if (pool_work_.has_value()) {
        for (const auto& work : pool_work_.value()) {
            details::AddSeparateWorkToTable(ui->table_work, work);
        }
    }

    SetTableProperties(ui->table_work);
    // Включаем сигналы обратно, чтобы изменения в таблице сразу заносились в работы/этапы
    ui->table_work->blockSignals(false);
}

void MainWindow::on_pb_add_work_clicked()
{
    // Создаем новую работу со значениями по умолчанию
    model::SeparateWork newWork{
        .name_ = "Новая работа",
        .names_responsible_employees_ = {},
        .date_deadline_ = model::Date(),
    };

    // Добавляем в pool_work_
    if (!pool_work_.has_value()) {
        pool_work_ = std::vector<model::SeparateWork>();
    }
    pool_work_->push_back(newWork);

    // Добавляем в таблицу
    details::AddSeparateWorkToTable(ui->table_work, newWork);
    SetTableProperties(ui->table_work);
}

void MainWindow::on_table_work_cellChanged(int row, int column)
{
    // Отключаем сигнал временно, чтобы избежать рекурсии
    ui->table_work->blockSignals(true);

    // Проверяем, что pool_work_ инициализирован и row в пределах
    if (!pool_work_.has_value() || row >= static_cast<int>(pool_work_->size())) {
        ui->table_work->blockSignals(false);
        return;
    }

    // Получаем текущий элемент
    model::SeparateWork& work = (*pool_work_)[row];
    QTableWidgetItem* item = ui->table_work->item(row, column);

    if (!item) {
        ui->table_work->blockSignals(false);
        return;
    }

    // Обновляем соответствующее поле в зависимости от колонки
    switch (column) {
    case 0: // name_
        work.name_ = item->text().toStdString();
        break;

    case 2: { // names_responsible_employees_
        // Преобразуем текст обратно в вектор строк
        std::string text = item->text().toStdString();
        std::vector<std::string> employees;

        // Разделяем по запятой или новой строке
        std::stringstream ss(text);
        std::string employee;
        while (std::getline(ss, employee, ',')) {
            // Удаляем пробелы в начале и конце
            employee.erase(0, employee.find_first_not_of(" \t\n\r"));
            employee.erase(employee.find_last_not_of(" \t\n\r") + 1);
            if (!employee.empty()) {
                employees.push_back(employee);
            }
        }
        work.names_responsible_employees_ = employees;
        break;
    }

    case 3: { // date_deadline_
        // Парсим дату из формата dd.mm.yyyy
        QString dateStr = item->text();
        QStringList parts = dateStr.split('.');
        if (parts.size() == 3) {
            model::Date data;
            data.day_ = parts[0].toInt();
            data.month_ = parts[1].toInt();
            data.year_ = parts[2].toInt();
            work.date_deadline_ = data;
        }
        break;
    }

    case 4: // info_
        if (item->text().isEmpty()) {
            work.info_ = std::nullopt;
        } else {
            work.info_ = item->text().toStdString();
        }
        break;
    }

    // Включаем сигналы обратно
    ui->table_work->blockSignals(false);
}

