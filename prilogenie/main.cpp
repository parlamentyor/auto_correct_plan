#include "authorization.h"
#include "workwindow.h"
#include "app.h"

#include <QApplication>

int main(int argc, char *argv[])
{
//    std::locale::global(std::locale("ru_RU.UTF-8"));
    std::shared_ptr<app::App> app = std::make_shared<app::App>();

    QApplication a(argc, argv);

    // потом возможно запилить менеджер окон

    Authorization authoriz(app);
    authoriz.show();

    // Создаем указатель на главное окно (пока nullptr)
    WorkWindow *work_window = nullptr;

    // Подключаем сигнал успешного входа к созданию главного окна
    QObject::connect(&authoriz, &Authorization::loginSuccess,
                    [&work_window, app]() {
                        work_window = new WorkWindow(app);
                        work_window->show();
                    });

    return a.exec();
}
