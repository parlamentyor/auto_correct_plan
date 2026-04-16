#ifndef ADD_CONTRACT_PLAN_MONTH_DOCX_H
#define ADD_CONTRACT_PLAN_MONTH_DOCX_H

#include <model.h>
#include <handler_add_work.h>
#include <my_logger.h>

#include <QString>
#include <QAxObject>
//#include <QDebug>
//#include <QColor>
#include <QVariant>
#include <optional>
#include <vector>
#include <string>

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
                LOG("Не удалось запустить Microsoft Word");
                return false;
            }

            // Делаем Word невидимым
            word->setProperty("Visible", false);

            // Получаем коллекцию документов
            documents = word->querySubObject("Documents");

            // Открываем документ
            document = documents->querySubObject("Open(const QString&)", filePath);
            if (!document || document->isNull()) {
                LOG("Не удалось открыть документ:", filePath.toStdString());
                delete word;
                return false;
            }

            // Получаем коллекцию таблиц
            QAxObject* tables = document->querySubObject("Tables");
            if (!tables || tables->isNull()) {
                LOG("В документе нет таблиц");
                document->dynamicCall("Close()");
                delete word;
                return false;
            }

            // Получаем количество таблиц
            int tableCount = tables->property("Count").toInt();
            if (tableCount < 3) {
                LOG("В документе меньше 3 таблиц");
                document->dynamicCall("Close()");
                delete word;
                return false;
            }

            // Получаем третью таблицу (индекс 3 в Word, так как нумерация с 1)
            QAxObject* table = tables->querySubObject("Item(int)", 3);
            if (!table || table->isNull()) {
                LOG("Не удалось получить третью таблицу");
                document->dynamicCall("Close()");
                delete word;
                return false;
            }

            // Добавляем новые строки
            modifyThirdTable(table, contract);

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
            LOG("Ошибка при работе с Word:", e.what());

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


    static void modifyThirdTable(QAxObject* table, const model::Contract& contract) {
        // Получаем объект Rows
        QAxObject* rows = table->querySubObject("Rows");
        if (!rows || rows->isNull()) {
            qDebug() << "Не удалось получить объект Rows";
            LOG("Не удалось получить объект Rows");
            return;
        }

        // Получаем количество строк
        int rowCount = rows->property("Count").toInt();
        LOG("Текущее количество строк в таблице: ", rowCount);

        // Добавляем новые строки в конец таблицы
        int newRowsCount = contract.pool_work.size() + 2;
        LOG("Добавляем строк: ", newRowsCount);

            for (int i = 0; i < newRowsCount; ++i) {
            rows->dynamicCall("Add()");
        }

        // Обновляем количество строк
        rowCount = rows->property("Count").toInt();
        LOG("Новое количество строк: ", rowCount);

        // Индексы новых строк (начинаем с последней добавленной)
        int startRow = rowCount - newRowsCount + 1;
        qDebug() << "Начальная строка для добавления:" << startRow;

        // 1. Первая добавленная строка - объединенная ячейка
        fillFirstRow(table, contract, startRow);

        // 2. Вторая добавленная строка - основные данные
        fillSecondRow(table, contract, startRow + 1, rowCount);

        // 3. Строки для pool_work
        for (int i = 0; i < contract.pool_work.size(); ++i) {
            fillWorkRow(table, contract.pool_work[i],
                        startRow + 2 + i, i + 1);
        }

        delete rows;
    }

private:
    static void fillFirstRow(QAxObject* table, const model::Contract& contract, int row) {
        LOG("Заполняем первую строку (объединенную): ", row);

        // Получаем ячейки для объединения
        QAxObject* startCell = table->querySubObject("Cell(int, int)", row, 1);
        QAxObject* endCell = table->querySubObject("Cell(int, int)", row, 7);

        if (!startCell || !endCell) {
            LOG("Не удалось получить ячейки для объединения");
            return;
        }

        // Объединяем ячейки первой добавленной строки
        startCell->querySubObject("Merge(const QVariant&)", endCell->asVariant());

        // Теперь startCell содержит объединенную ячейку
        // endCell больше не существует (стал частью объединенной ячейки)

        // Работаем с объединенной ячейкой
        QAxObject* mergedCell = table->querySubObject("Cell(int, int)", row, 1);
        QAxObject* mergedRange = mergedCell->querySubObject("Range");

        QAxObject* shadingCell = mergedCell->querySubObject("Shading");
        // Используем RGB значение для цвета
        // Зелёный, Акцент 6, более светлый оттенок 80% ~ RGB(183, 215, 168)
        shadingCell->setProperty("BackgroundPatternColor", /*12048183*/ 0xFF0000); // 0xB7D7A8 в десятичной // Word ожидает BGR порядок: (Blue << 16) | (Green << 8) | Red тогда 11032759
//        shadingCell->setProperty("Texture", 0); // wdTextureNone - сплошная заливка

        // Формируем текст
        QString text = QString::fromStdString(contract.name_organization_.value_or("")) +
                       " (" + QString::fromStdString(contract.name_short_.value_or("")) + ")";
        LOG("Сформированный текст для вставки в объеденённую ячейку: ", text.toStdString());

        // Устанавливаем текст
        mergedRange->setProperty("Text", text);
        LOG("Текст, по идеи, должен добавиться");

        // Применяем форматирование
        applyFormatting(mergedRange, true, 12, "Times New Roman", /*Qt::yellow*/65535); // Yellow RGB(255,255,0) = 65535 - нифига это не жёлтый
        LOG("Текст, по идеи, должен отформатироваться");

        delete shadingCell;
        delete mergedRange;
        delete mergedCell;
        delete startCell;
        delete endCell;
    }

    static void fillSecondRow(QAxObject* table, const model::Contract& contract, int row, int totalRows) {
        LOG("Заполняем вторую строку (основные данные): ", row);

        // Заполняем каждую ячейку
        for (int col = 1; col <= 7; ++col) {
            QAxObject* cell = table->querySubObject("Cell(int, int)", row, col);
            if (!cell || cell->isNull()) {
                LOG("Не удалось получить ячейку: ", row, ", ", col);
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

            case 7: // info_
                text = QString::fromStdString(contract.info_.value_or(""));
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

    static void fillWorkRow(QAxObject* table, const model::SeparateWork& work, int row, int workIndex) {
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

            case 7: // info_
                text = QString::fromStdString(work.info_.value_or(""));
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
                                const QString& fontName, /*const QColor& bgColor = QColor()*/ int bgColor) {
        if (!range || range->isNull()) return;

        // Применяем форматирование шрифта
        QAxObject* font = range->querySubObject("Font");
        if (font && !font->isNull()) {
            font->setProperty("Name", fontName);
            font->setProperty("Size", fontSize);
            font->setProperty("Bold", bold);

            LOG("Текст в объединенной ячейке должен отфартатироваться");
            delete font;
        }

        // Так мы задаём цвет подложки текста
        QAxObject* shading = range->querySubObject("Shading");
        if (shading && !shading->isNull()) {
            // В Word цвет фона задается через RGB
            shading->setProperty("BackgroundPatternColor",
                                 /*QVariant(bgColor.rgb())*/ bgColor);
        }

        delete shading;

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
inline bool updateContractDocument(const QString& filePath, const model::Contract& contract) {
    return WordDocxEditor::updateContractDocx(filePath, contract);
}

#endif // ADD_CONTRACT_PLAN_MONTH_DOCX_H
