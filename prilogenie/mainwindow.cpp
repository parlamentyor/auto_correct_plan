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
    // Настраиваем свойства таблицы для многострочного отображения
    table->resizeRowsToContents();
    table->resizeColumnsToContents();

    // Включаем перенос текста для второго столбца
    table->setWordWrap(true);

    // Устанавливаем политику размеров для строк
    table->verticalHeader()->setSectionResizeMode(QHeaderView::ResizeToContents);
}

void MainWindow::UpdateTable()
{
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

}
