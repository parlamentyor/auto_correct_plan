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
    void onAddWorkInContract(std::shared_ptr<app::App> app,
                             std::optional<std::vector<model::SeparateWork>>& pool_work);
    void onAddWorkInStage(std::optional<std::vector<model::SeparateWork>>& pool_work);
    void onUpdateTableWorkInStage();
    void onAppAboutToQuit();
    void onAddStageInContract(std::optional<std::vector<model::Stage>>& pool_stage);
    void onUpdateTable();
    void onAddExpensesInContract(std::shared_ptr<app::App> app,
                                 std::optional<std::vector<model::Expenses>>& expenses);
    void onAddExpensesInStage(std::optional<std::vector<model::Expenses>>& expenses);
    void onEditPayments(std::optional<std::vector<model::Payment>> &payments);
    void onEditWork(std::optional<std::vector<model::SeparateWork>>& pool_work,
                    int pos);

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
