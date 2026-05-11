#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QTableWidget>
#include <QDateEdit>

#include "app.h"

QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(std::shared_ptr<app::App> app, QWidget *parent = nullptr);
    ~MainWindow();

signals:
    void AddStageInContract(std::optional<std::vector<model::Stage>>& pool_stage);
    void AddWorkInContract(std::shared_ptr<app::App> app,
                           std::optional<std::vector<model::SeparateWork>>& pool_work);
    void AddExpensesInContract(std::shared_ptr<app::App> app,
                               std::optional<std::vector<model::Expenses>>& expenses);

public slots:
    void toUpdateTable();

private slots:
    void on_pb_odt_clicked();
    void on_pb_pdf_clicked();
    void on_pb_add_work_att_as_clicked();
    void on_pb_add_contract_clicked();
    void on_chb_stage_stateChanged(int arg1);
    void on_pb_add_stage_clicked();
    void on_cb_with_date_stateChanged(int arg1);
    void on_cb_with_deadline_data_stateChanged(int arg1);
    void on_pb_add_work_clicked();
    void on_table_work_cellChanged(int row, int column);

    void on_pb_edit_contract_data_clicked();
    void on_de_contract_data_dateChanged(const QDate &date);

    void on_pb_edit_deadline_data_clicked();
    void on_de_deadline_data_dateChanged(const QDate &date);

    void on_pb_expenses_clicked();

private:
    Ui::MainWindow *ui;
    std::shared_ptr<app::App> app_;
    std::optional<std::vector<model::SeparateWork>> pool_work_;
    std::optional<std::vector<model::Stage>> pool_stage_;
    std::optional<model::Date> contract_date_;
    std::optional<model::Date> deadline_date_;
    std::optional<std::vector<model::Expenses>> expenses_;

    void SetTableProperties(QTableWidget* table);
    void UpdateTable();
    void UpdateDate(std::optional<model::Date>& date, QDateEdit *de);
};
#endif // MAINWINDOW_H
