#include "addstage.h"
#include "ui_addstage.h"

#include <handler_add_work.h>

#include <QMessageBox>
#include <QCalendarWidget>
#include <QCompleter>
#include <QInputDialog>

AddStage::AddStage(std::shared_ptr<app::App> app,
                   QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::AddStage)
    , stage_(std::make_shared<model::Stage>())
    , date_({QDate::currentDate().day(),
             QDate::currentDate().month(),
             QDate::currentDate().year()})
    , app_(app) {
    ui->setupUi(this);
    setWindowTitle("Добавление этапа");
    ui->pb_correct->setVisible(false);
    SetCompleter(ui->le_responsible_employee, app_->GetBaseEmployee());

    ui->de_deadline_data->setDate(QDate::currentDate());
    ui->de_deadline_data->setDisplayFormat("dd.MM.yyyy");
    connect(ui->de_deadline_data, &QDateEdit::dateChanged,
            this, &AddStage::on_de_deadline_data_dateChanged);

    // Подключаем сигнал изменения ячейки таблицы
    connect(ui->table_work, &QTableWidget::cellChanged,
            this, &AddStage::on_table_work_cellChanged);

    SetTableProperties(ui->table_work);

// Убрал потому, что хочу убрать из конструктора pool_stage и поле pool_stage_
// Вторая причина этап нужно делать и инт и еще непойми что (может быть 1,2,3... а может 1.1, 17.2.4 и ТД
/*
    if (!pool_stage_.has_value()) {
        ui->le_number->setText("1");
    }
    else {
        ui->le_number->setText(QString::number(pool_stage_.value().size() + 1));
    }
*/
    // Для контекстного меню
    // 1. Включаем политику, разрешающую генерацию сигнала при запросе контекстного меню
    ui->table_work->setContextMenuPolicy(Qt::CustomContextMenu);

    // 2. Подключаем сигнал к нашему слоту
    connect(ui->table_work, &QTableWidget::customContextMenuRequested,
            this, &AddStage::ShowContextMenu);
}

