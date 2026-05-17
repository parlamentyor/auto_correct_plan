#include "windowmanager.h"
#include "my_logger.h"
#include "serialization_qt_json.h"
#include "general_functions.h"

#include "QCoreApplication"

WindowManager::WindowManager(std::shared_ptr<app::App> app, QObject *parent)
    : QObject(parent)
    , app_(app)
    , authoriz_(nullptr)
    , work_window_(nullptr)
    , main_window_(nullptr)
    , add_stage_(nullptr)
    , add_work_window_(nullptr)
    , expenses_window_(nullptr)
    , payments_window_(nullptr) {
    connect(qApp, &QCoreApplication::aboutToQuit, this, &WindowManager::onAppAboutToQuit);
}

void WindowManager::Start() {
    authoriz_ = new Authorization(app_);
    connect(authoriz_, &Authorization::loginSuccess, this, &WindowManager::onLoginSuccess);
    authoriz_->show();
    LOG("Запустилось окно авторизации");

}

void WindowManager::onLoginSuccess() {
    work_window_ = new WorkWindow(app_);
    connect(work_window_, &WorkWindow::AddContract, this, &WindowManager::onAddContract);
    work_window_->show();
    authoriz_->close();
}

void WindowManager::onAddContract() {
    if (!main_window_) {
        main_window_ = new MainWindow(app_);
        main_window_->setAttribute(Qt::WA_DeleteOnClose);
        connect(main_window_, &MainWindow::AddStageInContract, this, &WindowManager::onAddStageInContract);
        connect(main_window_, &MainWindow::AddWorkInContract, this, &WindowManager::onAddWorkInContract);
        connect(main_window_, &MainWindow::AddExpensesInContract, this, &WindowManager::onAddExpensesInContract);
        connect(main_window_, &MainWindow::EditPaymentsInContract, this, &WindowManager::onEditPayments);
        // Дополнительно: отслеживаем уничтожение окна, чтобы обнулить указатель
        connect(main_window_, &QObject::destroyed, this, [this]() {
            main_window_ = nullptr;
        });
    }
    main_window_->show();
}

void WindowManager::onAddWorkInContract(std::shared_ptr<app::App> app,
                                        std::optional<std::vector<model::SeparateWork>>& pool_work) {
    if (!add_work_window_) {
        add_work_window_ = new AddWorkWindow(app, pool_work);
        add_work_window_->setAttribute(Qt::WA_DeleteOnClose);
        connect(add_work_window_, &QObject::destroyed, this, [this]() {
            add_work_window_ = nullptr;
        });

        // Соединяем сигнал со слотом менеджера для обновления таблицы в окне добавления договора
        connect(add_work_window_, &AddWorkWindow::UpdateTable,
                this, &WindowManager::onUpdateTable);
    }
    add_work_window_->show();
}

void WindowManager::onAddWorkInStage(std::optional<std::vector<model::SeparateWork> > &pool_work) {
    if (!add_work_window_) {
        add_work_window_ = new AddWorkWindow(app_, pool_work);
        add_work_window_->setAttribute(Qt::WA_DeleteOnClose);
        connect(add_work_window_, &QObject::destroyed, this, [this]() {
            add_work_window_ = nullptr;
        });

        // Соединяем сигнал со слотом менеджера для обновления таблицы в окне добавления этапа
        connect(add_work_window_, &AddWorkWindow::UpdateTable,
                this, &WindowManager::onUpdateTableWorkInStage);
    }
    add_work_window_->show();
}

void WindowManager::onUpdateTableWorkInStage() {
    if (add_stage_) {
        add_stage_->toUpdateTableWorkInStage();
    } else {
        LOG("Ошибка: add_stage_ не существует при попытке обновить таблицу");
        // Можно создать main_window_ если нужно
        // onAddContract();
        // if (main_window_) main_window_->toUpdateTable();
    }
}

