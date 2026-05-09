#ifndef ADDWORKWINDOW_H
#define ADDWORKWINDOW_H

#include <QMainWindow>
#include <QCompleter>
#include <QLineEdit>

#include "app.h"
#include "model.h"

namespace Ui {
class AddWorkWindow;
}

class AddWorkWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit AddWorkWindow(std::shared_ptr<app::App> app,
                           std::optional<std::vector<model::SeparateWork>>& pool_work,
                           QWidget *parent = nullptr);
    ~AddWorkWindow();

signals:
    void UpdateTable();

private slots:
    void on_pb_add_work_clicked();
    void on_pb_add_employee_1_clicked();
    void on_pb_add_employee_2_clicked();
    void on_pb_add_employee_3_clicked();
    void on_pb_add_employee_4_clicked();
    void on_pb_add_employee_5_clicked();

    void on_pb_edit_deadline_data_clicked();
    void on_de_deadline_data_dateChanged(const QDate &date);

private:
    Ui::AddWorkWindow *ui;

    std::shared_ptr<app::App> app_;
    std::optional<std::vector<model::SeparateWork>>& pool_work_;
    model::Date date_;

    void SetCompleter(QLineEdit *le, const std::set<std::string>& base);
    void InitializeDateDisplay();
};

#endif // ADDWORKWINDOW_H
