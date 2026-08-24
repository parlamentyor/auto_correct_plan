#include "paymentwindow.h"
#include "ui_paymentwindow.h"
#include "general_qt_functions.h"

#include <QDialog>
#include <QMessageBox>

PaymentWindow::PaymentWindow(std::optional<std::vector<model::Payment>>& payments,
                             const std::string& activ_user,
                             QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::PaymentWindow)
    , payments_(payments)
    , activ_user_(activ_user)
    , date_({QDate::currentDate().day(),
             QDate::currentDate().month(),
             QDate::currentDate().year()}) {
    ui->setupUi(this);
    setWindowTitle("Редактирование оплаты");

    ui->de_add_data->setDate(QDate::currentDate());
    ui->de_add_data->setDisplayFormat("dd.MM.yyyy");
    connect(ui->de_add_data, &QDateEdit::dateChanged,
            this, &PaymentWindow::on_de_add_data_dateChanged);
    SetTableProperties();
    UpdateTable();
}

PaymentWindow::~PaymentWindow() {
    delete ui;
}

void PaymentWindow::on_de_add_data_dateChanged(const QDate &date) {
    if (date.isValid()) {
        date_.day_ = date.day();
        date_.month_ = date.month();
        date_.year_ = date.year();
    }
}

void PaymentWindow::on_pb_edit_add_data_clicked() {
    auto selected_date = details::OpenCalendar(this, date_);
    if (selected_date.has_value()) {
        date_ = selected_date.value();
        ui->de_add_data->setDate(QDate(date_.year_, date_.month_, date_.day_));
    }
}

void PaymentWindow::on_tw_payment_currentCellChanged(int currentRow, int currentColumn, int previousRow, int previousColumn) {
    // Проверяем, что currentRow валидный и что payments_ содержит данные
    if (currentRow != -1 && payments_.has_value() && currentRow < static_cast<int>(payments_.value().size())) {
        QString str = QString("от %1 на сумму %2,%3")
                        .arg(details::FormatDateToQstring(payments_.value().at(currentRow).date_))
                        .arg(payments_.value().at(currentRow).price_.ruble_)
                        .arg(payments_.value().at(currentRow).price_.kop_);
        ui->le_delete_payment->setText(str);
        ui->pb_delete_payment->setEnabled(true);
        current_row_ = currentRow;
    }
    else {
        ui->le_delete_payment->setText("");
        ui->pb_delete_payment->setEnabled(false);
        current_row_ = -1;
    }
}

void PaymentWindow::SetTableProperties() {
    // Настройка растягивания колонок
//    ui->tw_payment->horizontalHeader()->setSectionResizeMode(0, QHeaderView::ResizeToContents);
//    ui->tw_payment->horizontalHeader()->setSectionResizeMode(1, QHeaderView::ResizeToContents);
    ui->tw_payment->horizontalHeader()->setSectionResizeMode(0, QHeaderView::Stretch);
    ui->tw_payment->horizontalHeader()->setSectionResizeMode(1, QHeaderView::Stretch);

/*
    int totalWidth = ui->tw_payment->viewport()->width();
    ui->tw_payment->horizontalHeader()->setSectionResizeMode(0, QHeaderView::Interactive);
    ui->tw_payment->horizontalHeader()->setSectionResizeMode(1, QHeaderView::Interactive);
    ui->tw_payment->horizontalHeader()->resizeSection(0, totalWidth * 0.20);
    ui->tw_payment->horizontalHeader()->resizeSection(0, totalWidth * 0.80);
*/
    // Включаем перенос текста для всех ячеек
    ui->tw_payment->setWordWrap(true);

    // Настройка автоматической высоты строк
    ui->tw_payment->verticalHeader()->setSectionResizeMode(QHeaderView::ResizeToContents);
}

void PaymentWindow::UpdateTable() {
    ui->tw_payment->setRowCount(0);

    // Очищаем выделение и поля удаления, если таблица пуста
    if (!payments_.has_value() || payments_.value().empty()) {
        ui->le_delete_payment->setText("");
        ui->pb_delete_payment->setEnabled(false);
        return;
    }

    for (const auto& item : payments_.value()) {
        // Получаем текущее количество строк
        int rowCount = ui->tw_payment->rowCount();
        // Добавляем новую строку
        ui->tw_payment->insertRow(rowCount);
        // Заполняем первый столбец (Дата)
        ui->tw_payment->setItem(rowCount, 0,
                                new QTableWidgetItem(details::FormatDateToQstring(item.date_)));
        // Заполняем второй столбец (сумма)
        ui->tw_payment->setItem(rowCount, 1,
                                new QTableWidgetItem(QString("%1,%2 рублей").arg(item.price_.ruble_).arg(item.price_.kop_)));
    }
}

void PaymentWindow::on_pb_add_payment_clicked() {
    if (ui->sb_price_add_ruble->value() == 0) {
        QMessageBox::warning(this, "Добавление оплаты", "Отсутстует сумма оплаты!");
        return;
    }
    if (!payments_.has_value()) {
        payments_ = std::vector<model::Payment>{};
    }
    payments_.value().push_back({{ui->sb_price_add_ruble->value(), ui->sb_price_add_kop->value()},
                                 date_, activ_user_});
    UpdateTable();
    emit UpdateTableGlobal();
}


void PaymentWindow::on_pb_delete_payment_clicked() {
    if (payments_.has_value() && current_row_ != -1) {
        auto& payments = payments_.value();

        auto it = payments.begin() + current_row_;

        if (it != payments.end()) {
            payments.erase(it);
            if (payments.empty()) {
                ui->tw_payment->clearSelection();
                ui->le_delete_payment->setText("");
                ui->pb_delete_payment->setEnabled(false);
                payments_ = std::nullopt;
            }
            UpdateTable();
        }
    }
    emit UpdateTableGlobal();
}

