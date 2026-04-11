#include "windowmanager.h"
#include "my_logger.h"
#include "serialization_qt_json.h"

#include "QCoreApplication"
#include <QStandardPaths>
#include <QDir>

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

    //    QString configPath = QStandardPaths::writableLocation(QStandardPaths::AppLocalDataLocation);
    //    QDir().mkpath(configPath); // Создаем директорию, если её нет
    //    QString stateFile = configPath + "/app_state.json";
    QString stateFile ="app_state.json";

    if (serialization::SaveToJsonFile(stateFile, app_)) {
        LOG("State saved successfully on application exit. Dir:", stateFile.toStdString());
    } else {
        LOG("ERROR: Failed to save state on application exit");
    }
}