AddStage::AddStage(std::shared_ptr<app::App> app,
                   std::shared_ptr<model::Stage> edit_stage,
                   QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::AddStage)
    , stage_(edit_stage)
    , date_({QDate::currentDate().day(),
             QDate::currentDate().month(),
             QDate::currentDate().year()})
    , app_(app) {
    ui->setupUi(this);
    setWindowTitle("Корректировка этапа");
    ui->pb_correct->setVisible(true);
    ui->pb_add_stage->setVisible(false);
    SetCompleter(ui->le_responsible_employee, app_->GetBaseEmployee());

    if (stage_->date_deadline_.has_value()) {
        date_ = stage_->date_deadline_.value();
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
    connect(ui->de_deadline_data, &QDateEdit::dateChanged,
            this, &AddStage::on_de_deadline_data_dateChanged);

    // Подключаем сигнал изменения ячейки таблицы
    connect(ui->table_work, &QTableWidget::cellChanged,
            this, &AddStage::on_table_work_cellChanged);

    SetTableProperties(ui->table_work);

    if (stage_->name_full_.has_value()) {
        ui->le_name_full->setText(QString::fromStdString(stage_->name_full_.value()));
    }
    ui->le_number->setText(QString::fromStdString(stage_->number_));

    if (stage_->name_responsible_employee_.has_value()) {
        ui->le_responsible_employee->setText(QString::fromStdString(stage_->name_responsible_employee_.value()));
    }
// Не будем давать возможность тереть информацию, только добавлять
/*
    if (stage_->info_.has_value()) {
        ui->le_info->setText(QString::fromStdString(stage_->info_.value()));
    }
*/

    ui->sb_price_ruble->setValue(stage_->price_.ruble_);
    ui->sb_price_kop->setValue(stage_->price_.kop_);
    ui->sb_price_other_department_ruble->setValue(stage_->price_other_department_.ruble_);
    ui->sb_price_other_department_kop->setValue(stage_->price_other_department_.kop_);

    if (stage_->status_payment_.has_value()) {
        ui->le_status_payment->setText(QString::fromStdString(stage_->status_payment_.value()));
    } 

    // Для контекстного меню
    // 1. Включаем политику, разрешающую генерацию сигнала при запросе контекстного меню
    ui->table_work->setContextMenuPolicy(Qt::CustomContextMenu);

    // 2. Подключаем сигнал к нашему слоту
    connect(ui->table_work, &QTableWidget::customContextMenuRequested,
            this, &AddStage::ShowContextMenu);

    UpdateTableWorkInStage();
}

AddStage::~AddStage()
{
    delete ui;
}

void AddStage::toUpdateTableWorkInStage() {
    UpdateTableWorkInStage();
}

void AddStage::toAddNewWork(std::shared_ptr<model::SeparateWork> new_work) {
    // Добавляем в pool_work_
    if (!stage_->pool_work_.has_value()) {
        stage_->pool_work_ = std::vector<std::shared_ptr<model::SeparateWork>>();
    }
    stage_->pool_work_->push_back(new_work);
}

void AddStage::on_pb_add_work_att_as_clicked() {
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

    if (!stage_->pool_work_.has_value()) {
        stage_->pool_work_ = std::vector<std::shared_ptr<model::SeparateWork>>{};
    }
    stage_->pool_work_.value().push_back(razrab_PIM);
    stage_->pool_work_.value().push_back(att_as);
    stage_->pool_work_.value().push_back(razrab_doc);

    UpdateTableWorkInStage();
}

void AddStage::on_pb_add_stage_clicked() {

    if (ui->cb_with_deadline_data->isChecked() == true && ui->le_info->text().isEmpty()) {
        QMessageBox::warning(this, "Ошибка", "Добавьте в поле Дополнительная информация причину отсутствия даты исполнения по этапу");
        return;
    }

    if (ui->le_responsible_employee->text().isEmpty()) {
        QMessageBox::warning(this, "Ошибка", "Не назначен ответственный работник!");
        return;
    }

    QString qstr = QString("Создана %1.%2.%3\n")
                       .arg(QDate::currentDate().day(), 2, 10, QChar('0'))
                       .arg(QDate::currentDate().month(), 2, 10, QChar('0'))
                       .arg(QDate::currentDate().year(), 4, 10, QChar('0'));
    std::string str_info;
    if (ui->le_info->text().isEmpty()) {
        str_info = qstr.toStdString() + "\n";
    }
    else {
        str_info = qstr.toStdString() + ui->le_info->text().toStdString() + "\n\n";
    }

    model::Price price = {ui->sb_price_ruble->value(), ui->sb_price_kop->value()};

    stage_->number_ = ui->le_number->text().toStdString();
    stage_->name_full_ = ui->le_name_full->text().toStdString();
    stage_->date_deadline_ = date_;
    stage_->name_responsible_employee_ = ui->le_responsible_employee->text().toStdString();
    stage_->price_ = price;
    stage_->price_other_department_ = {ui->sb_price_other_department_ruble->value(), ui->sb_price_other_department_kop->value()};
    stage_->info_ = str_info;
    stage_->status_complet_ = {false, std::nullopt};
    stage_->status_actual_ = {false, std::nullopt, std::nullopt};
    stage_->is_paid_ = IsPaid(price);
    stage_->status_payment_ = ui->le_status_payment->text().toStdString();

    emit AddNewStage(stage_);

    emit UpdateTable();

    QMessageBox::information(this, "Добавление этапа", "Этап добавлен!");
}



void AddStage::on_pb_correct_clicked() {
    if (ui->cb_with_deadline_data->isChecked() == true && ui->le_info->text().isEmpty()) {
        QMessageBox::warning(this, "Ошибка", "Добавьте в поле Дополнительная информация причину отсутствия даты исполнения по этапу");
        return;
    }

    if (ui->le_responsible_employee->text().isEmpty()) {
        QMessageBox::warning(this, "Ошибка", "Не назначен ответственный работник!");
        return;
    }

    QString qstr = QString("Внесены изменения %1.%2.%3\n")
                       .arg(QDate::currentDate().day(), 2, 10, QChar('0'))
                       .arg(QDate::currentDate().month(), 2, 10, QChar('0'))
                       .arg(QDate::currentDate().year(), 4, 10, QChar('0'));
    std::string str_info = qstr.toStdString() + ui->le_info->text().toStdString();

    model::Price price = {ui->sb_price_ruble->value(), ui->sb_price_kop->value()};
    stage_->number_ = ui->le_number->text().toStdString();
    stage_->name_full_ = ui->le_name_full->text().toStdString();
    stage_->date_deadline_ = date_;
    stage_->name_responsible_employee_ = ui->le_responsible_employee->text().toStdString();
    stage_->price_ = price;
    stage_->is_paid_ = IsPaid(price);
    stage_->price_other_department_ = {ui->sb_price_other_department_ruble->value(), ui->sb_price_other_department_kop->value()};
    if (!stage_->info_.has_value()) {
        stage_->info_ = str_info;
    }
    else {
        stage_->info_ = stage_->info_.value() + "\n\n" + str_info;
    }
    stage_->status_payment_ = ui->le_status_payment->text().toStdString();

    emit UpdateTable();

    QMessageBox::information(this, "Изменение этапа", "Этап изменен!");
}

void AddStage::on_pb_add_work_clicked() {
    emit AddWorkInStage();
}

void AddStage::on_table_work_cellChanged(int row, int column)
{
    // Отключаем сигнал временно, чтобы избежать рекурсии
    ui->table_work->blockSignals(true);

    // Проверяем, что pool_work_ инициализирован и row в пределах
    if (!stage_->pool_work_.has_value() || row >= static_cast<int>(stage_->pool_work_->size())) {
        ui->table_work->blockSignals(false);
        return;
    }

    // Получаем текущий элемент
    model::SeparateWork& work = *(stage_->pool_work_.value())[row];
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

void AddStage::UpdateTableWorkInStage() {
    // Отключаем сигнал временно, чтобы избежать рекурсии
    ui->table_work->blockSignals(true);
    ui->table_work->setRowCount(0);

    if (stage_->pool_work_.has_value()) {
        for (const auto& work : stage_->pool_work_.value()) {
            details::AddSeparateWorkToTable(ui->table_work, *work);
            int row = ui->table_work->rowCount() - 1;
            SetRowBackgroundColor(row, GetColor(work));  //Светло-зеленый
        }
    }

    SetTableProperties(ui->table_work);
    // Включаем сигналы обратно, чтобы изменения в таблице сразу заносились в работы/этапы
    ui->table_work->blockSignals(false);
}

QColor AddStage::GetColor(const std::shared_ptr<model::SeparateWork>& work) {
    if (!(work->status_actual_.is_no_actual_)) {
        if (work->status_complet_.is_complet_) {
            return  QColor(144, 238, 144);          //Светло-зеленый
        }
        else {
            QDate date = QDate(work->date_deadline_.value().year_, work->date_deadline_.value().month_, work->date_deadline_.value().day_);
            if (QDate::currentDate() > date) {
                return  QColor(255, 0, 0);          //Красный
            }
            else {
                return  QColor(Qt::transparent);    //Прозрачный или QColor(0, 0, 0, 0)
            }
        }
    }
    return Qt::lightGray;                            //Светло-серый

}

bool AddStage::IsPaid(const model::Price &price) {
    if (price.ruble_ == 0 & price.kop_ == 0) {
        return true;
    }
    if (stage_->payments_.has_value()) {
        model::Price total_payments;
        for (const model::Payment& payment : stage_->payments_.value()) {
            total_payments += payment.price_;
        }
        if (total_payments >= price) {
            return true;
        }
    }
    return false;
}

void AddStage::SetCompleter(QLineEdit *le, const std::set<std::string> &base) {
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

void AddStage::on_pb_edit_deadline_data_clicked() {
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

void AddStage::on_de_deadline_data_dateChanged(const QDate &date)
{
    if (date.isValid()) {
        date_.value().day_ = date.day();
        date_.value().month_ = date.month();
        date_.value().year_ = date.year();
    }
}


void AddStage::on_cb_with_deadline_data_stateChanged(int arg1)
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

void AddStage::on_pb_expenses_clicked() {
    emit AddExpensesInStage(stage_->expenses_);
}


void AddStage::on_pb_payments_clicked() {
    emit EditPaymentsInStage(stage_->payments_);
}


void AddStage::ShowContextMenu(const QPoint &pos) {
    // pos - это координаты клика относительно виджета tableWidget.
    // Чтобы узнать, по какой именно ячейке кликнули, используем itemAt().
    QTableWidgetItem* item = ui->table_work->itemAt(pos);

    if (!item) {
        // Кликнули не по ячейке, а по пустой области таблицы
        return;
    }

    // Сохраняем строку, чтобы потом производить действия над ней
    int index = item->row();

    // Создаем и настраиваем меню
    QMenu menu;
    QAction* action_edit = menu.addAction("Редактировать работу");
    QAction* action_delete = menu.addAction("Удалить работу");
    QAction* action_complet = menu.addAction("Изменить статус выполнения");
    QAction* action_actual = menu.addAction("Изменить статус актуальности");

    if (stage_->pool_work_.value()[index]->status_complet_.is_complet_) {
        action_edit->setEnabled(false);
        action_delete->setEnabled(false);
        action_actual->setEnabled(false);
    }

    if (stage_->pool_work_.value()[index]->status_actual_.is_no_actual_) {
        action_edit->setEnabled(false);
        action_delete->setEnabled(false);
        action_complet->setEnabled(false);
    }

    // Показываем меню в точке клика. mapToGlobal преобразует локальные координаты в глобальные.
    QAction* selectedAction = menu.exec(ui->table_work->mapToGlobal(pos));

    if (selectedAction == action_edit) {
        emit EditWork(stage_->pool_work_.value()[index]);
    }
    else if (selectedAction == action_delete) {
        on_ActionDelete(index);
    }
    else if (selectedAction == action_complet) {
        on_ActionComplet(index);
    }
    else if (selectedAction == action_actual) {
        on_ActionActual(index);
    }
    UpdateTableWorkInStage();
}

void AddStage::on_ActionDelete(int index) {
    stage_->pool_work_.value().erase(stage_->pool_work_.value().begin() + index);
    if (stage_->pool_work_.value().empty()) {
        stage_->pool_work_ = std::nullopt;
    }
}

void AddStage::on_ActionComplet(int index) {
    if (!(stage_->pool_work_.value()[index]->status_complet_.is_complet_)) {
        model::Date date = {QDate::currentDate().day(),
                            QDate::currentDate().month(),
                            QDate::currentDate().year()};
        stage_->pool_work_.value()[index]->status_complet_ = {true, date};
        QString qstr = QString("\n\nВыполнена %1.%2.%3")
                           .arg(QDate::currentDate().day(), 2, 10, QChar('0'))
                           .arg(QDate::currentDate().month(), 2, 10, QChar('0'))
                           .arg(QDate::currentDate().year(), 4, 10, QChar('0'));
        std::string str_info = qstr.toStdString();
        stage_->pool_work_.value()[index]->info_ = stage_->pool_work_.value()[index]->info_.value() + str_info;
    }
    else {
        stage_->pool_work_.value()[index]->status_complet_ = {false, std::nullopt};
        QString qstr = QString("\n\nВернули в работу %1.%2.%3")
                           .arg(QDate::currentDate().day(), 2, 10, QChar('0'))
                           .arg(QDate::currentDate().month(), 2, 10, QChar('0'))
                           .arg(QDate::currentDate().year(), 4, 10, QChar('0'));
        std::string str_info = qstr.toStdString();
        stage_->pool_work_.value()[index]->info_ = stage_->pool_work_.value()[index]->info_.value() + str_info;
    }
}

void AddStage::on_ActionActual(int index) {
    if (!(stage_->pool_work_.value()[index]->status_actual_.is_no_actual_)) {
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
            stage_->pool_work_.value()[index]->status_actual_ = {true, date, text.toStdString()};
            QString qstr = QString("\n\nНеактуальна %1.%2.%3\n%4")
                               .arg(QDate::currentDate().day(), 2, 10, QChar('0'))
                               .arg(QDate::currentDate().month(), 2, 10, QChar('0'))
                               .arg(QDate::currentDate().year(), 4, 10, QChar('0'))
                               .arg(QString::fromStdString(text.toStdString()));
            std::string str_info = qstr.toStdString();
            stage_->pool_work_.value()[index]->info_ = stage_->pool_work_.value()[index]->info_.value() + str_info;
        }
    }
    else {
        stage_->pool_work_.value()[index]->status_actual_ = {false, std::nullopt, std::nullopt};
        QString qstr = QString("\n\nВернули в работу %1.%2.%3")
                           .arg(QDate::currentDate().day(), 2, 10, QChar('0'))
                           .arg(QDate::currentDate().month(), 2, 10, QChar('0'))
                           .arg(QDate::currentDate().year(), 4, 10, QChar('0'));
        std::string str_info = qstr.toStdString();
        stage_->pool_work_.value()[index]->info_ = stage_->pool_work_.value()[index]->info_.value() + str_info;
    }
}

void AddStage::SetRowBackgroundColor(int row, const QColor& color) {
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

