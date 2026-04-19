#ifndef WINDOWMANAGER_H
#define WINDOWMANAGER_H

#include <QObject>
#include <memory>

#include "app.h"
#include "authorization.h"
#include "addstage.h"
#include "workwindow.h"
#include "mainwindow.h"

class WindowManager : public QObject
{
    Q_OBJECT
public:
    explicit WindowManager(std::shared_ptr<app::App> app, QObject *parent = nullptr);

    void Start();

private slots:
    void onLoginSuccess();
    void onAddContract();
    void onAppAboutToQuit();
    void onAddStageInContract(std::optional<std::vector<model::Stage>>& pool_stage);
    void onUpdateTable();

private:
    std::shared_ptr<app::App> app_;
    Authorization *authoriz_;
    WorkWindow *work_window_;
    MainWindow *main_window_;
    AddStage *add_stage_;
};

#endif // WINDOWMANAGER_H
