#ifndef WINDOWMANAGER_H
#define WINDOWMANAGER_H

#include <QObject>
#include <memory>

#include "app.h"
#include "authorization.h"
#include "workwindow.h"
#include "mainwindow.h"

//namespace app {
//class App;
//}

class WindowManager : public QObject
{
    Q_OBJECT
public:
    explicit WindowManager(std::shared_ptr<app::App> app, QObject *parent = nullptr);

    void Start();

private slots:
    void onLoginSuccess();
    void onAddContract();

private:
    std::shared_ptr<app::App> app_;
    Authorization *authoriz_;
    WorkWindow *work_window_;
    MainWindow *main_window_;
};



#endif // WINDOWMANAGER_H
