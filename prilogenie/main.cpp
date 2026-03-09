#include "mainwindow.h"
#include "authorization.h"
#include "app.h"

#include <QApplication>

int main(int argc, char *argv[])
{
//    app::App app;
    std::shared_ptr<app::App> app = std::make_shared<app::App>();

    QApplication a(argc, argv);
    Authorization authoriz(app);
    authoriz.show();

    MainWindow w(app);
    w.show();
    return a.exec();
}
