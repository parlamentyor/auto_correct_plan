#ifndef EXPENSESWINDOW_H
#define EXPENSESWINDOW_H

#include <QMainWindow>
#include <QTableWidget>
#include <QLineEdit>

#include <set>

#include "app.h"
#include "model.h"

namespace Ui {
class ExpensesWindow;
}

class ExpensesWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit ExpensesWindow(std::shared_ptr<app::App> app,
                            std::optional<std::vector<model::Expenses>>& expenses,
                            QWidget *parent = nullptr);
    ~ExpensesWindow();

private slots:
    void on_pb_add_clicked();
    void on_tw_expenses_currentCellChanged(int currentRow, int currentColumn, int previousRow, int previousColumn);

    void on_pb_delete_clicked();

private:
    Ui::ExpensesWindow *ui;

    std::shared_ptr<app::App> app_;
    std::optional<std::vector<model::Expenses>>& expenses_;

    void SetTableProperties();
    void UpdateTable();
    void SetCompleter(QLineEdit *le, const std::set<std::string>& base);
};

#endif // EXPENSESWINDOW_H
