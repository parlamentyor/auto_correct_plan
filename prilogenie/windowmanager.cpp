#include "windowmanager.h"
#include "my_logger.h"
#include "serialization_qt_json.h"
#include "general_functions.h"

#include "QCoreApplication"
//#include <QStandardPaths>
//#include <QDir>

WindowManager::WindowManager(std::shared_ptr<app::App> app, QObject *parent)
    : QObject(parent)
    , app_(app)
    , authoriz_(nullptr)
    , work_window_(nullptr)
    , main_window_(nullptr)
{
    connect(qApp, &QCoreApplication::aboutToQuit, this, &WindowManager::onAppAboutToQuit);
}

void WindowManager::Start()
{
    authoriz_ = new Authorization(app_);
    connect(authoriz_, &Authorization::loginSuccess, this, &WindowManager::onLoginSuccess);
    authoriz_->show();
    LOG("Запустилось окно авторизации");

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

void WindowManager::onAppAboutToQuit()
{
    LOG("Application is about to quit, saving state...");
    QString stateFile = QString::fromStdString(details::CreatePathDokument("state", "app_state.json"));
    if (serialization::SaveToJsonFile(stateFile, app_)) {
        LOG("При закрытии программы состояние сохранено успешно по адресу:", stateFile.toStdString());
    }
    else {
        LOG("ОШИБКА: При закрытии программы состояние не сохранилось");
    }
}

