#include "app.h"
#include "general_functions.h"
#include "serialization_qt_json.h"
#include "windowmanager.h"
#include "my_logger.h"

#include <QApplication>

int main(int argc, char *argv[]) {

//    std::locale::global(std::locale("ru_RU.UTF-8"));

    std::shared_ptr<app::App> app = std::make_shared<app::App>();

    // Загружаем состояние при запуске
    QString state_file = QString::fromStdString(details::CreatePathDokument("state", "app_state.json"));
    if (serialization::LoadFromJsonFile(state_file, app)) {
        LOG("Состояние загружено успешно из ", state_file.toStdString());
    }
    else {
        LOG("Ошибка загрузки или нет данных в файле состояния ", state_file.toStdString());
    }

    QApplication a(argc, argv);

    WindowManager manager(app);
    manager.Start();

    return a.exec();
}
