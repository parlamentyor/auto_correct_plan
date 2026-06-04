#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QTableWidget>
#include <QDateEdit>
#include <QLineEdit>

#include "app.h"

QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(std::shared_ptr<app::App> app,
               QWidget *parent = nullptr);
    MainWindow(std::shared_ptr<app::App> app,
               std::shared_ptr<model::Contract> contract,
               QWidget *parent = nullptr);
    ~MainWindow();

signals:
    void AddStageInContract();
    void AddWorkInContract(std::shared_ptr<app::App> app);
    void AddExpensesInContract(std::shared_ptr<app::App> app,
                               std::optional<std::vector<model::Expenses>>& expenses);
    void EditPaymentsInContract(std::optional<std::vector<model::Payment>>& payments);
    void EditWork(std::shared_ptr<model::SeparateWork> edit_work);
    void EditStage(std::shared_ptr<model::Stage> edit_stage);

public slots:
    void toUpdateTable();
    void toAddNewWork(std::shared_ptr<model::SeparateWork> new_work);
    void toAddNewStage(std::shared_ptr<model::Stage> new_stage);

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
    void on_pb_payments_clicked();

    void on_chb_nds_stateChanged(int arg1);

    void on_ActionDeleteWork(std::optional<std::vector<std::shared_ptr<model::SeparateWork>>>& pool_work, int index);
    void on_ActionCompletWork(const std::optional<std::vector<std::shared_ptr<model::SeparateWork>>>& pool_work, int index);
    void on_ActionActualWork(const std::optional<std::vector<std::shared_ptr<model::SeparateWork>>>& pool_work, int index);

    void on_ActionDeleteStage(std::optional<std::vector<std::shared_ptr<model::Stage>>>& pool_stage, int index);
    void on_ActionCompletStage(const std::optional<std::vector<std::shared_ptr<model::Stage>>>& pool_stage, int index);
    void on_ActionActualStage(const std::optional<std::vector<std::shared_ptr<model::Stage>>>& pool_stage, int index);

    void ShowContextMenu(const QPoint &pos);

    void on_pb_correct_clicked();

private:
    Ui::MainWindow *ui;
    std::shared_ptr<app::App> app_;
    std::shared_ptr<model::Contract> contract_;
    std::optional<model::Date> contract_date_;
    std::optional<model::Date> deadline_date_;

    void SetTableProperties(QTableWidget* table);
    void UpdateTable();
    void UpdateDate(std::optional<model::Date>& date, QDateEdit *de);

    model::Price MakePriceContract() const;
    model::Price MakePriceOtherDepartmentContract() const;
    void UpdatePrice();
    void UpdatePriceOtherDepartment();

    void AddWorkInBase();
    void AddExpenseInBase();
    void AddOrganizationInBase();

    void SetCompleter(QLineEdit *le, const std::set<std::string>& base);
    void SetRowBackgroundColor(int row, const QColor& color);
    void ShowContextMenuWork(std::optional<std::vector<std::shared_ptr<model::SeparateWork>>>& pool_work,
                             const QPoint &pos,
                             int work_index);
    void ShowContextMenuStage(std::optional<std::vector<std::shared_ptr<model::Stage>>>& pool_stage,
                              const QPoint &pos,
                              int stage_index);

    QColor GetColorStage(const std::shared_ptr<model::Stage>& stage);
    QColor GetColorWork(const std::shared_ptr<model::SeparateWork>& work);

    // Получить оригинальные данные по виртуальному индексу
    struct ItemInfo {
        enum class Type { HeaderRow, StageRow, WorkRow };
        Type type_;
        int stage_index_;
        int work_index_;
        int virtual_row_;
    };

    ItemInfo GetItemInfo(int virtual_row) const;

    // Структура для хранения информации о виртуальных строках
    struct VirtualRow {
        ItemInfo info_;
    };

    mutable std::vector<VirtualRow> virtual_rows_;
    void BuildVirtualRows() const;

};
#endif // MAINWINDOW_H
