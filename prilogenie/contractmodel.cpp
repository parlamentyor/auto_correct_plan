#include "contractmodel.h"
#include "general_qt_functions.h"

#include <QDateTime>
#include <QBrush>
#include <QDebug>

ContractModel::ContractModel(std::vector<std::shared_ptr<model::Contract>>& contracts, QObject* parent)
    : AbstractContractModel(contracts, parent) {
    buildVirtualRows();
}

void ContractModel::refreshModel() {
    beginResetModel();
    buildVirtualRows();
    endResetModel();
}

void ContractModel::buildVirtualRows() const {
    virtualRows_.clear();
    int globalRow = 0;

    for (size_t contractIdx = 0; contractIdx < contracts_.size(); ++contractIdx) {
        const auto& contract = contracts_[contractIdx];

        // Строка заголовка контракта (объединенная)
        virtualRows_.push_back({{ItemInfo::Type::ContractHeader,
                                 static_cast<int>(contractIdx), -1, -1, globalRow++}});

        // Строка с основными данными контракта
        virtualRows_.push_back({{ItemInfo::Type::ContractRow,
                                 static_cast<int>(contractIdx), -1, -1, globalRow++}});

//        int subCounter = 1; // для нумерации работ без этапов // IDE РУГАЕТСЯ, ГОТОРИТ НЕ НУЖНА ЭТА ПЕРЕМЕННАЯ

        // Отображаем этапы
        if (contract->pool_stage_.has_value()) {
            const auto& stages = contract->pool_stage_.value();
            for (size_t stageIdx = 0; stageIdx < stages.size(); ++stageIdx) {
                // Строка этапа
                virtualRows_.push_back({{ItemInfo::Type::StageRow,
                                         static_cast<int>(contractIdx),
                                         static_cast<int>(stageIdx), -1, globalRow++}});

                // Отображаем работы этапа
                if (stages[stageIdx]->pool_work_.has_value()) {
                    const auto& works = stages[stageIdx]->pool_work_.value();
                    for (size_t workIdx = 0; workIdx < works.size(); ++workIdx) {
                        virtualRows_.push_back({{ItemInfo::Type::WorkRow,
                                                 static_cast<int>(contractIdx),
                                                 static_cast<int>(stageIdx),
                                                 static_cast<int>(workIdx), globalRow++}});
                    }
                }
//                subCounter = stages.size() + 1;   // IDE РУГАЕТСЯ, ГОВОРИТ НЕ НУЖНА ЭТА ПЕРЕМЕННАЯ c 'nbv ltqcndbtv
            }
        }

        // Отображаем работы контракта (без этапов)
        if (contract->pool_work.has_value()) {
            const auto& works = contract->pool_work.value();
            for (size_t workIdx = 0; workIdx < works.size(); ++workIdx) {
                virtualRows_.push_back({{ItemInfo::Type::WorkRow,
                                         static_cast<int>(contractIdx),
                                         -1,
                                         static_cast<int>(workIdx), globalRow++}});
            }
        }
    }
}

int ContractModel::rowCount(const QModelIndex& parent) const {
    if (parent.isValid()) return 0;
    return static_cast<int>(virtualRows_.size());
}

int ContractModel::columnCount(const QModelIndex& parent) const {
    if (parent.isValid()) return 0;
    return 7; // 7 столбцов
}

bool ContractModel::removeContract(int contractIndex) {
    if (contractIndex < 0 || contractIndex >= static_cast<int>(contracts_.size())) {
        return false;
    }

    beginResetModel();
    contracts_.erase(contracts_.begin() + contractIndex);
    buildVirtualRows();
    endResetModel();

    return true;
}

bool ContractModel::markContractAsCompleted(int contractIndex) {
    if (contractIndex < 0 || contractIndex >= static_cast<int>(contracts_.size())) {
        return false;
    }

    contracts_[contractIndex]->status_complet_.is_complet_ = true;

    // Обновляем все строки, связанные с этим контрактом
    for (int row = 0; row < static_cast<int>(virtualRows_.size()); ++row) {
        const auto& info = virtualRows_[row].info;
        if (info.contractIndex == contractIndex) {
            QModelIndex topLeft = index(row, 0);
            QModelIndex bottomRight = index(row, columnCount() - 1);
            emit dataChanged(topLeft, bottomRight, {Qt::BackgroundRole});
        }
    }

    return true;
}

bool ContractModel::isContractCompleted(int contractIndex) const {
    if (contractIndex < 0 || contractIndex >= static_cast<int>(contracts_.size())) {
        return false;
    }
    return contracts_[contractIndex]->status_complet_.is_complet_;
}

QVariant ContractModel::headerData(int section, Qt::Orientation orientation, int role) const {
    if (orientation == Qt::Horizontal && role == Qt::DisplayRole) {
        switch (section) {
        case 0: return "№ п/п";
        case 1: return "Наименование, этапы и содержание работ";
        case 2: return "Договор";
        case 3: return "Срок исполнения по договору";
        case 4: return "Отв. Исполнитель";
        case 5: return "Срок исполнения";
        case 6: return "Отметка о выполнении";
        default: return QVariant();
        }
    }
    return QVariant();
}

