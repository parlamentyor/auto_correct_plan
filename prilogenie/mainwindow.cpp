#include "mainwindow.h"
#include "ui_mainwindow.h"

#include "handler_odt.h"
#include "handler_pdf.h"
#include "handler_add_work.h"
#include "add_contract_plan_month_docx.h"

#include <QFileDialog>
#include <QMessageBox>
#include <QCalendarWidget>
#include <QCompleter>
#include <QInputDialog>

MainWindow::MainWindow(std::shared_ptr<app::App> app, QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
    , app_(app)
    , contract_(std::make_shared<model::Contract>())
    , contract_date_({QDate::currentDate().day(),
                      QDate::currentDate().month(),
                      QDate::currentDate().year()})
    , deadline_date_({QDate::currentDate().day(),
                      QDate::currentDate().month(),
                      QDate::currentDate().year()}) {
    ui->setupUi(this);
    setWindowTitle("Добавление договора");

    SetCompleter(ui->le_responsible_employee, app_->GetBaseEmployee());
    SetCompleter(ui->le_name_organization, app_->GetBaseOrganizations());

    ui->de_contract_data->setDate(QDate::currentDate());
    ui->de_contract_data->setDisplayFormat("dd.MM.yyyy");

    ui->de_deadline_data->setDate(QDate::currentDate());
    ui->de_deadline_data->setDisplayFormat("dd.MM.yyyy");

    // Подключаем сигнал изменения ячейки таблицы
    connect(ui->table_work, &QTableWidget::cellChanged,
            this, &MainWindow::on_table_work_cellChanged);

    SetTableProperties(ui->table_work);

    // Для контекстного меню
    // 1. Включаем политику, разрешающую генерацию сигнала при запросе контекстного меню
    ui->table_work->setContextMenuPolicy(Qt::CustomContextMenu);

    // 2. Подключаем сигнал к нашему слоту
    connect(ui->table_work, &QTableWidget::customContextMenuRequested,
            this, &MainWindow::ShowContextMenu);

    BuildVirtualRows();
}

MainWindow::~MainWindow() {
    delete ui;
}

void MainWindow::toUpdateTable() {
    UpdateTable();
}

void MainWindow::toAddNewWork(std::shared_ptr<model::SeparateWork> new_work) {
    if (!contract_->pool_work.has_value()) {
        contract_->pool_work = std::vector<std::shared_ptr<model::SeparateWork>>();
    }
    contract_->pool_work->push_back(new_work);
}

void MainWindow::toAddNewStage(std::shared_ptr<model::Stage> new_stage) {
    if (!contract_->pool_stage_.has_value()) {
        contract_->pool_stage_ = std::vector<std::shared_ptr<model::Stage>>();
    }
    contract_->pool_stage_->push_back(new_stage);

    UpdateTable();
}


void MainWindow::on_pb_odt_clicked() {
    CreateOdtWithTable();
}


void MainWindow::on_pb_pdf_clicked() {
    CreatePdfReport();
}


void MainWindow::on_pb_add_work_att_as_clicked()
{
    model::Date date_razrab_PIM = {11, 11, 2026};
    model::Date date_att_as = {12, 12, 2026};
    model::Date date_razrab_doc = {1, 2, 2027};
    std::shared_ptr<model::SeparateWork> razrab_PIM = std::make_shared<model::SeparateWork>(model::SeparateWork{"Разработка ПиМ",
                                                                                                                {"Пупкин С.С."},
                                                                                                                date_razrab_PIM,
                                                                                                                "По готовности объекта",
                                                                                                                {false, std::nullopt},
                                                                                                                {false, std::nullopt, std::nullopt}});
    std::shared_ptr<model::SeparateWork> att_as = std::make_shared<model::SeparateWork>(model::SeparateWork{"Аттестация АС",
                                                                                                            {"Суходрищев В.В."},
                                                                                                            date_att_as,
                                                                                                            "",
                                                                                                            {false, std::nullopt},
                                                                                                            {false, std::nullopt, std::nullopt}});
    std::shared_ptr<model::SeparateWork> razrab_doc = std::make_shared<model::SeparateWork>(model::SeparateWork{"Разработка документации после аттестационных испытаний с учетом погрешности, которая появляется в связи с долгой засухой",
                                                                                                                {"Суходрищев В.В.", "Пупкин С.С.", "Касторкин А.А."},
                                                                                                                date_razrab_doc,
                                                                                                                "может быть выполним когда-нибудь",
                                                                                                                {false, std::nullopt},
                                                                                                                {false, std::nullopt, std::nullopt}});

    if (!(contract_->pool_work.has_value())) {
        contract_->pool_work = std::vector<std::shared_ptr<model::SeparateWork>>{};
    }
    contract_->pool_work.value().push_back(razrab_PIM);
    contract_->pool_work.value().push_back(att_as);
    contract_->pool_work.value().push_back(razrab_doc);

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

    model::Price price = MakePriceContract();
    model::Price price_other_department = MakePriceOtherDepartmentContract();

    contract_->number_ = ui->le_number->text().toStdString();
    contract_->date_ = contract_date_;
    contract_->name_organization_ = ui->le_name_organization->text().toStdString();
    contract_->name_short_ = ui->le_name_short->text().toStdString();
    contract_->name_full_ = ui->le_name_full->text().toStdString();
    contract_->date_deadline_ = deadline_date_;
    contract_->name_responsible_employee_ = ui->le_responsible_employee->text().toStdString();
    contract_->price_ = price;
    contract_->price_other_department_ = price_other_department;
    contract_->with_nds_ = ui->chb_nds->isChecked();
    contract_->stavka_nds_ = ui->sb_stavka_nds->value();
    contract_->type_ = type;
    contract_->with_stage_ = ui->chb_stage->isChecked();
    contract_->info_ = ui->le_info->text().toStdString();
    contract_->status_complet_ = {false, std::nullopt};
    contract_->status_actual_ = {false, std::nullopt, std::nullopt};
    contract_->is_paid_ = false;
    contract_->status_payment_ = ui->le_status_payment->text().toStdString();

    if (app_->HasValuePathPlanMonth()) {
        if (updateContractDocument(QString::fromStdString(app_->GetPathPlanMonth()), contract_)) {
            qDebug() << "Документ успешно обновлен!";
        }
        else {
            qDebug() << "Ошибка при обновлении документа";
        }
    }

    AddWorkInBase();
    AddExpenseInBase();
    AddOrganizationInBase();

    app_->AddContract(contract_);

    QMessageBox::information(this, "Добавление договора", "Договор добавлен!");
}


void MainWindow::on_chb_stage_stateChanged(int arg1)
{
    ui->pb_add_stage->setEnabled(arg1);
    ui->sb_price_kop->setEnabled(!arg1);
    ui->sb_price_ruble->setEnabled(!arg1);
    ui->sb_price_other_department_kop->setEnabled(!arg1);
    ui->sb_price_other_department_ruble->setEnabled(!arg1);

// Возможно лишняя проверка так как возможность отключить этапность пропадает после добавления этапа
// Возможно понадобится, когда реализую удаление этапов
/*
    if (!arg1 && pool_stage_.has_value()) {
        pool_stage_ = std::nullopt;
        UpdateTable();
    }
*/
}


void MainWindow::on_pb_add_stage_clicked()
{
    ui->chb_stage->setEnabled(false);

    emit AddStageInContract();

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

void MainWindow::UpdateTable() {
    BuildVirtualRows();
    // Отключаем сигнал временно, чтобы избежать рекурсии
    ui->table_work->blockSignals(true);
    ui->table_work->setRowCount(0);

    if (contract_->pool_stage_.has_value()) {
        for (const auto& stage : contract_->pool_stage_.value()) {
            details::AddStageToTable(ui->table_work, *stage);
            int row_stage = ui->table_work->rowCount() - 1;
            if (stage->status_complet_.is_complet_) {
                SetRowBackgroundColor(row_stage, QColor(144, 238, 144));  //Светло-зеленый
            }
            if (stage->status_actual_.is_no_actual_) {
                SetRowBackgroundColor(row_stage, Qt::lightGray);  //Светло-серый
            }
            if (stage->pool_work_.has_value()) {
                for (const auto& work : stage->pool_work_.value()) {
                    details::AddSeparateWorkToTable(ui->table_work, *work);
                    int row = ui->table_work->rowCount() - 1;
                    if (work->status_complet_.is_complet_) {
                        SetRowBackgroundColor(row, QColor(144, 238, 144));  //Светло-зеленый
                    }
                    if (work->status_actual_.is_no_actual_) {
                        SetRowBackgroundColor(row, Qt::lightGray);  //Светло-серый
                    }
                }
            }
        }
    }

    if (contract_->pool_work.has_value()) {
        details::AddHeaderToTable(ui->table_work, "Работы вне этапа");
        for (const auto& work : contract_->pool_work.value()) {
            details::AddSeparateWorkToTable(ui->table_work, *work);
            int row = ui->table_work->rowCount() - 1;
            if (work->status_complet_.is_complet_) {
                SetRowBackgroundColor(row, QColor(144, 238, 144));  //Светло-зеленый
            }
            if (work->status_actual_.is_no_actual_) {
                SetRowBackgroundColor(row, Qt::lightGray);  //Светло-серый
            }
        }
    }

    SetTableProperties(ui->table_work);
    // Включаем сигналы обратно, чтобы изменения в таблице сразу заносились в работы/этапы
    ui->table_work->blockSignals(false);

    UpdatePrice();
    UpdatePriceOtherDepartment();
}

void MainWindow::on_pb_add_work_clicked() {
    emit AddWorkInContract(app_);
}

void MainWindow::on_table_work_cellChanged(int row, int column) {
    // Отключаем сигнал временно, чтобы избежать рекурсии
    ui->table_work->blockSignals(true);

    // Проверяем, что pool_work_ инициализирован и row в пределах
    if (!contract_->pool_work.has_value() || row >= static_cast<int>(contract_->pool_work->size())) {
        ui->table_work->blockSignals(false);
        return;
    }

    // Получаем текущий элемент
    model::SeparateWork& work = *(contract_->pool_work.value())[row];
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

void MainWindow::on_pb_edit_deadline_data_clicked() {
    UpdateDate(deadline_date_, ui->de_deadline_data);
}

void MainWindow::on_de_deadline_data_dateChanged(const QDate &date)
{
    if (date.isValid()) {
        deadline_date_.value().day_ = date.day();
        deadline_date_.value().month_ = date.month();
        deadline_date_.value().year_ = date.year();
    }
}

void MainWindow::on_cb_with_deadline_data_stateChanged(int arg1)
{
    if (arg1 == Qt::Checked) {
        deadline_date_= std::nullopt;
        ui->de_deadline_data->setDate(QDate());
        ui->pb_edit_deadline_data->setEnabled(false);
        ui->de_deadline_data->setEnabled(false);
    }
    else {
        ui->pb_edit_deadline_data->setEnabled(true);
        ui->de_deadline_data->setEnabled(true);
        deadline_date_ = {QDate::currentDate().day(),
                 QDate::currentDate().month(),
                 QDate::currentDate().year()};
        ui->de_deadline_data->setDate(QDate::currentDate());
    }
}

void MainWindow::on_pb_edit_contract_data_clicked() {
    UpdateDate(contract_date_, ui->de_contract_data);
}

void MainWindow::on_de_contract_data_dateChanged(const QDate &date)
{
    if (date.isValid()) {
        contract_date_.value().day_ = date.day();
        contract_date_.value().month_ = date.month();
        contract_date_.value().year_ = date.year();
    }
}

void MainWindow::on_cb_with_date_stateChanged(int arg1)
{
    if (arg1 == Qt::Checked) {
        contract_date_= std::nullopt;
        ui->de_contract_data->setDate(QDate());
        ui->pb_edit_contract_data->setEnabled(false);
        ui->de_contract_data->setEnabled(false);
    }
    else {
        ui->pb_edit_contract_data->setEnabled(true);
        ui->de_contract_data->setEnabled(true);
        contract_date_ = {QDate::currentDate().day(),
                          QDate::currentDate().month(),
                          QDate::currentDate().year()};
        ui->de_contract_data->setDate(QDate::currentDate());
    }
}

void MainWindow::UpdateDate(std::optional<model::Date>& date, QDateEdit *de) {
    // Создаём диалоговое окно
    QDialog *dialog = new QDialog(this);
    dialog->setWindowTitle("Выберите дату");
    dialog->setModal(true);

    // Создаём календарь
    QCalendarWidget *calendar = new QCalendarWidget(dialog);

    // Устанавливаем текущую дату из data_ в календарь
    QDate currentDate;
    if (date.has_value()) {
        currentDate = QDate(date.value().year_, date.value().month_, date.value().day_);
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
    connect(calendar, &QCalendarWidget::selectionChanged, [this, dialog, calendar, &date, &de]() {
        QDate selectedDate = calendar->selectedDate();

        // 1. Сохраняем дату в структуру data
        date.value().day_ = selectedDate.day();
        date.value().month_ = selectedDate.month();
        date.value().year_ = selectedDate.year();

        // 2. Отображаем дату в QDateEdit de_deadline_data
        de->setDate(selectedDate);

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

model::Price MainWindow::MakePriceContract() const {
    model::Price price {0,0};
    if (contract_->pool_stage_.has_value()) {
        for (const auto& stage : contract_->pool_stage_.value()) {
            price += stage->price_;
        }
    }
    else {
        price.kop_ = ui->sb_price_kop->value();
        price.ruble_ = ui->sb_price_ruble->value();
    }
    return price;
}

model::Price MainWindow::MakePriceOtherDepartmentContract() const {
    model::Price price {0,0};
    if (contract_->pool_stage_.has_value()) {
        for (const auto& stage : contract_->pool_stage_.value()) {
            price += stage->price_other_department_;
        }
    }
    else {
        price.kop_ = ui->sb_price_other_department_kop->value();
        price.ruble_ = ui->sb_price_other_department_ruble->value();
    }
    return price;
}

void MainWindow::UpdatePrice() {
    model::Price price = MakePriceContract();
    ui->sb_price_kop->setValue(price.kop_);
    ui->sb_price_ruble->setValue(price.ruble_);
}

void MainWindow::UpdatePriceOtherDepartment() {
    model::Price price = MakePriceOtherDepartmentContract();
    ui->sb_price_other_department_kop->setValue(price.kop_);
    ui->sb_price_other_department_ruble->setValue(price.ruble_);
}

void MainWindow::AddWorkInBase() {
    if (contract_->pool_stage_.has_value()) {
        for (const auto& stage : contract_->pool_stage_.value()) {
            if (stage->pool_work_.has_value()) {
                for (const auto& work : stage->pool_work_.value()) {
                    app_->AddBaseWork(work->name_);
                }
            }
        }
    }

    if (contract_->pool_work.has_value()) {
        for (const auto& work : contract_->pool_work.value()) {
            app_->AddBaseWork(work->name_);
        }
    }
}

void MainWindow::AddExpenseInBase() {
    if (contract_->pool_stage_.has_value()) {
        for (const auto& stage : contract_->pool_stage_.value()) {
            if (stage->expenses_.has_value()) {
                for (const auto& expense : stage->expenses_.value()) {
                    app_->AddBaseExpenses(expense.name_);
                }
            }
        }
    }

    if (contract_->expenses_.has_value()) {
        for (const auto& expense : contract_->expenses_.value()) {
            app_->AddBaseExpenses(expense.name_);
        }
    }
}

void MainWindow::AddOrganizationInBase() {
    if (!(ui->le_name_organization->text().isEmpty())) {
        app_->AddBaseOrganizations(ui->le_name_organization->text().toStdString());
    }
}

void MainWindow::SetCompleter(QLineEdit *le, const std::set<std::string> &base) {
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

void MainWindow::on_pb_expenses_clicked() {
    if (ui->chb_stage->isChecked()) {
        QMessageBox::warning(this, "Ошибка", "В договоре есть этапы!\nДобавляйте затраты в соответствующих этапах выполнения работы.");
    }
    else {
        emit AddExpensesInContract(app_, contract_->expenses_);
    }
}


void MainWindow::on_pb_payments_clicked() {
    if (ui->chb_stage->isChecked()) {
        QMessageBox::warning(this, "Ошибка", "В договоре есть этапы!\nДобавляйте оплаты в соответствующих этапах выполнения работы.");
    }
    else {
        emit EditPaymentsInContract(contract_->payments_);
    }
}


void MainWindow::on_chb_nds_stateChanged(int arg1) {
    if (arg1) {
        ui->sb_stavka_nds->setEnabled(true);
        ui->sb_stavka_nds->setValue(22);
    }
    else {
        ui->sb_stavka_nds->setEnabled(false);
        ui->sb_stavka_nds->setValue(0);
    }
}




MainWindow::ItemInfo MainWindow::GetItemInfo(int virtual_row) const {
    if (virtual_row >= 0 && virtual_row < static_cast<int>(virtual_rows_.size())) {
        return virtual_rows_[virtual_row].info_;
    }
    return {ItemInfo::Type::HeaderRow, -1, -1, -1}; //Возможно и не нужно это
}

void MainWindow::BuildVirtualRows() const {
    virtual_rows_.clear();
    int global_row = 0;
//    int subCounter = 1; // для нумерации работ без этапов

    // Отображаем этапы
    if (contract_->pool_stage_.has_value()) {
        const auto& stages = contract_->pool_stage_.value();
        for (size_t stage_idx = 0; stage_idx < stages.size(); ++stage_idx) {
            // Строка этапа
            virtual_rows_.push_back({{ItemInfo::Type::StageRow,
                                     static_cast<int>(stage_idx), -1, global_row++}});

            // Отображаем работы этапа
            if (stages[stage_idx]->pool_work_.has_value()) {
                const auto& works = stages[stage_idx]->pool_work_.value();
                for (size_t work_idx = 0; work_idx < works.size(); ++work_idx) {
                    virtual_rows_.push_back({{ItemInfo::Type::WorkRow,
                                              static_cast<int>(stage_idx),
                                              static_cast<int>(work_idx),
                                              global_row++}});
                }
            }
//            subCounter = stages.size() + 1;
        }
    }

    // Отображаем работы контракта (без этапов)
    if (contract_->pool_work.has_value()) {
        virtual_rows_.push_back({{ItemInfo::Type::HeaderRow, -1, -1, global_row++}});
        const auto& works = contract_->pool_work.value();
        for (size_t work_idx = 0; work_idx < works.size(); ++work_idx) {
            virtual_rows_.push_back({{ItemInfo::Type::WorkRow,
                                      -1,
                                      static_cast<int>(work_idx),
                                      global_row++}});
        }
    }
}

void MainWindow::ShowContextMenu(const QPoint &pos) {
    // pos - это координаты клика относительно виджета tableWidget.
    // Чтобы узнать, по какой именно ячейке кликнули, используем itemAt().
    QTableWidgetItem* item = ui->table_work->itemAt(pos);

    if (!item) {
        // Кликнули не по ячейке, а по пустой области таблицы
        return;
    }

    // Сохраняем строку, чтобы потом производить действия над ней
    int index = item->row();

    auto item_info = GetItemInfo(index);

    if (item_info.type_ == ItemInfo::Type::WorkRow && item_info.stage_index_ == -1) {
        ShowContextMenuWork(contract_->pool_work, pos, item_info.work_index_);
    }
    else if (item_info.type_ == ItemInfo::Type::WorkRow && item_info.stage_index_ != -1) {
        ShowContextMenuWork(contract_->pool_stage_.value()[item_info.stage_index_]->pool_work_, pos, item_info.work_index_);
    }
    else if (item_info.type_ == ItemInfo::Type::StageRow) {
        ShowContextMenuStage(contract_->pool_stage_, pos, item_info.stage_index_);
    }

    UpdateTable();
}

void MainWindow::ShowContextMenuWork(std::optional<std::vector<std::shared_ptr<model::SeparateWork>>>& pool_work, const QPoint &pos, int work_index) {
    QMenu menu;
    QAction* action_edit_work = menu.addAction("Редактировать работу");
    QAction* action_delete_work = menu.addAction("Удалить работу");
    QAction* action_complet_work = menu.addAction("Изменить статус выполнения");
    QAction* action_actual_work = menu.addAction("Изменить статус актуальности");

    if (pool_work.value()[work_index]->status_complet_.is_complet_) {
        action_edit_work->setEnabled(false);
        action_delete_work->setEnabled(false);
        action_actual_work->setEnabled(false);
    }

    if (pool_work.value()[work_index]->status_actual_.is_no_actual_) {
        action_edit_work->setEnabled(false);
        action_delete_work->setEnabled(false);
        action_complet_work->setEnabled(false);
    }

    // Показываем меню в точке клика. mapToGlobal преобразует локальные координаты в глобальные.
    QAction* selected_action = menu.exec(ui->table_work->mapToGlobal(pos));

    if (selected_action == action_edit_work) {
        emit EditWork(pool_work.value()[work_index]);
    }
    else if (selected_action == action_delete_work) {
        on_ActionDeleteWork(pool_work, work_index);
    }
    else if (selected_action == action_complet_work) {
        on_ActionCompletWork(pool_work, work_index);
    }
    else if (selected_action == action_actual_work) {
        on_ActionActualWork(pool_work, work_index);
    }
}

void MainWindow::ShowContextMenuStage(std::optional<std::vector<std::shared_ptr<model::Stage> > > &pool_stage,
                                      const QPoint &pos,
                                      int stage_index) {
    QMenu menu;
    QAction* action_edit_stage = menu.addAction("Редактировать этап");
    QAction* action_delete_stage = menu.addAction("Удалить этап");
    QAction* action_complet_stage = menu.addAction("Изменить статус выполнения");
    QAction* action_actual_stage = menu.addAction("Изменить статус актуальности");

    if (pool_stage.value()[stage_index]->status_complet_.is_complet_) {
        action_edit_stage->setEnabled(false);
        action_delete_stage->setEnabled(false);
        action_actual_stage->setEnabled(false);
    }

    if (pool_stage.value()[stage_index]->status_actual_.is_no_actual_) {
        action_edit_stage->setEnabled(false);
        action_delete_stage->setEnabled(false);
        action_complet_stage->setEnabled(false);
    }

    // Показываем меню в точке клика. mapToGlobal преобразует локальные координаты в глобальные.
    QAction* selected_action = menu.exec(ui->table_work->mapToGlobal(pos));

    if (selected_action == action_edit_stage) {
        emit EditStage(pool_stage.value()[stage_index]);
    }
    else if (selected_action == action_delete_stage) {
        on_ActionDeleteStage(pool_stage, stage_index);
    }
    else if (selected_action == action_complet_stage) {
        on_ActionCompletStage(pool_stage, stage_index);
    }
    else if (selected_action == action_actual_stage) {
        on_ActionActualStage(pool_stage, stage_index);
    }
}

void MainWindow::on_ActionDeleteWork(std::optional<std::vector<std::shared_ptr<model::SeparateWork>>>& pool_work, int index) {
    pool_work.value().erase(pool_work.value().begin() + index);
    if (pool_work.value().empty()) {
        pool_work = std::nullopt;
    }
}

void MainWindow::on_ActionCompletWork(const std::optional<std::vector<std::shared_ptr<model::SeparateWork>>>& pool_work, int index) {
    if (!(pool_work.value()[index]->status_complet_.is_complet_)) {
        model::Date date = {QDate::currentDate().day(),
                            QDate::currentDate().month(),
                            QDate::currentDate().year()};
        pool_work.value()[index]->status_complet_ = {true, date};
        QString qstr = QString("\n\nВыполнена %1.%2.%3")
                           .arg(QDate::currentDate().day(), 2, 10, QChar('0'))
                           .arg(QDate::currentDate().month(), 2, 10, QChar('0'))
                           .arg(QDate::currentDate().year(), 4, 10, QChar('0'));
        std::string str_info = qstr.toStdString();
        pool_work.value()[index]->info_ = pool_work.value()[index]->info_.value() + str_info;
    }
    else {
        pool_work.value()[index]->status_complet_ = {false, std::nullopt};
        QString qstr = QString("\n\nВернули в работу %1.%2.%3")
                           .arg(QDate::currentDate().day(), 2, 10, QChar('0'))
                           .arg(QDate::currentDate().month(), 2, 10, QChar('0'))
                           .arg(QDate::currentDate().year(), 4, 10, QChar('0'));
        std::string str_info = qstr.toStdString();
        pool_work.value()[index]->info_ = pool_work.value()[index]->info_.value() + str_info;
    }
}

void MainWindow::on_ActionActualWork(const std::optional<std::vector<std::shared_ptr<model::SeparateWork>>>& pool_work, int index) {
    if (!(pool_work.value()[index]->status_actual_.is_no_actual_)) {
        bool ok;
        QString text = QInputDialog::getText(
            nullptr,
            "Изменение статуса актуальности",
            "Введите причину изменения статуса актуальности:",
            QLineEdit::Normal,
            "",
            &ok
            );

        if (ok && !text.isEmpty()) {
            model::Date date = {QDate::currentDate().day(),
                                QDate::currentDate().month(),
                                QDate::currentDate().year()};
            pool_work.value()[index]->status_actual_ = {true, date, text.toStdString()};
            QString qstr = QString("\n\nНеактуальна %1.%2.%3\n%4")
                               .arg(QDate::currentDate().day(), 2, 10, QChar('0'))
                               .arg(QDate::currentDate().month(), 2, 10, QChar('0'))
                               .arg(QDate::currentDate().year(), 4, 10, QChar('0'))
                               .arg(QString::fromStdString(text.toStdString()));
            std::string str_info = qstr.toStdString();
            pool_work.value()[index]->info_ = pool_work.value()[index]->info_.value() + str_info;
        }
    }
    else {
        pool_work.value()[index]->status_actual_ = {false, std::nullopt, std::nullopt};
        QString qstr = QString("\n\nВернули в работу %1.%2.%3")
                           .arg(QDate::currentDate().day(), 2, 10, QChar('0'))
                           .arg(QDate::currentDate().month(), 2, 10, QChar('0'))
                           .arg(QDate::currentDate().year(), 4, 10, QChar('0'));
        std::string str_info = qstr.toStdString();
        pool_work.value()[index]->info_ = pool_work.value()[index]->info_.value() + str_info;
    }
}

void MainWindow::on_ActionDeleteStage(std::optional<std::vector<std::shared_ptr<model::Stage>>>& pool_stage, int index) {
    pool_stage.value().erase(pool_stage.value().begin() + index);
    if (pool_stage.value().empty()) {
        pool_stage = std::nullopt;
    }
}

void MainWindow::on_ActionCompletStage(const std::optional<std::vector<std::shared_ptr<model::Stage>>>& pool_stage, int index) {
    if (!(pool_stage.value()[index]->status_complet_.is_complet_)) {
        model::Date date = {QDate::currentDate().day(),
                            QDate::currentDate().month(),
                            QDate::currentDate().year()};
        pool_stage.value()[index]->status_complet_ = {true, date};
        QString qstr = QString("\n\nВыполнен %1.%2.%3")
                           .arg(QDate::currentDate().day(), 2, 10, QChar('0'))
                           .arg(QDate::currentDate().month(), 2, 10, QChar('0'))
                           .arg(QDate::currentDate().year(), 4, 10, QChar('0'));
        std::string str_info = qstr.toStdString();
        pool_stage.value()[index]->info_ = pool_stage.value()[index]->info_.value() + str_info;
    }
    else {
        pool_stage.value()[index]->status_complet_ = {false, std::nullopt};
        QString qstr = QString("\n\nВернули в работу %1.%2.%3")
                           .arg(QDate::currentDate().day(), 2, 10, QChar('0'))
                           .arg(QDate::currentDate().month(), 2, 10, QChar('0'))
                           .arg(QDate::currentDate().year(), 4, 10, QChar('0'));
        std::string str_info = qstr.toStdString();
        pool_stage.value()[index]->info_ = pool_stage.value()[index]->info_.value() + str_info;
    }
}

void MainWindow::on_ActionActualStage(const std::optional<std::vector<std::shared_ptr<model::Stage>>>& pool_stage, int index) {
    if (!(pool_stage.value()[index]->status_actual_.is_no_actual_)) {
        bool ok;
        QString text = QInputDialog::getText(
            nullptr,
            "Изменение статуса актуальности",
            "Введите причину изменения статуса актуальности:",
            QLineEdit::Normal,
            "",
            &ok
            );

        if (ok && !text.isEmpty()) {
            model::Date date = {QDate::currentDate().day(),
                                QDate::currentDate().month(),
                                QDate::currentDate().year()};
            pool_stage.value()[index]->status_actual_ = {true, date, text.toStdString()};
            QString qstr = QString("\n\nНеактуальный %1.%2.%3\n%4")
                               .arg(QDate::currentDate().day(), 2, 10, QChar('0'))
                               .arg(QDate::currentDate().month(), 2, 10, QChar('0'))
                               .arg(QDate::currentDate().year(), 4, 10, QChar('0'))
                               .arg(QString::fromStdString(text.toStdString()));
            std::string str_info = qstr.toStdString();
            pool_stage.value()[index]->info_ = pool_stage.value()[index]->info_.value() + str_info;
        }
    }
    else {
        pool_stage.value()[index]->status_actual_ = {false, std::nullopt, std::nullopt};
        QString qstr = QString("\n\nВернули в работу %1.%2.%3")
                           .arg(QDate::currentDate().day(), 2, 10, QChar('0'))
                           .arg(QDate::currentDate().month(), 2, 10, QChar('0'))
                           .arg(QDate::currentDate().year(), 4, 10, QChar('0'));
        std::string str_info = qstr.toStdString();
        pool_stage.value()[index]->info_ = pool_stage.value()[index]->info_.value() + str_info;
    }
}

void MainWindow::SetRowBackgroundColor(int row, const QColor& color) {
    // Получаем количество столбцов в таблице
    int columnCount = ui->table_work->columnCount();

    // Перебираем все столбцы в указанной строке
    for (int col = 0; col < columnCount; ++col) {
        QTableWidgetItem* item = ui->table_work->item(row, col);

        // Если ячейка существует
        if (item) {
            // Устанавливаем цвет фона
            item->setBackground(color);
        } else {
            // Если ячейка пустая, создаем новый item
            item = new QTableWidgetItem();
            item->setBackground(color);
            ui->table_work->setItem(row, col, item);
        }
    }
}
