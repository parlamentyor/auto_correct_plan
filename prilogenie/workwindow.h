#ifndef WORKWINDOW_H
#define WORKWINDOW_H

#include <QMainWindow>

#include <memory>

#include "app.h"

namespace Ui {
class WorkWindow;
}

class WorkWindow : public QMainWindow
{
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

private:
    Ui::WorkWindow *ui;
    std::shared_ptr<app::App> app_;
};

#endif // WORKWINDOW_H
