#ifndef HANDLER_ADD_WORK_H
#define HANDLER_ADD_WORK_H

#include "model.h"

#include <QTableWidget>
#include <QString>
#include <string>
#include <vector>
#include <sstream>
#include <iomanip>

// Функция для форматирования даты
inline QString formatDate(const model::Date& date) {
    return QString("%1.%2.%3")
        .arg(date.day_, 2, 10, QChar('0'))
        .arg(date.month_, 2, 10, QChar('0'))
        .arg(date.year_, 4, 10, QChar('0'));
}

// Альтернативный вариант с использованием stringstream
inline QString formatDateAlt(const model::Date& date) {
    std::stringstream ss;
    ss << std::setfill('0')
       << std::setw(2) << date.day_ << "."
       << std::setw(2) << date.month_ << "."
       << std::setw(4) << date.year_;
    return QString::fromStdString(ss.str());
}

// Функция для преобразования вектора строк в многострочный текст
inline QString formatResponsibleEmployees(const std::vector<std::string>& employees) {
    QString result;
    for (size_t i = 0; i < employees.size(); ++i) {
        if (i > 0) {
            result += "\n";
        }
        result += QString::fromStdString(employees[i]);
    }
    return result;
}

// Основная функция добавления строки в таблицу
inline void addSeparateWorkToTable(QTableWidget* table, const model::SeparateWork& work) {
    // Получаем текущее количество строк
    int rowCount = table->rowCount();

    // Добавляем новую строку
    table->insertRow(rowCount);

    // Заполняем первый столбец (name_)
    table->setItem(rowCount, 0,
                   new QTableWidgetItem(QString::fromStdString(work.name_)));

    // Заполняем второй столбец (name_responsible_employee_)
    // Каждый элемент с новой строки
    table->setItem(rowCount, 1,
                   new QTableWidgetItem(formatResponsibleEmployees(work.names_responsible_employees_)));

    // Заполняем третий столбец (date_deadline_) в формате dd.mm.yyyy
    table->setItem(rowCount, 2,
                   new QTableWidgetItem(formatDate(work.date_deadline_)));

    // Заполняем четвёртый столбец (info_)
    table->setItem(rowCount, 3,
                   new QTableWidgetItem(QString::fromStdString(work.info_.value_or(""))));
}

#endif // HANDLER_ADD_WORK_H