QVariant ContractModel::data(const QModelIndex& index, int role) const {
    if (!index.isValid() || index.row() >= static_cast<int>(virtualRows_.size()))
        return QVariant();

    const auto& itemInfo = virtualRows_[index.row()].info;
/*
    // Если нужно все строки договора окрасить в цвета ВЫПОЛНЕНО
    if (role == Qt::BackgroundRole && isContractCompleted(itemInfo.contractIndex)) {
        if (itemInfo.type != ItemInfo::Type::ContractHeader) {
            return QBrush(QColor(144, 238, 144)); // Зеленый для всех строк контракта
        }
    }
*/
    switch (itemInfo.type) {
    case ItemInfo::Type::ContractHeader:
        return getContractHeaderData(itemInfo, index.column(), role);
    case ItemInfo::Type::ContractRow:
        return getContractRowData(itemInfo, index.column(), role);
    case ItemInfo::Type::StageRow:
        return getStageRowData(itemInfo, index.column(), role);
    case ItemInfo::Type::WorkRow:
        return getWorkRowData(itemInfo, index.column(), role);
    default:
        return QVariant();
    }
}

Qt::ItemFlags ContractModel::flags(const QModelIndex& index) const {
    if (!index.isValid()) return Qt::NoItemFlags;

    auto info = getItemInfo(index.row());

    if (info.type == ItemInfo::Type::ContractHeader) {
        if (index.column() == 0) {
            return Qt::ItemIsEnabled | Qt::ItemIsSelectable;
        }
        return Qt::NoItemFlags;
    }

    return Qt::ItemIsEnabled | Qt::ItemIsSelectable;
}


QVariant ContractModel::getContractHeaderData(const ItemInfo& info, int column, int role) const {
    const auto& contract = contracts_[info.contractIndex];

    // Только для первого столбца возвращаем данные
    if (column != 0) {
        if (role == Qt::BackgroundRole && !isContractCompleted(info.contractIndex)) {
            // Возвращаем цвет фона для всех столбцов объединенной строки
            return QBrush(QColor(200, 255, 200));
        }
        return QVariant();
    }

    switch (role) {
    case Qt::DisplayRole: {
// Было так, предложено изначально
/*
        QString headerText;
        if (contract->name_organization_.has_value() && contract->name_short_.has_value()) {
            headerText = QString::fromStdString(contract->name_organization_.value() +
                                                " (" + contract->name_short_.value() + ")");
        }
        return headerText;
*/
        QString header_text = "";
        if (contract->name_organization_.has_value() && contract->name_short_.has_value()) {
            header_text = QString::fromStdString(contract->name_organization_.value() +
                                                " (" + contract->name_short_.value() + ")");
        }
        else if (contract->name_organization_.has_value() && !(contract->name_short_.has_value())) {
            header_text = QString::fromStdString(contract->name_organization_.value());
        }
        else if (contract->name_short_.has_value() && !(contract->name_organization_.has_value())) {
            header_text = QString::fromStdString(contract->name_short_.value());
        }

        return header_text;
    }
    case Qt::FontRole: {
        QFont font;
        font.setBold(true);
        font.setPointSize(font.pointSize() + 2);
        return font;
    }
    case Qt::BackgroundRole: {
        if (!isContractCompleted(info.contractIndex)) {
            return QBrush(QColor(200, 255, 200)); //Светло-зелёный
        }
        return QBrush(QColor(144, 238, 144));
    }
    case Qt::TextAlignmentRole: {
        return Qt::AlignCenter;
    }
    default:
        return QVariant();
    }
}

QVariant ContractModel::getContractRowData(const ItemInfo& info, int column, int role) const {
    const auto& contract = contracts_[info.contractIndex];

    if (role == Qt::FontRole) {
        QFont font;
        font.setBold(true);
        return font;
    }

    if (role == Qt::DisplayRole) {
        switch (column) {
        case 0: return QString::number(info.contractIndex + 1);
        case 1: return contract->name_full_.has_value() ?
                       QString::fromStdString(contract->name_full_.value()) : "";
        case 2: {
            if (contract->number_.has_value() && contract->date_.has_value()) {
                return QString("Договор № %1 от %2")
                    .arg(QString::fromStdString(contract->number_.value()))
                    .arg(details::FormatDateToQstring(contract->date_.value()));
            }
            return "";
        }
        case 3: {
            if (contract->date_deadline_.has_value()) {
                return details::FormatDateToQstring(contract->date_deadline_.value());
            }
            return "";
        }
        case 4: return contract->name_responsible_employee_.has_value() ?
                       QString::fromStdString(contract->name_responsible_employee_.value()) : "";
        case 5: return ""; // Срок исполнения для контракта не указан
        case 6: return contract->info_.has_value() ?
                       QString::fromStdString(contract->info_.value()) : "";
        default: return QVariant();
        }
    }

    if (role == Qt::TextAlignmentRole) {
        if (column == 0 || column == 1) return Qt::AlignLeft;
        return Qt::AlignCenter;
    }

    if (role == Qt::FontRole && column == 6) {
        QFont font;
        font.setPointSize(font.pointSize() - 2);
        return font;
    }

    return QVariant();
}

