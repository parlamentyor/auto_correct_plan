#ifndef CONTRACTVIEW_H
#define CONTRACTVIEW_H

#include <QTableView>
#include <QStyledItemDelegate>

#include "contractmodel.h"

// первая версия не все договора отрисовывались и после выделения надпись пропадала
/*
class ContractView : public QTableView {
    Q_OBJECT

public:
    explicit ContractView(QWidget* parent = nullptr);

protected:
    void resizeEvent(QResizeEvent* event) override;

private:
    void updateRowHeights();
    void updateColumnWidths();
};


class SpanDelegate : public QStyledItemDelegate {
public:
    using QStyledItemDelegate::QStyledItemDelegate;

    void paint(QPainter* painter, const QStyleOptionViewItem& option,
               const QModelIndex& index) const override;
};
*/

class ContractView : public QTableView {
    Q_OBJECT

public:
    explicit ContractView(QWidget* parent = nullptr);

protected:
    void paintEvent(QPaintEvent* event) override;

    // Переопределяем для правильного выделения при клике
    void mousePressEvent(QMouseEvent* event) override;

    void resizeEvent(QResizeEvent* event) override;
private:
    void updateRowHeights();
    void updateColumnWidths();
};

// Класс-делегат для поддержки объединения ячеек
class SpanDelegate : public QStyledItemDelegate {
public:
    using QStyledItemDelegate::QStyledItemDelegate;

    void paint(QPainter* painter, const QStyleOptionViewItem& option,
               const QModelIndex& index) const override;

    QSize sizeHint(const QStyleOptionViewItem& option, const QModelIndex& index) const override;
};

#endif // CONTRACTVIEW_H
