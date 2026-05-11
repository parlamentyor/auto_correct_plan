#include "expenseswindow.h"
#include "ui_expenseswindow.h"

#include <QMessageBox>

ExpensesWindow::ExpensesWindow(std::shared_ptr<app::App> app,
                               std::optional<std::vector<model::Expenses> > &expenses,
                               QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::ExpensesWindow)
    , app_(app)
    , expenses_(expenses) {
    ui->setupUi(this);
    setWindowTitle("Добавление затрат");
    SetTableProperties();
    UpdateTable();
}

ExpensesWindow::~ExpensesWindow()
{
    delete ui;
}

void ExpensesWindow::SetTableProperties() {
    // Настройка растягивания колонок
    ui->tw_expenses->horizontalHeader()->setSectionResizeMode(0, QHeaderView::Stretch);
    ui->tw_expenses->horizontalHeader()->setSectionResizeMode(1, QHeaderView::ResizeToContents);

    // Включаем перенос текста для всех ячеек
    ui->tw_expenses->setWordWrap(true);

    // Настройка автоматической высоты строк
    ui->tw_expenses->verticalHeader()->setSectionResizeMode(QHeaderView::ResizeToContents);
}

void ExpensesWindow::UpdateTable() {
    ui->tw_expenses->setRowCount(0);
    if (expenses_.has_value()) {
        for (const auto& item : expenses_.value()) {
            // Получаем текущее количество строк
            int rowCount = ui->tw_expenses->rowCount();
            // Добавляем новую строку
            ui->tw_expenses->insertRow(rowCount);
            // Заполняем первый столбец (наименование)
            ui->tw_expenses->setItem(rowCount, 0,
                                     new QTableWidgetItem(QString::fromStdString(item.name_)));
            // Заполняем второй столбец (стоимость)
            ui->tw_expenses->setItem(rowCount, 1,
                                     new QTableWidgetItem(QString("%1,%2 рублей").arg(item.price_.ruble_).arg(item.price_.kop_)));
        }
    }
}

void ExpensesWindow::on_pb_add_clicked() {
    if (ui->le_add_name->text().isEmpty()) {
        QMessageBox::warning(this, "Добавление затрат", "Отсутстует наименование затраты!");
        return;
    }
    if (ui->sb_price_add_ruble->value() == 0) {
        QMessageBox::warning(this, "Добавление затрат", "Отсутстует стоимость затраты!");
        return;
    }
    if (!expenses_.has_value()) {
        expenses_ = std::vector<model::Expenses>{};
    }
    expenses_.value().push_back({{ui->sb_price_add_ruble->value(), ui->sb_price_add_kop->value()},
                                  ui->le_add_name->text().toStdString()});
    UpdateTable();
}

