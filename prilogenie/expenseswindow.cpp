#include "expenseswindow.h"
#include "ui_expenseswindow.h"

#include <QMessageBox>
#include <QCompleter>

ExpensesWindow::ExpensesWindow(std::shared_ptr<app::App> app,
                               std::optional<std::vector<model::Expenses> > &expenses,
                               QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::ExpensesWindow)
    , app_(app)
    , expenses_(expenses) {
    ui->setupUi(this);
    setWindowTitle("Добавление затрат");
    SetCompleter(ui->le_add_name, app_->GetBaseExpenses());
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

void ExpensesWindow::SetCompleter(QLineEdit *le, const std::set<std::string> &base) {
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

