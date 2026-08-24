#ifndef HANDLER_ADD_WORK_H
#define HANDLER_ADD_WORK_H

#include "model.h"
#include "general_qt_functions.h"

#include <QTableWidget>
#include <QString>

#include <string>
#include <vector>
#include <sstream>
#include <iomanip>


namespace details {
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
        QString date_deadline = "";
        if (work.date_deadline_.has_value()) {
            date_deadline = FormatDateToQstring(work.date_deadline_.value());
        }
        QTableWidgetItem* dateItem = new QTableWidgetItem(date_deadline);
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
//        table->setItem(rowCount, 0,
//                       new QTableWidgetItem(QString::fromStdString(stage.name_full_.value_or(""))));
        QTableWidgetItem* nameItem = new QTableWidgetItem(QString("Этап %1\n%2").arg(QString::fromStdString(stage.number_)).arg(QString::fromStdString(stage.name_full_.value_or(""))));
        nameItem->setFlags(nameItem->flags() | Qt::ItemIsEditable);
        table->setItem(rowCount, 0, nameItem);

        // Заполняем Второй столбец (date_deadline_) в формате dd.mm.yyyy
        QString date_deadline = "";
        if (stage.date_deadline_.has_value()) {
            date_deadline = FormatDateToQstring(stage.date_deadline_.value());
        }
//        table->setItem(rowCount, 1,
//                       new QTableWidgetItem(date_deadline));
        QTableWidgetItem* dateItem = new QTableWidgetItem(date_deadline);
        dateItem->setFlags(dateItem->flags() | Qt::ItemIsEditable);
        table->setItem(rowCount, 1, dateItem);

        // Заполняем третий столбец (name_responsible_employee_)
//        table->setItem(rowCount, 2,
//                       new QTableWidgetItem(QString::fromStdString(stage.name_responsible_employee_.value_or(""))));
        QTableWidgetItem* respItem = new QTableWidgetItem(QString::fromStdString(stage.name_responsible_employee_.value_or("")));
        respItem->setFlags(respItem->flags() | Qt::ItemIsEditable);
        table->setItem(rowCount, 2, respItem);

        // Заполняем четвёртый столбец
        table->setItem(rowCount, 3, new QTableWidgetItem(""));

        // Заполняем пятый столбец (info_)
//        table->setItem(rowCount, 4,
//                       new QTableWidgetItem(QString::fromStdString(stage.info_.value_or(""))));
        QTableWidgetItem* infoItem = new QTableWidgetItem(QString::fromStdString(stage.info_.value_or("")));
        infoItem->setFlags(infoItem->flags() | Qt::ItemIsEditable);
        table->setItem(rowCount, 4, infoItem);

        SetRowBold(table, rowCount);
    }

    inline void AddHeaderToTable(QTableWidget* table, std::string name_header) {
        // Получаем текущее количество строк
        int rowCount = table->rowCount();

        // Добавляем новую строку
        table->insertRow(rowCount);

        QTableWidgetItem* nameItem = new QTableWidgetItem(QString::fromStdString(name_header));
        nameItem->setFlags(nameItem->flags() | Qt::ItemIsEditable);
        table->setItem(rowCount, 0, nameItem);

        SetRowBold(table, rowCount);
    }
} //namespace details
#endif // HANDLER_ADD_WORK_H
