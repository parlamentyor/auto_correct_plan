#ifndef ADDSTAGE_H
#define ADDSTAGE_H

#include <QMainWindow>
#include <vector>

#include <model.h>

namespace Ui {
class AddStage;
}

class AddStage : public QMainWindow
{
    Q_OBJECT

public:
    explicit AddStage(std::vector<model::Stage> &pool_stage, QWidget *parent = nullptr);
    ~AddStage();

private:
    Ui::AddStage *ui;
    std::vector<model::Stage> &pool_stage_;
};

#endif // ADDSTAGE_H
