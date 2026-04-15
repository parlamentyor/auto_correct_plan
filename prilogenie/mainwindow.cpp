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
{
    ui->setupUi(this);
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


void MainWindow::on_pb_add_work_att_as_clicked()
{
    model::SeparateWork razrab_PIM {"Разработка ПиМ", {"Пупкин С.С."}, {11, 11, 2026}, "По готовности объекта"};
    model::SeparateWork att_as {"Аттестация АС", {"Суходрищев В.В."}, {12, 12, 2026}, std::nullopt};
    model::SeparateWork razrab_doc {"Разработка документации после аттестационных испытаний с учетом погрешности, которая появляется в связи с долгой засухой", {"Суходрищев В.В.", "Пупкин С.С.", "Касторкин А.А."}, {1, 2, 2027}, "может быть выполним когда-нибудь"};

    addSeparateWorkToTable(ui->table_work, razrab_PIM);
    addSeparateWorkToTable(ui->table_work, att_as);
    addSeparateWorkToTable(ui->table_work, razrab_doc);

    // Настраиваем свойства таблицы для многострочного отображения
    ui->table_work->resizeRowsToContents();
    ui->table_work->resizeColumnsToContents();

    // Включаем перенос текста для второго столбца
    ui->table_work->setWordWrap(true);

    // Устанавливаем политику размеров для строк
    ui->table_work->verticalHeader()->setSectionResizeMode(QHeaderView::ResizeToContents);

    pool_work_.push_back(razrab_PIM);
    pool_work_.push_back(att_as);
    pool_work_.push_back(razrab_doc);
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

    model::Contract new_contract {
        ui->le_number->text().toStdString(),
        {ui->sb_contract_dd->value(), ui->sb_contract_mm->value(), ui->sb_contract_yyyy->value()},
        ui->le_name_organization->text().toStdString(),
        ui->le_name_short->text().toStdString(),
        ui->le_name_full->text().toStdString(),
        {ui->sb_deadline_dd->value(), ui->sb_deadline_mm->value(), ui->sb_deadline_yyyy->value()},
        ui->le_responsible_employee->text().toStdString(),
        {ui->sb_price_ruble->value(), ui->sb_price_kop->value()},
        {ui->sb_price_other_department_ruble->value(), ui->sb_price_other_department_kop->value()},
        ui->chb_nds->isChecked(),
        ui->sb_stavka_nds->value(),
        type,
        ui->chb_stage->isChecked(),
        pool_work_,
        ui->le_info->text().toStdString()
    };

    if (updateContractDocument(QString::fromStdString(app_->GetPathPlanMonth()), new_contract)) {
        qDebug() << "Документ успешно обновлен!";
    }
    else {
        qDebug() << "Ошибка при обновлении документа";
    }

    app_->AddContract(std::move(new_contract));
}

