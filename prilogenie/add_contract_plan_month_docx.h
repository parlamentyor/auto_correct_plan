#ifndef ADD_CONTRACT_PLAN_MONTH_DOCX_H
#define ADD_CONTRACT_PLAN_MONTH_DOCX_H

#include <model.h>
#include <handler_add_work.h>

#include <QString>
#include <QAxObject>
#include <QDebug>
#include <QColor>
#include <QVariant>
#include <optional>
#include <vector>
#include <string>

// неправильный класс в методах modifyThirdTable и fillSecondRow пытался вызвать метод querySubObject у table->property("Rows"),
// который возвращает QVariant а этот метод принадлежит QAxObject
/*
class WordDocxEditor {
public:
    static bool updateContractDocx(const QString& filePath, const model::Contract& contract) {
        QAxObject* word = nullptr;
        QAxObject* documents = nullptr;
        QAxObject* document = nullptr;

        try {
            // Создаем экземпляр Word
            word = new QAxObject("Word.Application");
            if (word->isNull()) {
                qDebug() << "Не удалось запустить Microsoft Word";
                return false;
            }

            // Делаем Word невидимым
            word->setProperty("Visible", false);

            // Получаем коллекцию документов
            documents = word->querySubObject("Documents");

            // Открываем документ
            document = documents->querySubObject("Open(const QString&)", filePath);
            if (!document || document->isNull()) {
                qDebug() << "Не удалось открыть документ:" << filePath;
                delete word;
                return false;
            }

            // Получаем коллекцию таблиц
            QAxObject* tables = document->querySubObject("Tables");
            if (!tables || tables->isNull()) {
                qDebug() << "В документе нет таблиц";
                document->dynamicCall("Close()");
                delete word;
                return false;
            }

            // Получаем количество таблиц
            int tableCount = tables->property("Count").toInt();
            if (tableCount < 3) {
                qDebug() << "В документе меньше 3 таблиц";
                document->dynamicCall("Close()");
                delete word;
                return false;
            }

            // Получаем третью таблицу (индекс 3 в Word, так как нумерация с 1)
            QAxObject* table = tables->querySubObject("Item(int)", 3);
            if (!table || table->isNull()) {
                qDebug() << "Не удалось получить третью таблицу";
                document->dynamicCall("Close()");
                delete word;
                return false;
            }

            // Добавляем новые строки
            modifyThirdTable(table, document, contract);

            // Сохраняем документ
            document->dynamicCall("Save()");

            // Закрываем документ
            document->dynamicCall("Close()");

            // Закрываем Word
            word->dynamicCall("Quit()");

            delete document;
            delete documents;
            delete word;

            return true;

        } catch (const std::exception& e) {
            qDebug() << "Ошибка при работе с Word:" << e.what();

            // Очистка ресурсов
            if (document) {
                document->dynamicCall("Close()");
                delete document;
            }
            if (documents) delete documents;
            if (word) {
                word->dynamicCall("Quit()");
                delete word;
            }
            return false;
        }
    }

private:
    static void modifyThirdTable(QAxObject* table, QAxObject* document, const model::Contract& contract) {
        // Получаем количество строк в таблице
        int rowCount = table->property("Rows").querySubObject()->property("Count").toInt();

        // Добавляем новые строки в конец таблицы
        QAxObject* rows = table->querySubObject("Rows");

        // Добавляем pool_work.size() + 2 строк
        int newRowsCount = contract.pool_work.size() + 2;

        for (int i = 0; i < newRowsCount; ++i) {
            rows->dynamicCall("Add()");
        }

        // Обновляем количество строк
        rowCount = table->property("Rows").querySubObject()->property("Count").toInt();

        // Индексы новых строк (начинаем с последней добавленной)
        int startRow = rowCount - newRowsCount + 1;

        // 1. Первая добавленная строка - объединенная ячейка
        fillFirstRow(table, document, contract, startRow);

        // 2. Вторая добавленная строка - основные данные
        fillSecondRow(table, document, contract, startRow + 1);

        // 3. Строки для pool_work
        for (int i = 0; i < contract.pool_work.size(); ++i) {
            fillWorkRow(table, document, contract.pool_work[i],
                        startRow + 2 + i, i + 1);
        }
    }

    static void fillFirstRow(QAxObject* table, QAxObject* document,
                             const model::Contract& contract, int row) {
        // Объединяем ячейки в строке (с 1 по 7 колонку)
        QAxObject* range = table->querySubObject("Cell(int, int)", row, 1)
                               ->querySubObject("Range");

        QAxObject* endRange = table->querySubObject("Cell(int, int)", row, 7)
                                  ->querySubObject("Range");

        // Объединяем
        QAxObject* mergedRange = range->querySubObject("Merge(QAxObject*)",
                                                       endRange->asVariant());

        // Формируем текст
        QString text = QString::fromStdString(contract.name_organization_.value_or("")) +
                       " (" + QString::fromStdString(contract.name_short_.value_or("")) + ")";

        // Устанавливаем текст
        mergedRange->setProperty("Text", text);

        // Применяем форматирование
        applyFormatting(mergedRange, true, 12, "Times New Roman", Qt::yellow);

        delete endRange;
        delete range;
        delete mergedRange;
    }

    static void fillSecondRow(QAxObject* table, QAxObject* document,
                              const model::Contract& contract, int row) {
        // Получаем общее количество строк для нумерации
        int totalRows = table->property("Rows").querySubObject()->property("Count").toInt();

        // Заполняем каждую ячейку
        for (int col = 1; col <= 7; ++col) {
            QAxObject* cell = table->querySubObject("Cell(int, int)", row, col);
            QAxObject* range = cell->querySubObject("Range");

            QString text;
            bool bold = true;
            int alignment = 0; // 0 - left, 1 - center, 2 - right

            switch (col) {
            case 1: // Номер
                text = QString::number(totalRows - contract.pool_work.size() - 1);
                alignment = 0; // left
                break;

            case 2: // name_full_
                text = QString::fromStdString(contract.name_full_.value_or(""));
                alignment = 0; // left
                break;

            case 3: // Договор
                text = QString("Договор № %1 от %2")
                           .arg(QString::fromStdString(contract.number_.value_or("")))
                           .arg(formatDate(contract.date_));
                alignment = 1; // center
                break;

            case 4: // date_deadline_
                text = formatDate(contract.date_deadline_);
                alignment = 1; // center
                break;

            case 5: // name_responsible_employee_
                text = QString::fromStdString(contract.name_responsible_employee_.value_or(""));
                alignment = 1; // center
                break;

            default:
                text = "";
                break;
            }

            range->setProperty("Text", text);
            applyCellFormatting(range, bold, 12, "Times New Roman", alignment);

            delete range;
            delete cell;
        }
    }

    static void fillWorkRow(QAxObject* table, QAxObject* document,
                            const model::SeparateWork& work, int row, int workIndex) {
        for (int col = 1; col <= 7; ++col) {
            QAxObject* cell = table->querySubObject("Cell(int, int)", row, col);
            QAxObject* range = cell->querySubObject("Range");

            QString text;
            bool bold = false;
            int alignment = 0;

            switch (col) {
            case 1: // Номер с уровнем ниже
                text = QString::number(workIndex) + ".1";
                alignment = 0; // left
                break;

            case 2: // name_
                text = QString::fromStdString(work.name_);
                alignment = 0; // left
                break;

            case 5: // names_responsible_employees_
            {
                QStringList employees;
                for (const auto& emp : work.names_responsible_employees_) {
                    employees << QString::fromStdString(emp);
                }
                text = employees.join("\n");
                alignment = 1; // center
            }
            break;

            case 6: // date_deadline_
                text = formatDate(work.date_deadline_);
                alignment = 1; // center
                break;

            default:
                text = "";
                break;
            }

            range->setProperty("Text", text);
            applyCellFormatting(range, bold, 12, "Times New Roman", alignment);

            delete range;
            delete cell;
        }
    }

    static void applyFormatting(QAxObject* range, bool bold, int fontSize,
                                const QString& fontName, const QColor& bgColor = QColor()) {
        // Применяем форматирование шрифта
        QAxObject* font = range->querySubObject("Font");
        if (font) {
            font->setProperty("Name", fontName);
            font->setProperty("Size", fontSize);
            font->setProperty("Bold", bold);

            delete font;
        }

        // Применяем цвет фона
        if (bgColor.isValid()) {
            QAxObject* shading = range->querySubObject("Shading");
            if (shading) {
                shading->setProperty("BackgroundPatternColor",
                                     QVariant(bgColor.rgb()));
                delete shading;
            }
        }

        // Выравнивание по центру по вертикали
        QAxObject* paragraphs = range->querySubObject("Paragraphs");
        if (paragraphs) {
            paragraphs->setProperty("Alignment", 1); // wdAlignParagraphCenter
            delete paragraphs;
        }
    }

    static void applyCellFormatting(QAxObject* range, bool bold, int fontSize,
                                    const QString& fontName, int alignment) {
        // Применяем форматирование шрифта
        QAxObject* font = range->querySubObject("Font");
        if (font) {
            font->setProperty("Name", fontName);
            font->setProperty("Size", fontSize);
            font->setProperty("Bold", bold);
            delete font;
        }

        // Выравнивание по горизонтали
        QAxObject* paragraphs = range->querySubObject("Paragraphs");
        if (paragraphs) {
            // 0 - left, 1 - center, 2 - right, 3 - justify
            paragraphs->setProperty("Alignment", alignment);
            delete paragraphs;
        }

        // Выравнивание по вертикали в ячейке
        QAxObject* cell = range->querySubObject("Cells(1)");
        if (cell) {
            cell->setProperty("VerticalAlignment", 1); // wdCellAlignVerticalCenter
            delete cell;
        }
    }
};
*/

