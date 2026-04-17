#include "addstage.h"
#include "ui_addstage.h"

AddStage::AddStage(std::vector<model::Stage> &pool_stage, QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::AddStage)
    , pool_stage_(pool_stage) {
    ui->setupUi(this);
}

AddStage::~AddStage()
{
    delete ui;
}