void WindowManager::onAppAboutToQuit() {
    LOG("Application is about to quit, saving state...");
    QString stateFile = QString::fromStdString(details::CreatePathDokument("state", "app_state.json"));
    if (serialization::SaveToJsonFile(stateFile, app_)) {
        LOG("При закрытии программы состояние сохранено успешно по адресу:", stateFile.toStdString());
    }
    else {
        LOG("ОШИБКА: При закрытии программы состояние не сохранилось");
    }
}

void WindowManager::onAddStageInContract(std::optional<std::vector<model::Stage>> &pool_stage) {
    if (!add_stage_) {
        add_stage_ = new AddStage(app_, pool_stage);
        add_stage_->setAttribute(Qt::WA_DeleteOnClose);
        connect(add_stage_, &AddStage::AddWorkInStage, this, &WindowManager::onAddWorkInStage);
        connect(add_stage_, &AddStage::AddExpensesInStage, this, &WindowManager::onAddExpensesInStage);
        connect(add_stage_, &AddStage::EditPaymentsInStage, this, &WindowManager::onEditPayments);
        connect(add_stage_, &QObject::destroyed, this, [this]() {
            add_stage_ = nullptr;
        });

        // Соединяем сигнал со слотом менеджера для обновления таблицы в окне добавления договора
        connect(add_stage_, &AddStage::UpdateTable,
                this, &WindowManager::onUpdateTable);
    }
    add_stage_->show();
}

void WindowManager::onUpdateTable()
{
    if (main_window_) {
        main_window_->toUpdateTable();
    } else {
        LOG("Ошибка: main_window_ не существует при попытке обновить таблицу");
        // Можно создать main_window_ если нужно
        // onAddContract();
        // if (main_window_) main_window_->toUpdateTable();
    }
}

void WindowManager::onAddExpensesInContract(std::shared_ptr<app::App> app, std::optional<std::vector<model::Expenses>> &expenses) {
    if (!expenses_window_) {
        expenses_window_ = new ExpensesWindow(app, expenses);
        expenses_window_->setAttribute(Qt::WA_DeleteOnClose);
        connect(expenses_window_, &QObject::destroyed, this, [this]() {
            expenses_window_ = nullptr;
        });

        // Соединяем сигнал со слотом менеджера для обновления таблицы в окне добавления договора
        // В будущем нужно отображать затраты в главном окне
//        connect(expenses_window_, &ExpensesWindow::UpdateTable,
//                this, &WindowManager::onUpdateTable);
    }
    expenses_window_->show();
}

void WindowManager::onAddExpensesInStage(std::optional<std::vector<model::Expenses>> &expenses) {
//     !!!!!!!!!!!!!!!!!!!------ПО ФАКТУ МОЖНО СДЕЛАТЬ ОДИН СЛОТ onAddExpenses--------------!!!!!!!!!!!!!!!!!!!!
    if (!expenses_window_) {
        expenses_window_ = new ExpensesWindow(app_, expenses);
        expenses_window_->setAttribute(Qt::WA_DeleteOnClose);
        connect(expenses_window_, &QObject::destroyed, this, [this]() {
            expenses_window_ = nullptr;
        });

        // Соединяем сигнал со слотом менеджера для обновления таблицы в окне добавления договора
        // В будущем нужно отображать затраты в главном окне
        //        connect(expenses_window_, &ExpensesWindow::UpdateTable,
        //                this, &WindowManager::onUpdateTable);
    }
    expenses_window_->show();
}

void WindowManager::onEditPayments(std::optional<std::vector<model::Payment>> &payments) {
    if (!payments_window_) {
        payments_window_ = new PaymentWindow(payments);
        payments_window_->setAttribute(Qt::WA_DeleteOnClose);
        connect(payments_window_, &QObject::destroyed, this, [this]() {
            payments_window_ = nullptr;
        });

        // Соединяем сигнал со слотом менеджера для обновления таблицы в окне добавления договора
        // В будущем нужно отображать затраты в главном окне
        //        connect(payments_window_, &PaymentWindow::UpdateTable,
        //                this, &WindowManager::onUpdateTable);
    }
    payments_window_->show();
}

