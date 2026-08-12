#include "workwindow.h"
#include "ui_workwindow.h"

#include "handler_docx.h"
#include "magic_defs.h"
#include "contractmodel.h"
#include "contractview.h"

#include <QFileDialog>
#include <QMessageBox>
#include <QMenu>
#include <QAction>
#include <QDebug>

WorkWindow::WorkWindow(std::shared_ptr<app::App> app, QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::WorkWindow)
    , app_(app)
    , tableView_(nullptr)
    , model_(nullptr)
    , currentContextMenuRow_(-1)
    , pos_employee_(std::nullopt)
    , currentViewType_(ViewType::Contracts)
{
    ui->setupUi(this);


    // Создаем виджет с фильтрами
    filterWidget_ = new FilterWidget(this);

    // Добавляем фильтры в layout
    if (ui->verticalLayout_3) {
        ui->verticalLayout_3->insertWidget(0, filterWidget_);
    }

    // Подключаем сигналы фильтров
//    connect(filterWidget_, &FilterWidget::filterCompletedChanged,
//            this, &WorkWindow::onFilterCompletedChanged);
//    connect(filterWidget_, &FilterWidget::filterPaidChanged,
//            this, &WorkWindow::onFilterPaidChanged);
//    connect(filterWidget_, &FilterWidget::filterTypeChanged,
//            this, &WorkWindow::onFilterTypeChanged);


    ui->lb_authorization->setText(QString("Добро пожаловать, вы авторизовались как %1!").arg(QString::fromStdString(app->GetActivUserName())));
    setWindowTitle("Менеджер работ БИТ");

    setupContractsView();
    UpdateTableEmployees();

//    setupContractsTable(); // нужно проверить нужен ли он тут

    /*
    // Обработка кликов для открытия окна редактирования
    connect(tableView, &QTableView::clicked, this, [this, model](const QModelIndex& index) {
        auto info = model->getItemInfo(index.row());

        // Открываем окно редактирования в зависимости от типа строки
        if (info.type == ContractModel::ItemInfo::Type::ContractRow) {
            // Открыть окно редактирования контракта
            openContractEditDialog(info.contractIndex);
        } else if (info.type == ContractModel::ItemInfo::Type::StageRow) {
            // Открыть окно редактирования этапа
            openStageEditDialog(info.contractIndex, info.stageIndex);
        } else if (info.type == ContractModel::ItemInfo::Type::WorkRow) {
            // Открыть окно редактирования работы
            openWorkEditDialog(info.contractIndex, info.stageIndex, info.workIndex);
        }
    });
*/
}

WorkWindow::~WorkWindow() {
    delete ui;
}

void WorkWindow::onUpdatTechnicalMainTable() {
    // Обновляем модель, которая сейчас установлена
    switchToView(currentViewType_);
}

void WorkWindow::on_pb_open_plan_month_clicked()
{
    QString fileName = QFileDialog::getOpenFileName(
        this,
        QString("Открыть файл с планом на месяц"),
        QDir::currentPath(), // QDir::currentPath — текущая папка.
        "*.doc;*.docx;*.odt"
        );

    ui->le_plan_month->setText(fileName);
    app_->SetPathPlanMonth(fileName.toStdString());

    // Дальше нужен актион

}


void WorkWindow::on_pb_create_plan_month_clicked()
{
    QString selected_filter;
    QString fileName = QFileDialog::getSaveFileName(this,
                                                    QString("Создать файл"),
                                                    QDir::currentPath(),
                                                    QString("%1;;%2;;%3").arg(TypeFile::ODT).arg(TypeFile::DOCX).arg(TypeFile::PDF),
                                                    &selected_filter);              // сюда запишется выбранный фильтр
    ui->le_plan_month->setText(fileName);
    app_->SetPathPlanMonth(fileName.toStdString());

    if (selected_filter == TypeFile::ODT) {

    }
    else if (selected_filter == TypeFile::DOCX) {

    }
    else if (selected_filter == TypeFile::PDF) {

    }
    else {
        QMessageBox::critical(this, "Ошибка", QString("Не могу создать файл формата %1").arg(selected_filter));
    }

}

