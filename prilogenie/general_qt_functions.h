#ifndef GENERAL_QT_FUNCTIONS_H
#define GENERAL_QT_FUNCTIONS_H

#include "model.h"

#include <optional>
#include <QString>
#include <QWidget>

namespace details {
    QString FormatDateToQstring(const model::Date& date);

    // Альтернативный вариант с использованием stringstream
    QString FormatDateToQstringAlt(const model::Date& date);

    std::optional<model::Date> OpenCalendar(QWidget* parent, const model::Date& initial_date);
} //namespace details
#endif // GENERAL_QT_FUNCTIONS_H
