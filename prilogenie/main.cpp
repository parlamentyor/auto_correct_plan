#include "windowmanager.h"
#include "app.h"

#include <QApplication>

int main(int argc, char *argv[])
{
// первыя реализация, при которой окно добавления договора не запускалось, потому что небыло объекта work_window
/*
    std::shared_ptr<app::App> app = std::make_shared<app::App>();

    QApplication a(argc, argv);
    Authorization authoriz(app);
    authoriz.show();

    WorkWindow *work_window = nullptr;
    MainWindow *main_window = nullptr;

    // Подключаем сигнал успешного входа к созданию главного окна
    QObject::connect(&authoriz, &Authorization::loginSuccess,
                    [&work_window, app]() {
                        work_window = new WorkWindow(app);
                        work_window->show();
                    });

    QObject::connect(&work_window, &WorkWindow::AddContract(),
                     [&main_window, app]() {
                         main_window = new MainWindow(app);
                         main_window->show();
                     });

    return a.exec();
*/

//    std::locale::global(std::locale("ru_RU.UTF-8"));
    std::shared_ptr<app::App> app = std::make_shared<app::App>();
    QApplication a(argc, argv);

    WindowManager manager(app);
    manager.Start();

    return a.exec();
}