void WorkWindow::on_pb_add_new_contract_clicked() {
    if (!(app_->HasValuePathPlanMonth())) {
        QMessageBox::StandardButton button = QMessageBox::question(
            this, "Добавление договора",
            "Не выбран файл с планом на месяц для добавления договора. Добавить договор только в базу?",
            QMessageBox::Ok | QMessageBox::Cancel
            );

        if (button == QMessageBox::Cancel) {
            return;
        }
    }
    emit AddContract();
}


void WorkWindow::on_pb_create_plan_month_default_clicked() {
    CreateDocxWithWord(app_->GetContracts());
    QMessageBox::information(this, "Успех", "План на месяц создан");
}

/*
void WorkWindow::setupContractsTable() {

    setupContractsView();
*//*
    // Обработка кликов для открытия окна редактирования
    connect(tableView, &QTableView::clicked, this, [this, model](const QModelIndex& index) {
        auto info = model->getItemInfo(index.row());

        // Открываем окно редактирования в зависимости от типа строки
        if (info.type == ContractModel::ItemInfo::Type::ContractRow) {
            // Открыть окно редактирования контракта
            openContractEditDialog(info.contractIndex);
        } else if (info.type == ContractModel::ItemInfo::Type::StageRow) {
            // Открыть окно редактирования этапа
            openStageEditDialog(info.contractIndex, info.stageIndex);
        } else if (info.type == ContractModel::ItemInfo::Type::WorkRow) {
            // Открыть окно редактирования работы
            openWorkEditDialog(info.contractIndex, info.stageIndex, info.workIndex);
        }
    });
*/

//}

void WorkWindow::setupContractsView() {
    // если в Layout есть виджит, то удаляем его
    DeleteWidgetInLayout();
    currentViewType_ = ViewType::Contracts;
    // Создаем модель и представление
/*
    ContractModel* model = new ContractModel(app_->GetContracts(), this);
    ContractView* tableView = new ContractView(this);
    tableView->setModel(model);
*/

// по новому
    auto& contracts = const_cast<std::vector<std::shared_ptr<model::Contract>>&>(app_->GetContracts()); // нужно переделать и просто добавить в app_ метод выдачи неконстантной ссылки

    // Создаем модель и представление
    model_ = new ContractModel(contracts, this);
    tableView_ = new ContractView(this);
    tableView_->setModel(model_);

    // Устанавливаем делегат для поддержки объединения ячеек
    tableView_->setItemDelegate(new SpanDelegate(tableView_));

// новое для контекстного меню -------

    // Настройка контекстного меню
    tableView_->setContextMenuPolicy(Qt::CustomContextMenu);
    connect(tableView_, &QTableView::customContextMenuRequested,
            this, &WorkWindow::onTableViewCustomContextMenuRequested);

    // Настройка внешнего вида
    tableView_->setSelectionBehavior(QAbstractItemView::SelectRows);
    tableView_->setSelectionMode(QAbstractItemView::SingleSelection);
// новое закончилось ------------------

    ui->horizontalLayout_4->addWidget(tableView_);

    // Применяем текущие фильтры
//    applyFilters();   // ПОКА НЕ РЕАЛИЗОВАНО
}

// Новое представление: только этапы
void WorkWindow::setupStagesView() {
    DeleteWidgetInLayout();
    currentViewType_ = ViewType::Stages;

    auto& contracts = const_cast<std::vector<std::shared_ptr<model::Contract>>&>(app_->GetContracts());

// НУЖНО РЕАЛИЗОВАТЬ ЭТУ МОДЕЛЬ ПРЕДСТАВЛЕНИЯ ЧЕРЕЗ АБСТРАКТНЫЙ КЛАСС
/*
    // Создаем новую модель для этапов
    model_ = new StagesOnlyModel(contracts, this);
    tableView_ = new QTableView(this);  // Используем обычный QTableView
    tableView_->setModel(model_);

    // НЕТ контекстного меню для этого представления
    tableView_->setContextMenuPolicy(Qt::NoContextMenu);

    // Настройка внешнего вида
    tableView_->setSelectionBehavior(QAbstractItemView::SelectRows);
    tableView_->setSelectionMode(QAbstractItemView::SingleSelection);
    tableView_->horizontalHeader()->setStretchLastSection(true);

    ui->horizontalLayout_4->addWidget(tableView_);
*/
}