class WordDocxEditor {
public:
    static bool updateContractDocx(const QString& filePath, const model::Contract& contract) {
        QAxObject* word = nullptr;
        QAxObject* documents = nullptr;
        QAxObject* document = nullptr;

        try {
            // Создаем экземпляр Word
            word = new QAxObject("Word.Application");
            if (word->isNull()) {
                qDebug() << "Не удалось запустить Microsoft Word";
                return false;
            }

            // Делаем Word невидимым
            word->setProperty("Visible", false);

            // Получаем коллекцию документов
            documents = word->querySubObject("Documents");

            // Открываем документ
            document = documents->querySubObject("Open(const QString&)", filePath);
            if (!document || document->isNull()) {
                qDebug() << "Не удалось открыть документ:" << filePath;
                delete word;
                return false;
            }

            // Получаем коллекцию таблиц
            QAxObject* tables = document->querySubObject("Tables");
            if (!tables || tables->isNull()) {
                qDebug() << "В документе нет таблиц";
                document->dynamicCall("Close()");
                delete word;
                return false;
            }

            // Получаем количество таблиц
            int tableCount = tables->property("Count").toInt();
            if (tableCount < 3) {
                qDebug() << "В документе меньше 3 таблиц";
                document->dynamicCall("Close()");
                delete word;
                return false;
            }

            // Получаем третью таблицу (индекс 3 в Word, так как нумерация с 1)
            QAxObject* table = tables->querySubObject("Item(int)", 3);
            if (!table || table->isNull()) {
                qDebug() << "Не удалось получить третью таблицу";
                document->dynamicCall("Close()");
                delete word;
                return false;
            }

            // Добавляем новые строки
            modifyThirdTable(table, document, contract);

            // Сохраняем документ
            document->dynamicCall("Save()");

            // Закрываем документ
            document->dynamicCall("Close()");

            // Закрываем Word
            word->dynamicCall("Quit()");

            delete table;
            delete tables;
            delete document;
            delete documents;
            delete word;

            return true;

        } catch (const std::exception& e) {
            qDebug() << "Ошибка при работе с Word:" << e.what();

            // Очистка ресурсов
            if (document) {
                document->dynamicCall("Close()");
                delete document;
            }
            if (documents) delete documents;
            if (word) {
                word->dynamicCall("Quit()");
                delete word;
            }
            return false;
        }
    }

private:
    static void modifyThirdTable(QAxObject* table, QAxObject* document, const model::Contract& contract) {
        // Получаем объект Rows
        QAxObject* rows = table->querySubObject("Rows");
        if (!rows || rows->isNull()) {
            qDebug() << "Не удалось получить объект Rows";
            return;
        }

        // Получаем количество строк
        int rowCount = rows->property("Count").toInt();
        qDebug() << "Текущее количество строк в таблице:" << rowCount;

        // Добавляем новые строки в конец таблицы
        int newRowsCount = contract.pool_work.size() + 2;
        qDebug() << "Добавляем строк:" << newRowsCount;

            for (int i = 0; i < newRowsCount; ++i) {
            rows->dynamicCall("Add()");
        }

        // Обновляем количество строк
        rowCount = rows->property("Count").toInt();
        qDebug() << "Новое количество строк:" << rowCount;

        // Индексы новых строк (начинаем с последней добавленной)
        int startRow = rowCount - newRowsCount + 1;
        qDebug() << "Начальная строка для добавления:" << startRow;

        // 1. Первая добавленная строка - объединенная ячейка
        fillFirstRow(table, document, contract, startRow);

        // 2. Вторая добавленная строка - основные данные
        fillSecondRow(table, document, contract, startRow + 1, rowCount);

        // 3. Строки для pool_work
        for (int i = 0; i < contract.pool_work.size(); ++i) {
            fillWorkRow(table, document, contract.pool_work[i],
                        startRow + 2 + i, i + 1);
        }

        delete rows;
    }

