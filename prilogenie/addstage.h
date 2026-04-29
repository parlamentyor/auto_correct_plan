#ifndef ADDSTAGE_H
#define ADDSTAGE_H

#include <QMainWindow>
#include <vector>

#include <model.h>

namespace Ui {
class AddStage;
}

class AddStage : public QMainWindow
{
    Q_OBJECT

public:
    explicit AddStage(std::optional<std::vector<model::Stage>> &pool_stage, QWidget *parent = nullptr);
    ~AddStage();

signals:
    void UpdateTable();

private slots:
    void on_pb_add_work_att_as_clicked();
    void on_pb_add_stage_clicked();
    void on_cb_with_deadline_data_stateChanged(int arg1);
    void on_cb_correct_number_stateChanged(int arg1);
    void on_pb_add_work_clicked();
    void on_table_work_cellChanged(int row, int column);

private:
    Ui::AddStage *ui;
    std::optional<std::vector<model::Stage>>& pool_stage_;
    std::optional<std::vector<model::SeparateWork>> pool_work_;
};

#endif // ADDSTAGE_H