// Новое представление: только работы
void WorkWindow::setupWorksView() {
    DeleteWidgetInLayout();
    currentViewType_ = ViewType::Works;

    auto& contracts = const_cast<std::vector<std::shared_ptr<model::Contract>>&>(app_->GetContracts());

// НУЖНО РЕАЛИЗОВАТЬ ЭТУ МОДЕЛЬ ПРЕДСТАВЛЕНИЯ ЧЕРЕЗ АБСТРАКТНЫЙ КЛАСС
/*
    // Создаем новую модель для работ
    model_ = new WorksOnlyModel(contracts, this);
    tableView_ = new QTableView(this);
    tableView_->setModel(model_);

    // НЕТ контекстного меню
    tableView_->setContextMenuPolicy(Qt::NoContextMenu);

    // Настройка внешнего вида
    tableView_->setSelectionBehavior(QAbstractItemView::SelectRows);
    tableView_->setSelectionMode(QAbstractItemView::SingleSelection);
    tableView_->horizontalHeader()->setStretchLastSection(true);

    ui->horizontalLayout_4->addWidget(tableView_);
*/
}

// Метод для переключения представлений
void WorkWindow::switchToView(ViewType type) {
    switch (type) {
    case ViewType::Contracts:
        setupContractsView();
        break;
    case ViewType::Stages:
        setupStagesView();
        break;
    case ViewType::Works:
        setupWorksView();
        break;
    }
}

// Слоты для кнопок
void WorkWindow::on_pb_show_contracts_clicked() {
    switchToView(ViewType::Contracts);
}

void WorkWindow::on_pb_show_stages_clicked() {
    switchToView(ViewType::Stages);
}

void WorkWindow::on_pb_show_works_clicked() {
    switchToView(ViewType::Works);
}


void WorkWindow::DeleteWidgetInLayout() {
    QLayoutItem* item;
    while ((item = ui->horizontalLayout_4->takeAt(0)) != 0) {
        if (item->widget()) {
            delete item->widget();
        }
    }
    delete item;

    // Дополнительно удаляем модель
    if (model_) {
        delete model_;
        model_ = nullptr;
    }
    tableView_ = nullptr;  // уже удален через виджет, просто обнуляем указатель
}

int WorkWindow::getCurrentContractIndex() const {
    if (!tableView_ || !model_) return -1;

    QModelIndex currentIndex = tableView_->currentIndex();
    if (!currentIndex.isValid()) return -1;

    auto itemInfo = model_->getItemInfo(currentIndex.row());

    // Возвращаем индекс контракта для любой строки (заголовок, контракт, этап, работа)
    return itemInfo.contractIndex;
}

std::shared_ptr<model::Contract> WorkWindow::getCurrentContract() const {
    int index = getCurrentContractIndex();
    if (index >= 0 && index < static_cast<int>(app_->GetContracts().size())) {
            return app_->GetContracts()[index];
    }
    return nullptr;
}

void WorkWindow::onTableViewCustomContextMenuRequested(const QPoint& pos) {
    QModelIndex index = tableView_->indexAt(pos);
    if (!index.isValid()) return;

    auto itemInfo = model_->getItemInfo(index.row());

    // Показываем меню только для строк контракта (не для этапов и работ)
    // Можно раскомментировать, если нужно для всех строк
    // if (itemInfo.type != ContractModel::ItemInfo::Type::ContractRow) return;

    currentContextMenuRow_ = itemInfo.contractIndex;

    QMenu contextMenu;

    // Действие "Изменить договор"
    QAction* editAction = contextMenu.addAction("Изменить договор");
    editAction->setIcon(QIcon::fromTheme("document-edit"));

    // Действие "Удалить договор"
    QAction* deleteAction = contextMenu.addAction("Удалить договор");
    deleteAction->setIcon(QIcon::fromTheme("edit-delete"));

    // Действие "Пометить как выполненный"
    QAction* completeAction = contextMenu.addAction("Пометить как выполненный");
    completeAction->setIcon(QIcon::fromTheme("task-complete"));

    // Проверяем, не выполнен ли уже договор
    const auto& contracts = app_->GetContracts();
    if (currentContextMenuRow_ >= 0 &&
        currentContextMenuRow_ < static_cast<int>(contracts.size()) &&
        contracts[currentContextMenuRow_]->status_complet_.is_complet_) {
            completeAction->setEnabled(false);
            completeAction->setText("Уже выполнен");
    }

    // Показываем меню и обрабатываем выбор
    QAction* selectedAction = contextMenu.exec(tableView_->viewport()->mapToGlobal(pos));

    if (selectedAction == editAction) {
            onEditContractAction();
    } else if (selectedAction == deleteAction) {
            onDeleteContractAction();
    } else if (selectedAction == completeAction) {
            onMarkAsCompletedAction();
    }
}

