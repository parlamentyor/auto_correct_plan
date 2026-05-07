#ifndef ADDWORKWINDOW_H
#define ADDWORKWINDOW_H

#include <QMainWindow>

#include "app.h"

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

private:
    Ui::AddWorkWindow *ui;

    std::shared_ptr<app::App> app_;
    std::optional<std::vector<model::SeparateWork>>& pool_work_;
};

#endif // ADDWORKWINDOW_H