    static void fillFirstRow(QAxObject* table, QAxObject* document,
                             const model::Contract& contract, int row) {
        qDebug() << "Заполняем первую строку (объединенную):" << row;

        // Получаем ячейки для объединения
        QAxObject* startCell = table->querySubObject("Cell(int, int)", row, 1);
        QAxObject* endCell = table->querySubObject("Cell(int, int)", row, 7);

        if (!startCell || !endCell) {
            qDebug() << "Не удалось получить ячейки для объединения";
            return;
        }

        QAxObject* startRange = startCell->querySubObject("Range");
        QAxObject* endRange = endCell->querySubObject("Range");

        if (!startRange || !endRange) {
            delete startCell;
            delete endCell;
            return;
        }

        // Объединяем ячейки
        QAxObject* mergedRange = startRange->querySubObject("Merge(QAxObject*)",
                                                            endRange->asVariant());

        if (mergedRange && !mergedRange->isNull()) {
            // Формируем текст
            QString text = QString::fromStdString(contract.name_organization_.value_or("")) +
                           " (" + QString::fromStdString(contract.name_short_.value_or("")) + ")";

            // Устанавливаем текст
            mergedRange->setProperty("Text", text);

            // Применяем форматирование
            applyFormatting(mergedRange, true, 12, "Times New Roman", Qt::yellow);

            delete mergedRange;
        }

        delete startRange;
        delete endRange;
        delete startCell;
        delete endCell;
    }

