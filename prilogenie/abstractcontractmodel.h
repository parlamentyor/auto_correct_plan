#ifndef ABSTRACTCONTRACTMODEL_H
#define ABSTRACTCONTRACTMODEL_H

#include <QAbstractTableModel>
#include <memory>
#include <vector>
#include "model.h"

class AbstractContractModel : public QAbstractTableModel {
    Q_OBJECT

public:
    explicit AbstractContractModel(std::vector<std::shared_ptr<model::Contract>>& contracts, QObject *parent = nullptr)
        : QAbstractTableModel(parent)
        , contracts_(contracts) {}

    virtual ~AbstractContractModel() = default;

    // Общий метод для обновления всех представлений при изменении данных
    virtual void refreshModel() {
        beginResetModel();
        endResetModel();
    }

    // Методы для работы с данными
    virtual bool removeContract(int contractIndex) = 0;
    virtual bool markContractAsCompleted(int contractIndex) = 0;

    // Получить оригинальные данные по виртуальному индексу
    struct ItemInfo {
        enum class Type { ContractHeader, ContractRow, StageRow, WorkRow };
        Type type;
        int contractIndex;
        int stageIndex;
        int workIndex;
        int virtualRow;
    };

    virtual ItemInfo getItemInfo(int virtualRow) const = 0;

protected:
    std::vector<std::shared_ptr<model::Contract>>& contracts_;

    QString formatDate(const model::Date& date) const {
        return QString("%1.%2.%3")
            .arg(date.day_, 2, 10, QChar('0'))
            .arg(date.month_, 2, 10, QChar('0'))
            .arg(date.year_);
    }
};

#endif // ABSTRACTCONTRACTMODEL_H
