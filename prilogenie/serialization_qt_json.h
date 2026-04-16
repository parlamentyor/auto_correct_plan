#ifndef SERIALIZATION_QT_JSON_H
#define SERIALIZATION_QT_JSON_H

#include "model.h"
#include "app.h"

#include <QJsonObject>

namespace serialization {

        // Свободные функции для сериализации Date
    QJsonObject SerializeDate(const model::Date& date);
    model::Date DeserializeDate(const QJsonObject& obj);

        // Свободные функции для сериализации Price
    QJsonObject SerializePrice(const model::Price& price);
    model::Price DeserializePrice(const QJsonObject& obj);

        // Свободные функции для сериализации TypeContract
    QString SerializeTypeContract(model::TypeContract type);
    model::TypeContract DeserializeTypeContract(const QString& str);

        // Свободные функции для сериализации SeparateWork
    QJsonObject SerializeSeparateWork(const model::SeparateWork& work);
    model::SeparateWork DeserializeSeparateWork(const QJsonObject& obj);

    QJsonObject SerializePayment(const model::Payment& payment);
    model::Payment DeserializePayment(const QJsonObject& obj);

    QJsonObject SerializeExpenses(const model::Expenses& expenses);
    model::Expenses DeserializeExpenses(const QJsonObject& obj);

    QJsonObject SerializeStage(const model::Stage& stage);
    model::Stage DeserializeStage(const QJsonObject& obj);

        // Свободные функции для сериализации Contract
    QJsonObject DerializeContract(const model::Contract& contract);
    model::Contract DeserializeContract(const QJsonObject& obj);

        // Свободные функции для сериализации User
    QJsonObject SerializeUser(const model::User& user);
    model::User DeserializeUser(const QJsonObject& obj);

    bool SaveToJsonFile(const QString& filename, std::shared_ptr<app::App> app);
    bool LoadFromJsonFile(const QString& filename, std::shared_ptr<app::App> app);

}   // namespace serialization


#endif // SERIALIZATION_QT_JSON_H