    static void fillSecondRow(QAxObject* table, QAxObject* document,
                              const model::Contract& contract, int row, int totalRows) {
        qDebug() << "Заполняем вторую строку (основные данные):" << row;

        // Заполняем каждую ячейку
        for (int col = 1; col <= 7; ++col) {
            QAxObject* cell = table->querySubObject("Cell(int, int)", row, col);
            if (!cell || cell->isNull()) {
                qDebug() << "Не удалось получить ячейку:" << row << col;
                continue;
            }

            QAxObject* range = cell->querySubObject("Range");
            if (!range || range->isNull()) {
                delete cell;
                continue;
            }

            QString text;
            bool bold = true;
            int alignment = 0; // 0 - left, 1 - center, 2 - right

            switch (col) {
            case 1: // Номер
                text = QString::number(totalRows - contract.pool_work.size() - 1);
                alignment = 0; // left
                break;

            case 2: // name_full_
                text = QString::fromStdString(contract.name_full_.value_or(""));
                alignment = 0; // left
                break;

            case 3: // Договор
                text = QString("Договор № %1 от %2")
                           .arg(QString::fromStdString(contract.number_.value_or("")))
                           .arg(formatDate(contract.date_));
                alignment = 1; // center
                break;

            case 4: // date_deadline_
                text = formatDate(contract.date_deadline_);
                alignment = 1; // center
                break;

            case 5: // name_responsible_employee_
                text = QString::fromStdString(contract.name_responsible_employee_.value_or(""));
                alignment = 1; // center
                break;

            default:
                text = "";
                break;
            }

            range->setProperty("Text", text);
            applyCellFormatting(range, bold, 12, "Times New Roman", alignment);

            delete range;
            delete cell;
        }
    }

