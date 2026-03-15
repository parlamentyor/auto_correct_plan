#include "windowmanager.h"

WindowManager::WindowManager(std::shared_ptr<app::App> app, QObject *parent)
    : QObject(parent)
    , app_(app)
    , authoriz_(nullptr)
    , work_window_(nullptr)
    , main_window_(nullptr)
{
}

void WindowManager::Start()
{
    authoriz_ = new Authorization(app_);
    connect(authoriz_, &Authorization::loginSuccess, this, &WindowManager::onLoginSuccess);
    authoriz_->show();
}

void WindowManager::onLoginSuccess()
{
    work_window_ = new WorkWindow(app_);
    connect(work_window_, &WorkWindow::AddContract, this, &WindowManager::onAddContract);
    work_window_->show();
    authoriz_->close();
}

void WindowManager::onAddContract()
{
    if (!main_window_) {
        main_window_ = new MainWindow(app_);
    }
    main_window_->show();
}
