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
{
    ui->setupUi(this);
    ui->lb_authorization->setText(QString("Добро пожаловать, вы авторизовались как %1!").arg(QString::fromStdString(app->GetActivUserName())));
    setWindowTitle("Менеджер работ БИТ");

    AddMainTable();

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

WorkWindow::~WorkWindow()
{
    delete ui;
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

void WorkWindow::setupContractsTable() {

    AddMainTable();
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

void WorkWindow::AddMainTable() {
    // если в Layout есть виджит, то удаляем его
    DeleteWidgetInLayout();
    // Создаем модель и представление
/*
    ContractModel* model = new ContractModel(app_->GetContracts(), this);
    ContractView* tableView = new ContractView(this);
    tableView->setModel(model);
*/

// по новому
    auto& contracts = const_cast<std::vector<model::Contract>&>(app_->GetContracts()); // нужно переделать и просто добавить в app_ метод выдачи неконстантной ссылки

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
}

void WorkWindow::DeleteWidgetInLayout() {
    QLayoutItem* item;
    while ((item = ui->horizontalLayout_4->takeAt(0)) != 0) {
            if (item->widget()) {
                delete item->widget();
            }
    }
}

void WorkWindow::on_pushButton_clicked()
{
    setupContractsTable();
}







int WorkWindow::getCurrentContractIndex() const {
    if (!tableView_ || !model_) return -1;

    QModelIndex currentIndex = tableView_->currentIndex();
    if (!currentIndex.isValid()) return -1;

    auto itemInfo = model_->getItemInfo(currentIndex.row());

    // Возвращаем индекс контракта для любой строки (заголовок, контракт, этап, работа)
    return itemInfo.contractIndex;
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
        contracts[currentContextMenuRow_].is_complet_) {
            completeAction->setEnabled(false);
            completeAction->setText("Уже выполнен");
    }

    // Показываем меню и обрабатываем выбор
    QAction* selectedAction = contextMenu.exec(tableView_->viewport()->mapToGlobal(pos));

    if (selectedAction == deleteAction) {
            onDeleteContractAction();
    } else if (selectedAction == completeAction) {
            onMarkAsCompletedAction();
    }
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
