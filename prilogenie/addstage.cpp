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

    // Подключаем сигнал изменения ячейки таблицы
    connect(ui->table_work, &QTableWidget::cellChanged,
            this, &AddStage::on_table_work_cellChanged);

    SetTableProperties(ui->table_work);

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

    SetTableProperties(ui->table_work);
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

void AddStage::on_pb_add_work_clicked()
{
    // Создаем новую работу со значениями по умолчанию
    model::SeparateWork newWork{
        .name_ = "Новая работа",
        .names_responsible_employees_ = {},
        .date_deadline_ = model::Date(), // или Date() для пустой даты
        .info_ = std::nullopt
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

void AddStage::on_table_work_cellChanged(int row, int column)
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

void AddStage::SetTableProperties(QTableWidget* table) {
    // Настройка растягивания колонок
    table->horizontalHeader()->setSectionResizeMode(0, QHeaderView::Stretch);
    table->horizontalHeader()->setSectionResizeMode(1, QHeaderView::ResizeToContents);
    table->horizontalHeader()->setSectionResizeMode(2, QHeaderView::ResizeToContents);
    table->horizontalHeader()->setSectionResizeMode(3, QHeaderView::ResizeToContents);
    table->horizontalHeader()->setSectionResizeMode(4, QHeaderView::Stretch);

    // Включаем перенос текста для всех ячеек
    table->setWordWrap(true);

    // Настройка автоматической высоты строк
    table->verticalHeader()->setSectionResizeMode(QHeaderView::ResizeToContents);
}
