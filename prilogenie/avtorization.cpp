#include "avtorization.h"
#include "ui_avtorization.h"

avtorization::avtorization(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::avtorization)
{
    ui->setupUi(this);
}

avtorization::~avtorization()
{
    delete ui;
}
