#ifndef WORKWINDOW_H
#define WORKWINDOW_H

#include <QMainWindow>
#include <QPoint>

#include <memory>

#include "app.h"
#include "contractmodel.h"
#include "contractview.h"

namespace Ui {
class WorkWindow;
}

class WorkWindow : public QMainWindow {
    Q_OBJECT

public:
    explicit WorkWindow(std::shared_ptr<app::App> app, QWidget *parent = nullptr);
    ~WorkWindow();

signals:
    void AddContract();

private slots:
    void on_pb_open_plan_month_clicked();

    void on_pb_create_plan_month_clicked();

    void on_pb_add_new_contract_clicked();

    void on_pb_create_plan_month_default_clicked();

    void on_pushButton_clicked();

    // для контекстного меню
    void onTableViewCustomContextMenuRequested(const QPoint& pos);
    void onDeleteContractAction();
    void onMarkAsCompletedAction();

private:
    Ui::WorkWindow *ui;
    std::shared_ptr<app::App> app_;

    ContractView* tableView_;
    ContractModel* model_;
    int currentContextMenuRow_;

    void setupContractsTable();
    void AddMainTable();
    void DeleteWidgetInLayout();

    int getCurrentContractIndex() const;
};

#endif // WORKWINDOW_H
