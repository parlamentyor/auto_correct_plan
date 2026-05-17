#ifndef PAYMENTWINDOW_H
#define PAYMENTWINDOW_H

#include <QMainWindow>

#include <model.h>

namespace Ui {
class PaymentWindow;
}

class PaymentWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit PaymentWindow(std::optional<std::vector<model::Payment>>& payments,
                           QWidget *parent = nullptr);
    ~PaymentWindow();

private slots:
    void on_de_add_data_dateChanged(const QDate &date);
    void on_pb_edit_add_data_clicked();
    void on_tw_payment_currentCellChanged(int currentRow, int currentColumn, int previousRow, int previousColumn);
    void on_pb_add_payment_clicked();

    void on_pb_delete_payment_clicked();

private:
    Ui::PaymentWindow *ui;

    std::optional<std::vector<model::Payment>>& payments_;
    model::Date date_;

    void SetTableProperties();
    void UpdateTable();
    QString FormatDate(const model::Date& date) const;
    int current_row_ = -1;
};

#endif // PAYMENTWINDOW_H
