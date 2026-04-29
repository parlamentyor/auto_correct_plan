#ifndef HANDLER_ADD_WORK_H
#define HANDLER_ADD_WORK_H

#include "model.h"

#include <QTableWidget>
#include <QString>

#include <string>
#include <vector>
#include <sstream>
#include <iomanip>


namespace details {
    // Функция для форматирования даты
    inline QString FormatDate(const model::Date& date) {
        return QString("%1.%2.%3")
            .arg(date.day_, 2, 10, QChar('0'))
            .arg(date.month_, 2, 10, QChar('0'))
            .arg(date.year_, 4, 10, QChar('0'));
    }

    // Альтернативный вариант с использованием stringstream
    inline QString FormatDateAlt(const model::Date& date) {
        std::stringstream ss;
        ss << std::setfill('0')
           << std::setw(2) << date.day_ << "."
           << std::setw(2) << date.month_ << "."
           << std::setw(4) << date.year_;
        return QString::fromStdString(ss.str());
    }

    // Функция для преобразования вектора строк в многострочный текст
    inline QString FormatResponsibleEmployees(const std::vector<std::string>& employees) {
        QString result;
        for (size_t i = 0; i < employees.size(); ++i) {
            if (i > 0) {
                result += "\n";
            }
            result += QString::fromStdString(employees[i]);
        }
        return result;
    }

    inline void SetRowBold(QTableWidget* table, int row) {
        QFont boldFont;
        boldFont.setBold(true);

        for (int col = 0; col < table->columnCount(); ++col) {
            QTableWidgetItem* item = table->item(row, col);
            if (item) {
                item->setFont(boldFont);
            }
        }
    }

    // Основная функция добавления строки в таблицу
    inline void AddSeparateWorkToTable(QTableWidget* table, const model::SeparateWork& work) {
        // Получаем текущее количество строк
        int rowCount = table->rowCount();

        // Добавляем новую строку
        table->insertRow(rowCount);

        // Заполняем первый столбец (name_)
        QTableWidgetItem* nameItem = new QTableWidgetItem(QString::fromStdString(work.name_));
        nameItem->setFlags(nameItem->flags() | Qt::ItemIsEditable);
        table->setItem(rowCount, 0, nameItem);

        // Второй столбец может быть пустым или содержать что-то еще
        table->setItem(rowCount, 1, new QTableWidgetItem(""));

        // Заполняем третий столбец (name_responsible_employee_)
        QTableWidgetItem* respItem = new QTableWidgetItem(FormatResponsibleEmployees(work.names_responsible_employees_));
        respItem->setFlags(respItem->flags() | Qt::ItemIsEditable);
        table->setItem(rowCount, 2, respItem);

        // Заполняем четвёртый столбец (date_deadline_)
        QTableWidgetItem* dateItem = new QTableWidgetItem(FormatDate(work.date_deadline_));
        dateItem->setFlags(dateItem->flags() | Qt::ItemIsEditable);
        table->setItem(rowCount, 3, dateItem);

        // Заполняем пятый столбец (info_)
        QTableWidgetItem* infoItem = new QTableWidgetItem(QString::fromStdString(work.info_.value_or("")));
        infoItem->setFlags(infoItem->flags() | Qt::ItemIsEditable);
        table->setItem(rowCount, 4, infoItem);
    }

    inline void AddStageToTable(QTableWidget* table, const model::Stage& stage) {
        // Получаем текущее количество строк
        int rowCount = table->rowCount();

        // Добавляем новую строку
        table->insertRow(rowCount);

        // Заполняем первый столбец (name_)
        table->setItem(rowCount, 0,
                       new QTableWidgetItem(QString::fromStdString(stage.name_full_.value_or(""))));

        // Заполняем Второй столбец (date_deadline_) в формате dd.mm.yyyy
        QString date_deadline = "";
        if (stage.date_deadline_.has_value()) {
            date_deadline = FormatDate(stage.date_deadline_.value());
        }
        table->setItem(rowCount, 1,
                       new QTableWidgetItem(date_deadline));

        // Заполняем третий столбец (name_responsible_employee_)
        table->setItem(rowCount, 2,
                       new QTableWidgetItem(QString::fromStdString(stage.name_responsible_employee_.value_or(""))));

        // Заполняем пятый столбец (info_)
        table->setItem(rowCount, 4,
                       new QTableWidgetItem(QString::fromStdString(stage.info_.value_or(""))));

        SetRowBold(table, rowCount);
    }
} //namespace details
#endif // HANDLER_ADD_WORK_H
