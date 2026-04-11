#ifndef SERIALIZATION_QT_JSON_H
#define SERIALIZATION_QT_JSON_H

/*
class serialization_Qt_json
{
public:
    serialization_Qt_json();
};
*/

#include "model.h"
#include "app.h"

#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonArray>
#include <QFile>
#include <QDebug>

namespace serialization {

        // Свободные функции для сериализации Date
    QJsonObject serializeDate(const model::Date& date);
    model::Date deserializeDate(const QJsonObject& obj);

        // Свободные функции для сериализации Price
    QJsonObject serializePrice(const model::Price& price);
    model::Price deserializePrice(const QJsonObject& obj);

        // Свободные функции для сериализации TypeContract
    QString serializeTypeContract(model::TypeContract type);
    model::TypeContract deserializeTypeContract(const QString& str);

        // Свободные функции для сериализации SeparateWork
    QJsonObject serializeSeparateWork(const model::SeparateWork& work);
    model::SeparateWork deserializeSeparateWork(const QJsonObject& obj);

        // Свободные функции для сериализации Contract
    QJsonObject serializeContract(const model::Contract& contract);
    model::Contract deserializeContract(const QJsonObject& obj);

        // Свободные функции для сериализации User
    QJsonObject serializeUser(const model::User& user);
    model::User deserializeUser(const QJsonObject& obj);

    bool SaveToJsonFile(const QString& filename, std::shared_ptr<app::App> app);
    bool LoadFromJsonFile(const QString& filename, std::shared_ptr<app::App> app);

}   // namespace serialization


#endif // SERIALIZATION_QT_JSON_H