QVariant ContractModel::getStageRowData(const ItemInfo& info, int column, int role) const {
    const auto& contract = contracts_[info.contractIndex];
    if (!contract->pool_stage_.has_value()) return QVariant();

    const auto& stages = contract->pool_stage_.value();
    if (info.stageIndex >= static_cast<int>(stages.size())) return QVariant();

    const auto& stage = stages[info.stageIndex];

    if (role == Qt::FontRole) {
        QFont font;
        font.setBold(true);
        return font;
    }

    if (role == Qt::DisplayRole) {
        switch (column) {
        case 0: return QString("%1.%2").arg(info.contractIndex + 1).arg(info.stageIndex + 1);
        case 1: return stage->name_full_.has_value() ?
                       QString("Этап №%1 %2").arg(QString::fromStdString(stage->number_)).arg(QString::fromStdString(stage->name_full_.value())) : "";
        case 3: return stage->date_deadline_.has_value() ?
                       details::FormatDateToQstring(stage->date_deadline_.value()) : "";
        case 4: return stage->name_responsible_employee_.has_value() ?
                       QString::fromStdString(stage->name_responsible_employee_.value()) : "";
        case 6: return stage->info_.has_value() ?
                       QString::fromStdString(stage->info_.value()) : "";
        default: return QVariant();
        }
    }

    if (role == Qt::TextAlignmentRole) {
        if (column == 0 || column == 1) return Qt::AlignLeft;
        return Qt::AlignCenter;
    }

    if (role == Qt::FontRole && column == 6) {
        QFont font;
        font.setPointSize(10);
        return font;
    }

    return QVariant();
}

QVariant ContractModel::getWorkRowData(const ItemInfo& info, int column, int role) const {
    const auto& contract = contracts_[info.contractIndex];

    const model::SeparateWork* work = nullptr;
    QString prefix;

    // Определяем, откуда брать работу (из этапа или из контракта)
    if (info.stageIndex >= 0 && contract->pool_stage_.has_value()) {
        const auto& stages = contract->pool_stage_.value();
        if (info.stageIndex < static_cast<int>(stages.size()) &&
            stages[info.stageIndex]->pool_work_.has_value()) {
            const auto& works = stages[info.stageIndex]->pool_work_.value();
            if (info.workIndex < static_cast<int>(works.size())) {
                work = works[info.workIndex].get();
                prefix = QString("%1.%2.%3")
                             .arg(info.contractIndex + 1)
                             .arg(info.stageIndex + 1)
                             .arg(info.workIndex + 1);
            }
        }
    } else if (contract->pool_work.has_value()) {
        const auto& works = contract->pool_work.value();
        if (info.workIndex < static_cast<int>(works.size())) {
            work = works[info.workIndex].get();

            // Вычисляем номер для работы контракта
            int stageCount = contract->pool_stage_.has_value() ?
                                 contract->pool_stage_.value().size() : 0;
            prefix = QString("%1.%2")
                         .arg(info.contractIndex + 1)
                         .arg(stageCount + info.workIndex + 1);
        }
    }

    if (!work) return QVariant();

    if (role == Qt::DisplayRole) {
        switch (column) {
        case 0: return prefix;
        case 1: return QString::fromStdString(work->name_);
        case 4: return formatResponsibleEmployees(work->names_responsible_employees_).join("\n");
        case 5: return work->date_deadline_.has_value() ?
                       details::FormatDateToQstring(work->date_deadline_.value()) : "";
        case 6: return work->info_.has_value() ?
                       QString::fromStdString(work->info_.value()) : "";
        default: return QVariant();
        }
    }

    if (role == Qt::TextAlignmentRole) {
        if (column == 0 || column == 1) return Qt::AlignLeft;
        return Qt::AlignCenter;
    }

    if (role == Qt::FontRole && column == 6) {
        QFont font;
        font.setPointSize(10);
        return font;
    }

    return QVariant();
}

QStringList ContractModel::formatResponsibleEmployees(const std::vector<std::string>& employees) const {
    QStringList result;
    for (const auto& emp : employees) {
        result << QString::fromStdString(emp);
    }
    return result;
}

ContractModel::ItemInfo ContractModel::getItemInfo(int virtualRow) const {
    if (virtualRow >= 0 && virtualRow < static_cast<int>(virtualRows_.size())) {
        return virtualRows_[virtualRow].info;
    }
    return {ItemInfo::Type::ContractHeader, -1, -1, -1, -1};
}
