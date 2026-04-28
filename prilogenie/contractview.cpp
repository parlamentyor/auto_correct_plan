#include "contractview.h"

#include <QHeaderView>
#include <QStyledItemDelegate>
#include <QMouseEvent>

/*
ContractView::ContractView(QWidget* parent) : QTableView(parent) {
    // Настройка внешнего вида
    setAlternatingRowColors(true);
    setSelectionBehavior(QAbstractItemView::SelectRows);
    setSelectionMode(QAbstractItemView::SingleSelection);

    // Настройка заголовков
    horizontalHeader()->setStretchLastSection(false);
    horizontalHeader()->setSectionResizeMode(QHeaderView::Interactive);
    verticalHeader()->setVisible(false);

    // Автоподбор высоты строк
    verticalHeader()->setSectionResizeMode(QHeaderView::ResizeToContents);
}

void ContractView::resizeEvent(QResizeEvent* event) {
    QTableView::resizeEvent(event);
    updateColumnWidths();
}

void ContractView::updateColumnWidths() {
    if (!model()) return;

    int totalWidth = viewport()->width();

    // Устанавливаем пропорции столбцов
    horizontalHeader()->setSectionResizeMode(0, QHeaderView::Interactive);
    horizontalHeader()->setSectionResizeMode(1, QHeaderView::Interactive);
    horizontalHeader()->setSectionResizeMode(2, QHeaderView::Interactive);
    horizontalHeader()->setSectionResizeMode(3, QHeaderView::Interactive);
    horizontalHeader()->setSectionResizeMode(4, QHeaderView::Interactive);
    horizontalHeader()->setSectionResizeMode(5, QHeaderView::Interactive);
    horizontalHeader()->setSectionResizeMode(6, QHeaderView::Interactive);

    // Задаем ширину столбцов (в процентах от общей ширины)
    horizontalHeader()->resizeSection(0, totalWidth * 0.08);  // 8% - № п/п
    horizontalHeader()->resizeSection(1, totalWidth * 0.30);  // 30% - Наименование
    horizontalHeader()->resizeSection(2, totalWidth * 0.15);  // 15% - Договор
    horizontalHeader()->resizeSection(3, totalWidth * 0.10);  // 10% - Срок по договору
    horizontalHeader()->resizeSection(4, totalWidth * 0.12);  // 12% - Отв. исполнитель
    horizontalHeader()->resizeSection(5, totalWidth * 0.10);  // 10% - Срок исполнения
    horizontalHeader()->resizeSection(6, totalWidth * 0.15);  // 15% - Отметка
}


void SpanDelegate::paint(QPainter* painter, const QStyleOptionViewItem& option,
           const QModelIndex& index) const {
    // Проверяем, является ли ячейка частью объединенной области
    if (index.column() == 0) {
        auto model = dynamic_cast<const ContractModel*>(index.model());
        if (model) {
            auto info = model->getItemInfo(index.row());
            if (info.type == ContractModel::ItemInfo::Type::ContractHeader) {
                // Рисуем объединенную ячейку на весь ряд
                QStyleOptionViewItem opt = option;
                opt.rect = QRect(opt.rect.x(), opt.rect.y(),
                                 opt.widget->width(), opt.rect.height());
                QStyledItemDelegate::paint(painter, opt, index);
                return;
            }
        }
    }
    QStyledItemDelegate::paint(painter, option, index);
}
*/

ContractView::ContractView(QWidget* parent) : QTableView(parent) {
    // Настройка внешнего вида
    setAlternatingRowColors(true);
    setSelectionBehavior(QAbstractItemView::SelectRows);
    setSelectionMode(QAbstractItemView::SingleSelection);

    // Настройка заголовков
    horizontalHeader()->setStretchLastSection(false);
    horizontalHeader()->setSectionResizeMode(QHeaderView::Interactive);
    verticalHeader()->setVisible(false);

    // Автоподбор высоты строк
    verticalHeader()->setSectionResizeMode(QHeaderView::ResizeToContents);

    // Важно: отключаем обновление фокуса по ячейкам
    setFocusPolicy(Qt::WheelFocus);

    // Убираем сетку для объединенных строк (опционально)
    setShowGrid(true);
}

void ContractView::paintEvent(QPaintEvent* event) {
    // Стандартная отрисовка
    QTableView::paintEvent(event);
}

// Переопределяем для правильного выделения при клике
void ContractView::mousePressEvent(QMouseEvent* event) {
    QModelIndex index = indexAt(event->pos());
    if (index.isValid()) {
        auto* model = dynamic_cast<const ContractModel*>(this->model());
        if (model) {
            auto info = model->getItemInfo(index.row());
            // Для объединенной строки выделяем всю строку при клике на любой столбец
            if (info.type == ContractModel::ItemInfo::Type::ContractHeader) {
                selectRow(index.row());
                return;
            }
        }
    }
    QTableView::mousePressEvent(event);
}