void WorkWindow::onEditContractAction() {
    auto contract = getCurrentContract();
    if (!contract) {
            QMessageBox::warning(this, "Ошибка", "Договор не найден");
            return;
    }

    // Испускаем сигнал для редактирования договора
    emit EditContract(contract);

    // Закрываем текущее окно, так как откроется новое
//    this->close();
}

void WorkWindow::onDeleteContractAction() {
    if (currentContextMenuRow_ < 0) return;

    // Подтверждение удаления
    QMessageBox::StandardButton reply = QMessageBox::question(
        this,
        "Подтверждение удаления",
        "Вы уверены, что хотите удалить этот договор?",
        QMessageBox::Yes | QMessageBox::No
        );

    if (reply == QMessageBox::Yes) {
            // Удаляем через модель
            if (model_->removeContract(currentContextMenuRow_)) {
                QMessageBox::information(this, "Успех", "Договор успешно удален");
            } else {
                QMessageBox::warning(this, "Ошибка", "Не удалось удалить договор");
            }
    }
}

void WorkWindow::onMarkAsCompletedAction() {
    if (currentContextMenuRow_ < 0) return;

    // Подтверждение
    QMessageBox::StandardButton reply = QMessageBox::question(
        this,
        "Подтверждение",
        "Пометить договор как выполненный?",
        QMessageBox::Yes | QMessageBox::No
        );

    if (reply == QMessageBox::Yes) {
            // Отмечаем как выполненный через модель
            if (model_->markContractAsCompleted(currentContextMenuRow_)) {
                QMessageBox::information(this, "Успех", "Договор помечен как выполненный");
            } else {
                QMessageBox::warning(this, "Ошибка", "Не удалось отметить договор");
            }
    }
}

void WorkWindow::on_pb_add_employee_clicked() {
    if (!(ui->le_add_employee->text().isEmpty())) {
        app_->AddBaseEmployee(ui->le_add_employee->text().toStdString());
        UpdateTableEmployees();
    };
    ui->le_add_employee->clear();
}


void WorkWindow::UpdateTableEmployees() {
    ui->lw_employees->clear();
    for (const auto& employee : app_->GetBaseEmployee()) {
        ui->lw_employees->addItem(QString::fromStdString(employee));
    }
    ApplyIterator();
}


void WorkWindow::ApplyIterator() {
    if (it_base_employees_ == app_->GetBaseEmployee().end()) {
        ui->le_delete_employee->setText("");
        ui->pb_delete_employee->setEnabled(false);
        return;
    }
    int pos = std::distance(app_->GetBaseEmployee().begin(), it_base_employees_);
    ui->lw_employees->setCurrentRow(pos);
    ui->le_delete_employee->setText(QString::fromStdString(*it_base_employees_));
    ui->pb_delete_employee->setEnabled(true);
}

void WorkWindow::on_pb_delete_employee_clicked() {
    app_->GetBaseEmployee().erase(it_base_employees_);
    it_base_employees_ = app_->GetBaseEmployee().begin();
    UpdateTableEmployees();
}


void WorkWindow::on_lw_employees_currentRowChanged(int currentRow) {
    if (currentRow < 0) {
        return;
    }

    if (currentRow >= static_cast<int>(app_->GetBaseEmployee().size())) {
        it_base_employees_ = app_->GetBaseEmployee().end();
    }
    else {
        auto it = app_->GetBaseEmployee().begin();
        std::advance(it, currentRow);
        it_base_employees_ = it;
    }
    ApplyIterator();
}


