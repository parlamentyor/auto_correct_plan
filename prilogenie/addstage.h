#ifndef ADDSTAGE_H
#define ADDSTAGE_H

#include <QMainWindow>
#include <QTableWidget>
#include <QLineEdit>

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
                      QWidget *parent = nullptr);
    explicit AddStage(std::shared_ptr<app::App> app,
                      std::shared_ptr<model::Stage> edit_stage_,
                      QWidget *parent = nullptr);
    ~AddStage();

signals:
    void UpdateTable();
    void AddWorkInStage();
    void AddExpensesInStage(std::optional<std::vector<model::Expenses>>& expenses);
    void EditPaymentsInStage(std::optional<std::vector<model::Payment>>& payments);
    void EditWork(std::shared_ptr<model::SeparateWork> edit_work);
    void AddNewStage(std::shared_ptr<model::Stage> new_stage);

public slots:
    void toUpdateTableWorkInStage();
    void toAddNewWork(std::shared_ptr<model::SeparateWork> new_work);

private slots:
    void on_pb_add_work_att_as_clicked();
    void on_pb_add_stage_clicked();
    void on_cb_with_deadline_data_stateChanged(int arg1);
    void on_pb_add_work_clicked();
    void on_table_work_cellChanged(int row, int column);

    void on_pb_edit_deadline_data_clicked();
    void on_de_deadline_data_dateChanged(const QDate &date);
    void on_pb_expenses_clicked();
    void on_pb_payments_clicked();
    void on_ActionDelete(int index);
    void on_ActionComplet(int index);
    void on_ActionActual(int index);

    void ShowContextMenu(const QPoint& pos);

    void on_pb_correct_clicked();

private:
    Ui::AddStage *ui;
    std::shared_ptr<model::Stage> stage_;
    std::optional<model::Date> date_;
    std::shared_ptr<app::App> app_;

    void SetTableProperties(QTableWidget* table);
    void UpdateTableWorkInStage();
    QColor GetColor(const std::shared_ptr<model::SeparateWork>& work);
    void SetCompleter(QLineEdit *le, const std::set<std::string>& base);
    void SetRowBackgroundColor(int row, const QColor& color);
};

#endif // ADDSTAGE_H
