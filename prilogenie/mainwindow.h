#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QTableWidget>

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


private:
    Ui::MainWindow *ui;
    std::shared_ptr<app::App> app_;
    std::optional<std::vector<model::SeparateWork>> pool_work_;
    std::optional<std::vector<model::Stage>> pool_stage_;

    void SetTableProperties(QTableWidget* table);
    void UpdateTable();
};
#endif // MAINWINDOW_H
