#include "general_qt_functions.h"

#include <QAction>
#include <QCalendarWidget>
#include <QDialog>
#include <QVBoxLayout>

namespace details {
    QString FormatDateToQstring(const model::Date& date) {
        return QString("%1.%2.%3")
            .arg(date.day_, 2, 10, QChar('0'))
            .arg(date.month_, 2, 10, QChar('0'))
            .arg(date.year_);
    }

    // Альтернативный вариант с использованием stringstream
    QString FormatDateToQstringAlt(const model::Date& date) {
        std::stringstream ss;
        ss << std::setfill('0')
           << std::setw(2) << date.day_ << "."
           << std::setw(2) << date.month_ << "."
           << std::setw(4) << date.year_;
        return QString::fromStdString(ss.str());
    }

    std::optional<model::Date> OpenCalendar(QWidget* parent, const model::Date& initial_date) {
        // Создаём диалоговое окно
        QDialog* dialog = new QDialog(parent);
        dialog->setWindowTitle("Выберите дату");
        dialog->setModal(true);

        // Создаём календарь
        QCalendarWidget* calendar = new QCalendarWidget(dialog);

        // Устанавливаем текущую дату в календарь
        QDate current_date = QDate(initial_date.year_, initial_date.month_, initial_date.day_);
        if (current_date.isValid()) {
            calendar->setSelectedDate(current_date);
        }
        else {
            calendar->setSelectedDate(QDate::currentDate());
        }

        // Компоновка
        QVBoxLayout* layout = new QVBoxLayout(dialog);
        layout->addWidget(calendar);

        // Подключаем сигнал выбора даты

        std::optional<model::Date> result;

        QAction::connect(calendar, &QCalendarWidget::selectionChanged, [dialog, calendar, &result]() {
            QDate selected_date = calendar->selectedDate();
            model::Date date{selected_date.day(), selected_date.month(), selected_date.year()};
            result = date;
            dialog->accept();
        });

        // Позиционируем окно в месте курсора мыши
        QPoint cursor_pos = QCursor::pos();
        dialog->move(cursor_pos);

        // Показываем диалог
        dialog->exec();

        // Удаляем диалог после закрытия
        dialog->deleteLater();

        return result;
    }
} //namespace details
