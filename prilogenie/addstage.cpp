#include "addstage.h"
#include "ui_addstage.h"

#include <handler_add_work.h>

#include <QMessageBox>

AddStage::AddStage(std::optional<std::vector<model::Stage>> &pool_stage, QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::AddStage)
    , pool_stage_(pool_stage)
    , pool_work_(std::nullopt) {
    ui->setupUi(this);
    setWindowTitle("Добавление этапа");

    if (!pool_stage_.has_value()) {
        ui->le_number->setText("1");
    }
    else {
        ui->le_number->setText(QString::number(pool_stage_.value().size() + 1));
    }
}

AddStage::~AddStage()
{
    delete ui;
}

void AddStage::on_pb_add_work_att_as_clicked() {
    model::SeparateWork razrab_PIM {"Разработка ПиМ", {"Пупкин С.С."}, {11, 11, 2026}, "По готовности объекта"};
    model::SeparateWork att_as {"Аттестация АС", {"Суходрищев В.В."}, {12, 12, 2026}, std::nullopt};
    model::SeparateWork razrab_doc {"Разработка документации после аттестационных испытаний с учетом погрешности, которая появляется в связи с долгой засухой", {"Суходрищев В.В.", "Пупкин С.С.", "Касторкин А.А."}, {1, 2, 2027}, "может быть выполним когда-нибудь"};

    details::AddSeparateWorkToTable(ui->table_work, razrab_PIM);
    details::AddSeparateWorkToTable(ui->table_work, att_as);
    details::AddSeparateWorkToTable(ui->table_work, razrab_doc);

    // Настраиваем свойства таблицы для многострочного отображения
    ui->table_work->resizeRowsToContents();
    ui->table_work->resizeColumnsToContents();

    // Включаем перенос текста для второго столбца
    ui->table_work->setWordWrap(true);

    // Устанавливаем политику размеров для строк
    ui->table_work->verticalHeader()->setSectionResizeMode(QHeaderView::ResizeToContents);

    if (!pool_work_.has_value()) {
        pool_work_ = std::vector<model::SeparateWork>{};
    }
    pool_work_.value().push_back(razrab_PIM);
    pool_work_.value().push_back(att_as);
    pool_work_.value().push_back(razrab_doc);
}

void AddStage::on_pb_add_stage_clicked() {
    std::optional<model::Date> date_deadline = std::nullopt;
    if (!(ui->cb_with_deadline_data->isChecked())) {
        date_deadline = model::Date{
            .day_ = ui->sb_deadline_dd->value(),
            .month_ = ui->sb_deadline_mm->value(),
            .year_ = ui->sb_deadline_yyyy->value()
        };
    }

    model::Stage new_stage {
        ui->le_number->text().toInt(),
        ui->le_name_full->text().toStdString(),
        date_deadline,
        ui->le_responsible_employee->text().toStdString(),
        {ui->sb_price_ruble->value(), ui->sb_price_kop->value()},
        {ui->sb_price_other_department_ruble->value(), ui->sb_price_other_department_kop->value()},
        pool_work_,
        ui->le_info->text().toStdString(),
        false,
        false,
        std::nullopt,
        std::nullopt,
        ui->le_status_payment->text().toStdString()
    };

    if (!pool_stage_.has_value()) {
        pool_stage_ = std::vector<model::Stage>{};
    }

    pool_stage_.value().push_back(std::move(new_stage));

    emit UpdateTable();

    QMessageBox::information(this, "Добавление этапа", "Этап добавлен!");
}

void AddStage::on_cb_with_deadline_data_stateChanged(int arg1) {
    ui->sb_deadline_dd->setEnabled(arg1);
    ui->sb_deadline_mm->setEnabled(arg1);
    ui->sb_deadline_yyyy->setEnabled(arg1);
}

void AddStage::on_cb_correct_number_stateChanged(int arg1)
{
    ui->le_number->setEnabled(arg1);
}

