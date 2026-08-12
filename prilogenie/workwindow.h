#ifndef WORKWINDOW_H
#define WORKWINDOW_H

#include <QMainWindow>
#include <QPoint>

#include <memory>

#include "app.h"
#include "abstractcontractmodel.h"
#include "contractmodel.h"
#include "contractview.h"
#include "filterwidget.h"

namespace Ui {
class WorkWindow;
}

class WorkWindow : public QMainWindow {
    Q_OBJECT

public:
    explicit WorkWindow(std::shared_ptr<app::App> app, QWidget *parent = nullptr);
    ~WorkWindow();

signals:
    void AddContract();
    void EditContract(std::shared_ptr<model::Contract> contract);

public slots:
    void onUpdatTechnicalMainTable();

private slots:
    void on_pb_open_plan_month_clicked();
    void on_pb_create_plan_month_clicked();
    void on_pb_add_new_contract_clicked();
    void on_pb_create_plan_month_default_clicked();


    // Новые слоты для переключения представлений
    void on_pb_show_contracts_clicked();
    void on_pb_show_stages_clicked();
    void on_pb_show_works_clicked();


    // для контекстного меню
    void onTableViewCustomContextMenuRequested(const QPoint& pos);
    void onEditContractAction();
    void onDeleteContractAction();
    void onMarkAsCompletedAction();

    void on_pb_add_employee_clicked();
    void on_pb_delete_employee_clicked();

    void on_lw_employees_currentRowChanged(int currentRow);

    // Новые слоты для фильтров
//    void onFilterCompletedChanged(bool checked);
//    void onFilterPaidChanged(bool checked);
//    void onFilterTypeChanged(model::TypeContract type, bool checked);
//    void onClearFiltersClicked();

private:

    // Типы представлений
    enum class ViewType {
        Contracts,  // Только договоры (с группировкой)
        Stages,     // Только этапы (плоский список)
        Works       // Только работы (плоский список)
    };

    Ui::WorkWindow *ui;
    std::shared_ptr<app::App> app_;

    QTableView* tableView_;
    AbstractContractModel* model_;
    int currentContextMenuRow_;

    FilterWidget* filterWidget_;

    std::set<std::string>::iterator it_base_employees_ = app_->GetBaseEmployee().begin();
    std::optional<int> pos_employee_;

//    void setupContractsTable();    помоему рудимент - удалить
    void AddMainTable();
    // Общий метод для очистки layout
    void DeleteWidgetInLayout();




    // Текущий тип представления
    ViewType currentViewType_;

    // Метод для переключения представления
    void switchToView(ViewType type);

    // Методы для создания разных представлений
    void setupContractsView();
    void setupStagesView();
    void setupWorksView();




    int getCurrentContractIndex() const;
    std::shared_ptr<model::Contract> getCurrentContract() const;

    void UpdateTableEmployees();
    void ApplyIterator();

    // Метод обновления фильтров
    void applyFilters();
};

#endif // WORKWINDOW_H
