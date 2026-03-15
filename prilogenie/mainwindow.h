#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>

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

private slots:
    void on_pb_odt_clicked();

    void on_pb_pdf_clicked();

    void on_pb_docx_clicked();

    void on_pb_add_work_att_as_clicked();

    void on_pb_add_contract_clicked();

private:
    Ui::MainWindow *ui;
    std::shared_ptr<app::App> app_;
    std::vector<model::SeparateWork> pool_work_;
};
#endif // MAINWINDOW_H
