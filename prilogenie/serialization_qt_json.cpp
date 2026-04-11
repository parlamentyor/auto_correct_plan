#include "serialization_qt_json.h"
#include "my_logger.h"

#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonArray>
#include <QFile>
#include <QDebug>

namespace serialization {

    // Свободные функции для сериализации Date
    QJsonObject SerializeDate(const model::Date& date) {
        QJsonObject obj;
        obj["day"] = date.day_;
        obj["month"] = date.month_;
        obj["year"] = date.year_;
        return obj;
    }


    model::Date DeserializeDate(const QJsonObject& obj) {
        model::Date date;
        date.day_ = obj["day"].toInt();
        date.month_ = obj["month"].toInt();
        date.year_ = obj["year"].toInt();
        return date;
    }

    // Свободные функции для сериализации Price
    QJsonObject SerializePrice(const model::Price& price) {
        QJsonObject obj;
        obj["ruble"] = price.ruble_;
        obj["kop"] = price.kop_;
        return obj;
    }

    model::Price DeserializePrice(const QJsonObject& obj) {
        model::Price price;
        price.ruble_ = obj["ruble"].toInt();
        price.kop_ = obj["kop"].toInt();
        return price;
    }

    // Свободные функции для сериализации TypeContract
    QString SerializeTypeContract(model::TypeContract type) {
        switch(type) {
        case model::GOZ: return "GOZ";
        case model::ATT: return "ATT";
        case model::BEK: return "BEK";
        case model::SI: return "SI";
        default: return "GOZ";
        }
    }

    model::TypeContract DeserializeTypeContract(const QString& str) {
        if (str == "GOZ") return model::GOZ;
        if (str == "ATT") return model::ATT;
        if (str == "BEK") return model::BEK;
        if (str == "SI") return model::SI;
        return model::GOZ;
    }

    // Свободные функции для сериализации SeparateWork
    QJsonObject SerializeSeparateWork(const model::SeparateWork& work) {
        QJsonObject obj;
        obj["name"] = QString::fromStdString(work.name_);

        QJsonArray responsible_array;
        for (const auto& employee : work.names_responsible_employees_) {
            responsible_array.append(QString::fromStdString(employee));
        }
        obj["names_responsible_employees"] = responsible_array;
        obj["date_deadline"] = SerializeDate(work.date_deadline_);

        return obj;
    }

    model::SeparateWork DeserializeSeparateWork(const QJsonObject& obj) {
        model::SeparateWork work;
        work.name_ = obj["name"].toString().toStdString();

        QJsonArray responsible_array = obj["names_responsible_employees"].toArray();
        for (const auto& item : responsible_array) {
            work.names_responsible_employees_.push_back(item.toString().toStdString());
        }

        work.date_deadline_ = DeserializeDate(obj["date_deadline"].toObject());

        return work;
    }

    // Свободные функции для сериализации Contract
    QJsonObject SerializeContract(const model::Contract& contract) {
        QJsonObject obj;

        // optional поля
        if (contract.number_.has_value()) {
            obj["number"] = QString::fromStdString(contract.number_.value());
        }

        obj["date"] = SerializeDate(contract.date_);

        if (contract.name_organization_.has_value()) {
            obj["name_organization"] = QString::fromStdString(contract.name_organization_.value());
        }

        if (contract.name_short_.has_value()) {
            obj["name_short"] = QString::fromStdString(contract.name_short_.value());
        }

        if (contract.name_full_.has_value()) {
            obj["name_full"] = QString::fromStdString(contract.name_full_.value());
        }

        obj["date_deadline"] = SerializeDate(contract.date_deadline_);

        if (contract.name_responsible_employee_.has_value()) {
            obj["name_responsible_employee"] = QString::fromStdString(contract.name_responsible_employee_.value());
        }

        obj["price"] = SerializePrice(contract.price_);
        obj["price_other_department"] = SerializePrice(contract.price_other_department_);
        obj["with_nds"] = contract.with_nds_;
        obj["stavka_nds"] = contract.stavka_nds_;
        obj["type"] = SerializeTypeContract(contract.type_);
        obj["with_stage"] = contract.with_stage_;

        // Сохраняем вектор SeparateWork
        QJsonArray pool_array;
        for (const auto& work : contract.pool_work) {
            pool_array.append(SerializeSeparateWork(work));
        }
        obj["pool_work"] = pool_array;


        // вот тут есть вопросики - у меня static int id_counter_, которая ++ при создании объекта......подумать как себя будет вести при серриализации
        obj["id"] = contract.id_;

        return obj;
    }