    static void fillWorkRow(QAxObject* table, QAxObject* document,
                            const model::SeparateWork& work, int row, int workIndex) {
        qDebug() << "Заполняем строку работы" << workIndex << "в строке:" << row;

            for (int col = 1; col <= 7; ++col) {
            QAxObject* cell = table->querySubObject("Cell(int, int)", row, col);
            if (!cell || cell->isNull()) {
                continue;
            }

            QAxObject* range = cell->querySubObject("Range");
            if (!range || range->isNull()) {
                delete cell;
                continue;
            }

            QString text;
            bool bold = false;
            int alignment = 0;

            switch (col) {
            case 1: // Номер с уровнем ниже
                text = QString::number(workIndex) + ".1";
                alignment = 0; // left
                break;

            case 2: // name_
                text = QString::fromStdString(work.name_);
                alignment = 0; // left
                break;

            case 5: // names_responsible_employees_
            {
                QStringList employees;
                for (const auto& emp : work.names_responsible_employees_) {
                    employees << QString::fromStdString(emp);
                }
                text = employees.join("\n");
                alignment = 1; // center
            }
            break;

            case 6: // date_deadline_
                text = formatDate(work.date_deadline_);
                alignment = 1; // center
                break;

            default:
                text = "";
                break;
            }

            range->setProperty("Text", text);
            applyCellFormatting(range, bold, 12, "Times New Roman", alignment);

            delete range;
            delete cell;
        }
    }

    static void applyFormatting(QAxObject* range, bool bold, int fontSize,
                                const QString& fontName, const QColor& bgColor = QColor()) {
        if (!range || range->isNull()) return;

        // Применяем форматирование шрифта
        QAxObject* font = range->querySubObject("Font");
        if (font && !font->isNull()) {
            font->setProperty("Name", fontName);
            font->setProperty("Size", fontSize);
            font->setProperty("Bold", bold);

            delete font;
        }

        // Применяем цвет фона
        if (bgColor.isValid()) {
            QAxObject* shading = range->querySubObject("Shading");
            if (shading && !shading->isNull()) {
                // В Word цвет фона задается через RGB
                shading->setProperty("BackgroundPatternColor",
                                     QVariant(bgColor.rgb()));
                delete shading;
            }
        }

        // Выравнивание по центру по вертикали
        QAxObject* paragraphs = range->querySubObject("Paragraphs");
        if (paragraphs && !paragraphs->isNull()) {
            paragraphs->setProperty("Alignment", 1); // wdAlignParagraphCenter
            delete paragraphs;
        }
    }

    static void applyCellFormatting(QAxObject* range, bool bold, int fontSize,
                                    const QString& fontName, int alignment) {
        if (!range || range->isNull()) return;

        // Применяем форматирование шрифта
        QAxObject* font = range->querySubObject("Font");
        if (font && !font->isNull()) {
            font->setProperty("Name", fontName);
            font->setProperty("Size", fontSize);
            font->setProperty("Bold", bold);
            delete font;
        }

        // Выравнивание по горизонтали
        QAxObject* paragraphs = range->querySubObject("Paragraphs");
        if (paragraphs && !paragraphs->isNull()) {
            // 0 - left, 1 - center, 2 - right, 3 - justify
            paragraphs->setProperty("Alignment", alignment);
            delete paragraphs;
        }

        // Выравнивание по вертикали в ячейке
        QAxObject* cell = range->querySubObject("Cells(1)");
        if (cell && !cell->isNull()) {
            cell->setProperty("VerticalAlignment", 1); // wdCellAlignVerticalCenter
            delete cell;
        }
    }
};

// Основная функция для вызова
bool updateContractDocument(const QString& filePath, const model::Contract& contract) {
    return WordDocxEditor::updateContractDocx(filePath, contract);
}

#endif // ADD_CONTRACT_PLAN_MONTH_DOCX_H
