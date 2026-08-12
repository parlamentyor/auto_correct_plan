#ifndef CONTRACTMODEL_H
#define CONTRACTMODEL_H

#include <QAbstractTableModel>
#include <QFont>
#include <memory>
#include "abstractcontractmodel.h"
#include "model.h"

class ContractModel : public AbstractContractModel {
    Q_OBJECT

public:
    explicit ContractModel(std::vector<std::shared_ptr<model::Contract>>& contracts, QObject* parent = nullptr);

    // Базовые методы модели
    int rowCount(const QModelIndex& parent = QModelIndex()) const override;
    int columnCount(const QModelIndex& parent = QModelIndex()) const override;
    QVariant data(const QModelIndex& index, int role = Qt::DisplayRole) const override;
    QVariant headerData(int section, Qt::Orientation orientation, int role = Qt::DisplayRole) const override;

    // Для объединения ячеек
    Qt::ItemFlags flags(const QModelIndex& index) const override;

    // Методы для работы с данными
    bool removeContract(int contractIndex) override;
    bool markContractAsCompleted(int contractIndex) override;

    ItemInfo getItemInfo(int virtualRow) const override;


    // Обновление модели
    void refreshModel() override;

private:
    // Структура для хранения информации о виртуальных строках
    struct VirtualRow {
        ItemInfo info;
    };

    mutable std::vector<VirtualRow> virtualRows_;
    void buildVirtualRows() const;

    // Вспомогательные методы для форматирования
    QString formatDate(const model::Date& date) const;
    QString formatPrice(const model::Price& price) const;
    QStringList formatResponsibleEmployees(const std::vector<std::string>& employees) const;

    // Методы для получения данных из разных типов строк
    QVariant getContractHeaderData(const ItemInfo& info, int column, int role) const;
    QVariant getContractRowData(const ItemInfo& info, int column, int role) const;
    QVariant getStageRowData(const ItemInfo& info, int column, int role) const;
    QVariant getWorkRowData(const ItemInfo& info, int column, int role) const;

    // Проверка, выполнен ли контракт
    bool isContractCompleted(int contractIndex) const;
};

#endif // CONTRACTMODEL_H