    model::Contract DeserializeContract(const QJsonObject& obj) {
        model::Contract contract;

        if (obj.contains("number") && !obj["number"].isNull()) {
            contract.number_ = obj["number"].toString().toStdString();
        }

        contract.date_ = DeserializeDate(obj["date"].toObject());

        if (obj.contains("name_organization") && !obj["name_organization"].isNull()) {
            contract.name_organization_ = obj["name_organization"].toString().toStdString();
        }

        if (obj.contains("name_short") && !obj["name_short"].isNull()) {
            contract.name_short_ = obj["name_short"].toString().toStdString();
        }

        if (obj.contains("name_full") && !obj["name_full"].isNull()) {
            contract.name_full_ = obj["name_full"].toString().toStdString();
        }

        contract.date_deadline_ = DeserializeDate(obj["date_deadline"].toObject());

        if (obj.contains("name_responsible_employee") && !obj["name_responsible_employee"].isNull()) {
            contract.name_responsible_employee_ = obj["name_responsible_employee"].toString().toStdString();
        }

        contract.price_ = DeserializePrice(obj["price"].toObject());
        contract.price_other_department_ = DeserializePrice(obj["price_other_department"].toObject());
        contract.with_nds_ = obj["with_nds"].toBool();
        contract.stavka_nds_ = obj["stavka_nds"].toInt();
        contract.type_ = DeserializeTypeContract(obj["type"].toString());
        contract.with_stage_ = obj["with_stage"].toBool();

        QJsonArray pool_array = obj["pool_work"].toArray();
        for (const auto& item : pool_array) {
            contract.pool_work.push_back(DeserializeSeparateWork(item.toObject()));
        }

        contract.id_ = obj["id"].toInt();

        // Обновляем статический счетчик (придется сделать дружественную функцию или публичный метод)
        // Временное решение - через указатель на статическую переменную
        // Лучше добавить публичный статический метод в Contract, но это потребует изменения класса

        return contract;
    }

    // Свободные функции для сериализации User
    QJsonObject SerializeUser(const model::User& user) {
        QJsonObject obj;
        obj["id"] = user.GetId();
        obj["login"] = QString::fromStdString(user.GetLogin());
        obj["pass"] = QString::fromStdString(user.GetPass());
        return obj;
    }

    model::User DeserializeUser(const QJsonObject& obj) {
        int id = obj["id"].toInt();
        std::string login = obj["login"].toString().toStdString();
        std::string pass = obj["pass"].toString().toStdString();
        return model::User(id, login, pass);
    }

    bool SaveToJsonFile(const QString& filename, std::shared_ptr<app::App> app) {
        QJsonObject root_object;

        // Сохраняем users_ - используем ключ из мапы
        QJsonObject users_object;
        for (const auto& [login, user] : app->GetUsers()) {
            QJsonObject user_obj;
            user_obj["id"] = user.GetId();
            user_obj["pass"] = QString::fromStdString(user.GetPass());
            // Логин не сохраняем, так как он является ключом
            users_object[QString::fromStdString(login)] = user_obj;
        }
        root_object["users"] = users_object;

        // Сохраняем contracts_
        QJsonArray contracts_array;
        for (const auto& contract : app->GetContracts()) {
            contracts_array.append(SerializeContract(contract));
        }
        root_object["contracts"] = contracts_array;

        // Сохраняем дополнительные данные
        if (app->HasValuePathPlanMonth()) {
            root_object["path_plan_month"] = QString::fromStdString(app->GetPathPlanMonth());
        }

        QFile file(filename);
        if (!file.open(QIODevice::WriteOnly)) {
            return false;
        }

        QJsonDocument doc(root_object);
        file.write(doc.toJson(QJsonDocument::Indented));
        return true;
    }

    bool LoadFromJsonFile(const QString& filename, std::shared_ptr<app::App> app) {
        QFile file(filename);
        if (!file.exists() || !file.open(QIODevice::ReadOnly)) {
            LOG("С файлом что-то не так");
            return false;
        }

        QByteArray data = file.readAll();
        QJsonDocument doc = QJsonDocument::fromJson(data);
        if (doc.isNull()) {
            LOG("Не удалось считать содержимое файла");
            return false;
        }

        QJsonObject root_object = doc.object();

        // Загружаем users_
        if (root_object.contains("users")) {
            QJsonObject users_object = root_object["users"].toObject();
            //            users_.clear();
            for (auto it = users_object.begin(); it != users_object.end(); ++it) {
                std::string login = it.key().toStdString();
                QJsonObject userObj = it.value().toObject();
                std::string pass = userObj["pass"].toString().toStdString();

                // Создаем пользователя - он получит новый id
                // Это проблема: старый id теряется
                app->AddUser(login, pass);
            }
        }

        // Загружаем contracts_
        if (root_object.contains("contracts")) {
            QJsonArray contracts_array = root_object["contracts"].toArray();
            //            contracts_.clear();
            for (const auto& item : contracts_array) {
                app->AddContract(DeserializeContract(item.toObject()));
            }
        }

        if (root_object.contains("path_plan_month")) {
            app->SetPathPlanMonth(root_object["path_plan_month"].toString().toStdString());
        }

        return true;
    }

}   // namespace serialization
