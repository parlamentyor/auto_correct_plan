#ifndef ADDSTAGE_H
#define ADDSTAGE_H

#include <QMainWindow>
#include <QTableWidget>

#include <vector>

#include <model.h>
#include <app.h>

namespace Ui {
class AddStage;
}

class AddStage : public QMainWindow
{
    Q_OBJECT

public:
    explicit AddStage(std::shared_ptr<app::App> app,
                      std::optional<std::vector<model::Stage>> &pool_stage, QWidget *parent = nullptr);
    ~AddStage();

signals:
    void UpdateTable();
    void AddWorkInStage(std::optional<std::vector<model::SeparateWork>>& pool_work);
    void AddExpensesInStage(std::optional<std::vector<model::Expenses>>& expenses);
    void EditPaymentsInStage(std::optional<std::vector<model::Payment>>& payments);

public slots:
    void toUpdateTableWorkInStage();

private slots:
    void on_pb_add_work_att_as_clicked();
    void on_pb_add_stage_clicked();
    void on_cb_with_deadline_data_stateChanged(int arg1);
    void on_cb_correct_number_stateChanged(int arg1);
    void on_pb_add_work_clicked();
    void on_table_work_cellChanged(int row, int column);

    void on_pb_edit_deadline_data_clicked();
    void on_de_deadline_data_dateChanged(const QDate &date);

    void on_pb_expenses_clicked();

    void on_pb_payments_clicked();

private:
    Ui::AddStage *ui;
    std::optional<std::vector<model::Stage>>& pool_stage_;
    std::optional<std::vector<model::SeparateWork>> pool_work_;
    std::optional<model::Date> date_;
    std::shared_ptr<app::App> app_;
    std::optional<std::vector<model::Expenses>> expenses_;
    std::optional<std::vector<model::Payment>> payments_;

    void SetTableProperties(QTableWidget* table);
    void UpdateTableWorkInStage();
};

#endif // ADDSTAGE_H
