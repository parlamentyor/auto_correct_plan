#ifndef SERIALIZATION_QT_JSON_H
#define SERIALIZATION_QT_JSON_H

#include "model.h"
#include "app.h"

#include <QJsonObject>

namespace serialization {

    QJsonObject SerializeDate(const model::Date& date);
    model::Date DeserializeDate(const QJsonObject& obj);

    QJsonObject SerializePrice(const model::Price& price);
    model::Price DeserializePrice(const QJsonObject& obj);

    QString SerializeTypeContract(model::TypeContract type);
    model::TypeContract DeserializeTypeContract(const QString& str);

    QJsonObject SerializeStatusComplet(const model::StatusComplet& status_complet);
    model::StatusComplet DeserializeStatusComplet(const QJsonObject& obj);

    QJsonObject SerializeSeparateWork(const model::SeparateWork& work);
    model::SeparateWork DeserializeSeparateWork(const QJsonObject& obj);

    QJsonObject SerializePayment(const model::Payment& payment);
    model::Payment DeserializePayment(const QJsonObject& obj);

    QJsonObject SerializeExpenses(const model::Expenses& expenses);
    model::Expenses DeserializeExpenses(const QJsonObject& obj);

    QJsonObject SerializeStage(const model::Stage& stage);
    model::Stage DeserializeStage(const QJsonObject& obj);

    QJsonObject DerializeContract(const model::Contract& contract);
    model::Contract DeserializeContract(const QJsonObject& obj);

    QJsonObject SerializeUser(const model::User& user);
    model::User DeserializeUser(const QJsonObject& obj);

    bool SaveToJsonFile(const QString& filename, std::shared_ptr<app::App> app);
    bool LoadFromJsonFile(const QString& filename, std::shared_ptr<app::App> app);

}   // namespace serialization


#endif // SERIALIZATION_QT_JSON_H
