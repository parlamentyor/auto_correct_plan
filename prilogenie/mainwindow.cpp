#include "mainwindow.h"
#include "ui_mainwindow.h"

#include "handler_odt.h"
#include "handler_pdf.h"
#include "handler_docx.h"

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);
}

MainWindow::~MainWindow()
{
    delete ui;
}


void MainWindow::on_pb_odt_clicked()
{
    CreateOdtWithTable();
}


void MainWindow::on_pb_pdf_clicked()
{
    CreatePdfReport();
}


void MainWindow::on_pb_docx_clicked()
{
    CreateDocxWithWord();
}