void SpanDelegate::paint(QPainter* painter, const QStyleOptionViewItem& option,
           const QModelIndex& index) const {
//    auto* tableView = qobject_cast<QTableView*>(option.widget); // тут ошибка заменил далее объявлением и if...
    // Правильное получение tableView
    QTableView* tableView = nullptr;
    if (option.widget) {
        // const_cast безопасен, так как widget на самом деле не константный
        tableView = qobject_cast<QTableView*>(const_cast<QWidget*>(option.widget));
    }

    auto* model = dynamic_cast<const ContractModel*>(index.model());

    if (model && tableView) {
        auto info = model->getItemInfo(index.row());

        // Обработка объединенной ячейки заголовка контракта
        if (info.type == ContractModel::ItemInfo::Type::ContractHeader && index.column() == 0) {
            // Получаем стиль для выделения
            QStyleOptionViewItem opt = option;

            // Определяем, выделена ли строка
            bool isSelected = option.state & QStyle::State_Selected;

            // Если строка выделена, рисуем фон выделения на всю ширину
            if (isSelected) {
                // Рисуем фон выделения на всю ширину таблицы
                QRect fullRowRect = option.rect;
                fullRowRect.setX(0);
                fullRowRect.setWidth(tableView->viewport()->width());

                // Рисуем фон выделения
                QStyleOptionViewItem selectionOpt = option;
                selectionOpt.rect = fullRowRect;
                selectionOpt.state |= QStyle::State_Selected;
                tableView->style()->drawPrimitive(QStyle::PE_PanelItemViewRow,
                                                  &selectionOpt, painter, tableView);

                // Рисуем рамку выделения
                tableView->style()->drawPrimitive(QStyle::PE_FrameFocusRect,
                                                  &selectionOpt, painter, tableView);
            } else {
                // Если не выделена, рисуем обычный фон
                tableView->style()->drawPrimitive(QStyle::PE_PanelItemViewItem,
                                                  &opt, painter, tableView);
            }

            // Рисуем текст на всю ширину
            opt.rect = option.rect;
            opt.rect.setWidth(tableView->viewport()->width() - opt.rect.x());

            // Получаем данные для отображения
            QVariant displayData = index.data(Qt::DisplayRole);
            QVariant fontData = index.data(Qt::FontRole);
            QVariant bgData = index.data(Qt::BackgroundRole);

            if (fontData.canConvert<QFont>()) {
                opt.font = fontData.value<QFont>();
            }

            if (bgData.canConvert<QBrush>() && !isSelected) {
                opt.backgroundBrush = bgData.value<QBrush>();
            }

            // Рисуем текст
            QStyledItemDelegate::paint(painter, opt, index);
            return;
        }

        // Для остальных ячеек с данными в других столбцах
        if (info.type == ContractModel::ItemInfo::Type::ContractHeader && index.column() > 0) {
            // Пустые ячейки в объединенной строке - ничего не рисуем, но фон обрабатываем
            bool isSelected = option.state & QStyle::State_Selected;
            if (isSelected) {
                // Рисуем фон выделения для всей строки
                QRect fullRowRect = option.rect;
                fullRowRect.setX(0);
                fullRowRect.setWidth(tableView->viewport()->width());

                QStyleOptionViewItem selectionOpt = option;
                selectionOpt.rect = fullRowRect;
                selectionOpt.state |= QStyle::State_Selected;
                tableView->style()->drawPrimitive(QStyle::PE_PanelItemViewRow,
                                                  &selectionOpt, painter, tableView);
            } else {
                // Обычный фон
                tableView->style()->drawPrimitive(QStyle::PE_PanelItemViewItem,
                                                  &option, painter, tableView);
            }
            return;
        }
    }

    // Стандартная отрисовка для всех остальных ячеек
    QStyledItemDelegate::paint(painter, option, index);
}

void ContractView::resizeEvent(QResizeEvent* event) {
    QTableView::resizeEvent(event);
    updateColumnWidths();
}

void ContractView::updateColumnWidths() {
    if (!model()) return;

    int totalWidth = viewport()->width();

    // Устанавливаем пропорции столбцов
    horizontalHeader()->setSectionResizeMode(0, QHeaderView::Interactive);
    horizontalHeader()->setSectionResizeMode(1, QHeaderView::Interactive);
    horizontalHeader()->setSectionResizeMode(2, QHeaderView::Interactive);
    horizontalHeader()->setSectionResizeMode(3, QHeaderView::Interactive);
    horizontalHeader()->setSectionResizeMode(4, QHeaderView::Interactive);
    horizontalHeader()->setSectionResizeMode(5, QHeaderView::Interactive);
    horizontalHeader()->setSectionResizeMode(6, QHeaderView::Interactive);

    // Задаем ширину столбцов (в процентах от общей ширины)
    horizontalHeader()->resizeSection(0, totalWidth * 0.08);  // 8% - № п/п
    horizontalHeader()->resizeSection(1, totalWidth * 0.30);  // 30% - Наименование
    horizontalHeader()->resizeSection(2, totalWidth * 0.15);  // 15% - Договор
    horizontalHeader()->resizeSection(3, totalWidth * 0.10);  // 10% - Срок по договору
    horizontalHeader()->resizeSection(4, totalWidth * 0.12);  // 12% - Отв. исполнитель
    horizontalHeader()->resizeSection(5, totalWidth * 0.10);  // 10% - Срок исполнения
    horizontalHeader()->resizeSection(6, totalWidth * 0.15);  // 15% - Отметка
}

QSize SpanDelegate::sizeHint(const QStyleOptionViewItem& option, const QModelIndex& index) const {
    auto* model = dynamic_cast<const ContractModel*>(index.model());
    if (model) {
        auto info = model->getItemInfo(index.row());
        // Для объединенной ячейки даем достаточно места
        if (info.type == ContractModel::ItemInfo::Type::ContractHeader && index.column() == 0) {
            QSize size = QStyledItemDelegate::sizeHint(option, index);
            size.setWidth(option.widget->width());
            return size;
        }
    }
    return QStyledItemDelegate::sizeHint(option, index);
}
