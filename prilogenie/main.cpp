#include "mainwindow.h"
#include "authorization.h"
#include "app.h"

#include <QApplication>

int main(int argc, char *argv[])
{
//    app::App app;
    std::shared_ptr<app::App> app = std::make_shared<app::App>();

    QApplication a(argc, argv);

    // потом возможно запилить менеджер окон

    Authorization authoriz(app);
    authoriz.show();

    // Создаем указатель на главное окно (пока nullptr)
    MainWindow *mainWindow = nullptr;

    // Подключаем сигнал успешного входа к созданию главного окна
    QObject::connect(&authoriz, &Authorization::loginSuccess,
                    [&mainWindow, app]() {
                        mainWindow = new MainWindow(app);
                        mainWindow->show();
                    });

    return a.exec();
}
