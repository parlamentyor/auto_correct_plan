#ifndef WINDOWMANAGER_H
#define WINDOWMANAGER_H

#include <QObject>
#include <memory>

#include "app.h"
#include "authorization.h"
#include "addstage.h"
#include "addworkwindow.h"
#include "workwindow.h"
#include "mainwindow.h"
#include "expenseswindow.h"
#include "paymentwindow.h"

class WindowManager : public QObject
{
    Q_OBJECT
public:
    explicit WindowManager(std::shared_ptr<app::App> app, QObject *parent = nullptr);

    void Start();

private slots:
    void onLoginSuccess();
    void onAddContract();
    void onEditContract(std::shared_ptr<model::Contract> contract);
    void onAddWorkInContract(std::shared_ptr<app::App> app);
    void onAddWorkInStage();
    void onUpdateTableWorkInStage();
    void onAppAboutToQuit();
    void onAddStageInContract();
    void onUpdateTable();
    void onAddExpensesInContract(std::shared_ptr<app::App> app,
                                 std::optional<std::vector<model::Expenses>>& expenses);
    void onAddExpensesInStage(std::optional<std::vector<model::Expenses>>& expenses);
    void onEditPayments(std::optional<std::vector<model::Payment>> &payments);
    void onEditWork(std::shared_ptr<model::SeparateWork> edit_work);
    void onEditStage(std::shared_ptr<model::Stage> edit_stage);

private:
    std::shared_ptr<app::App> app_;
    Authorization *authoriz_;
    WorkWindow *work_window_;
    MainWindow *main_window_;
    AddStage *add_stage_;
    AddWorkWindow *add_work_window_;
    ExpensesWindow *expenses_window_;
    PaymentWindow *payments_window_;
};

#endif // WINDOWMANAGER_H
