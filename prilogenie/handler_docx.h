#ifndef HANDLER_DOCX_H
#define HANDLER_DOCX_H

#include "my_logger.h"
//#include "add_contract_plan_month_docx.h"

#include <QAxObject>
#include <QDir>
#include <QCoreApplication>
#include <QFileInfo>
#include <windows.h>
#include <string>
#include <filesystem>

static void ApplyMainCellFormatting(QAxObject* range, bool bold, int fontSize,
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
    LOG("Применили фарматирование шрифта");

    // Выравнивание по горизонтали
    QAxObject* paragraphs = range->querySubObject("Paragraphs");
    if (paragraphs && !paragraphs->isNull()) {
        // 0 - left, 1 - center, 2 - right, 3 - justify
        paragraphs->setProperty("Alignment", alignment);
        delete paragraphs;
    }
    LOG("Выравняли по горизонтали");

    // Выравнивание по вертикали в ячейке
    QAxObject* cell = range->querySubObject("Cells(1)");
    if (cell && !cell->isNull()) {
        cell->setProperty("VerticalAlignment", 1); // wdCellAlignVerticalCenter
        delete cell;
    }
    LOG("Выравняли по вертикали");
}

void CreateDocxWithWord() {

    // 1. Создаем папку "save" в месте расположения программы (используя только STL C++)
    std::string exePath;
    char buffer[MAX_PATH];
    GetModuleFileNameA(NULL, buffer, MAX_PATH);
    exePath = buffer;

    std::filesystem::path exeDir = std::filesystem::path(exePath).parent_path();
    std::filesystem::path saveFolder = exeDir / "save";

    // Создаем папку (STL filesystem)
    if (!std::filesystem::exists(saveFolder)) {
        std::filesystem::create_directory(saveFolder);
        LOG("Папка save создалась");
    }
    else {
        LOG("Папка save существует");
    }
    // Полный путь к файлу Word
    std::filesystem::path docPath = saveFolder / "document.docx";
    QString docPathQ = QString::fromStdWString(docPath.wstring());

    // Запускаем приложение Word
    QAxObject *wordApp = new QAxObject("Word.Application");

    // Проверка, запустился ли Word (установлен ли Office?)
    if (wordApp->isNull()) {
        // Обработка ошибки: "Microsoft Word не найден"
        LOG("Microsoft Word не найден");
        return;
    }
    else {
        LOG("Microsoft Word открылся");
    }

    // Делаем Word невидимым для пользователя (или можно поставить true для отладки)
    wordApp->setProperty("Visible", false);

    // Получаем коллекцию документов и добавляем новый
    QAxObject *documents = wordApp->querySubObject("Documents");
    QAxObject *document = documents->querySubObject("Add()");
    LOG("Документ Word создался");

    // Получаем объект PageSetup у документа
    QAxObject *pageSetup = document->querySubObject("PageSetup");

    if (pageSetup && !pageSetup->isNull())
    {
        // 2. Устанавливаем альбомную ориентацию (горизонтальную)
        // Используем строковое значение "wdOrientLandscape" [citation:1][citation:5]
        pageSetup->setProperty("Orientation", "wdOrientLandscape");
        LOG("Поменяли ориентацию страницы");

        // 3. Устанавливаем поля (значения в пунктах)
        // 1 дюйм = 72 пункта. Например, 72 пункта = 1 дюйм = 2.54 см ИТОГО cm * 72.0 / 2.54;
        // Верхнее поле: 2 см ≈ 56.7 пунктов
        pageSetup->setProperty("TopMargin", 56.7);

        // Нижнее поле: 2 см
        pageSetup->setProperty("BottomMargin", 56.7);

        // Левое поле: 3 см ≈ 85.0 пунктов
        pageSetup->setProperty("LeftMargin", 85.0);

        // Правое поле: 0.5 см ≈ 14.2 пунктов
        pageSetup->setProperty("RightMargin", 14.2);
        LOG("Установили значения полей страницы");
    }
    else {
        LOG("Не удалось получить объект PageSetup");
    }


    // Получаем текущее выделение (место, куда пойдет текст)
    QAxObject *selection = wordApp->querySubObject("Selection");
    LOG("Получили текущее место выделения");

    // Печатаем текст (используем dynamicCall для вызова методов COM)
    selection->dynamicCall("TypeText(const QString&)", "Привет из Qt приложения!");
    selection->dynamicCall("TypeParagraph()"); // Перевод строки
    selection->dynamicCall("TypeText(const QString&)", "Это вторая строка документа.");
    selection->dynamicCall("TypeParagraph()"); // Перевод строки
    selection->dynamicCall("TypeParagraph()"); // Перевод строки
    LOG("Сделали первые две строки");

    // 1. Явно получаем Range-объект из Selection
    // Это самый надежный способ получить "место" для вставки
    QAxObject* range = selection->querySubObject("Range");

    // Проверяем, что Range получен успешно
    if (!range || range->isNull()) {
        LOG("Не удалось получить Range из Selection");
        // Обработайте ошибку: удалите объекты и выйдите из функции
        return;
    }

    // 2. Создаем таблицу, используя Range
    // Приводим параметры к типам, понятным COM
    //Создаем таблицу 5 строк x 7 столбцов
    QAxObject* tables = document->querySubObject("Tables");
//    QAxObject* table = tables->querySubObject("Add(const QVariant&, int, int)",
//                                              selection->asVariant(), 1, 7);

    QAxObject* table = tables->querySubObject("Add(QAxObject*, int, int)",
                                              range->asVariant(), 1, 7); // 5 строк, 7 столбцов

    // 3. Проверяем, что таблица создана корректно
    if (!table || table->isNull()) {
        LOG("Не удалось создать таблицу");
        delete range;
        return;
    }

    // 4. Теперь можно применять автоформатирование (опционально)
    // Обратите внимание: свойство AutoFormat может быть недоступно в новых версиях Word
    // Более надежный способ — использовать стили таблиц:
    // table->dynamicCall("ApplyStyle(const QString&)", "Light Grid");

    // Если очень нужно использовать AutoFormat, попробуйте так:
    // table->dynamicCall("AutoFormat(int)", 1);  // 1 = wdAutoFormatSimple

    delete range;






    LOG("Создали таблицу");

    // Применяем автоформатирование таблицы
    if (table && !table->isNull())
    {
        table->setProperty("AutoFormat", 1); // wdAutoFormatSimple
        LOG("Применили автоформатирование таблицы");
    }
    else {
        LOG("Что-то не то с таблицей, не удаётся получить доступ к ней");
    }

    // Заполняем каждую ячейку
    for (int col = 1; col <= 7; ++col) {
        QAxObject* cell = table->querySubObject("Cell(int, int)", 1, col);
        if (!cell || cell->isNull()) {
            LOG("Не удалось получить ячейку: ", 1, ", ", col);
            continue;
        }

        QAxObject* range = cell->querySubObject("Range");
        if (!range || range->isNull()) {
            delete cell;
            continue;
        }

        QString text;
        bool bold = true;
        int alignment = 1; // 0 - left, 1 - center, 2 - right

        switch (col) {
        case 1: // Номер
            text = "№ п/п";
            break;

        case 2:
            text = "Наименование, этапы и содержание работ";
            break;

        case 3: // Договор
            text = "Договор";
            break;

        case 4:
            text = "Срок исполнения по договору";
            break;

        case 5:
            text = "Ответственный исполнитель";
            break;

        case 6:
            text = "Срок исполнения";
            break;

        case 7:
            text = "Отметка о выполнении. Состояние на начало месяца";
            break;

        default:
            text = "";
            break;
        }

        range->setProperty("Text", text);
        LOG("Добавили текст в ячейки");
        ApplyMainCellFormatting(range, bold, 12, "Times New Roman", alignment);
        LOG("Применили фарматирование в ячейках");

        delete range;
        delete cell;
    }

    // Сохраняем документ. Формат 16 = wdFormatDocumentDefault (.docx)
    //QString savePath = QDir::toNativeSeparators("C:/Temp/MyDocument.docx");
    document->dynamicCall("SaveAs(const QString&, int)", docPathQ, 16);
    LOG("Сохранили документ");

    // Закрываем документ и выгружаем Word
    document->dynamicCall("Close()");
    LOG("Закрыли документ");
    wordApp->dynamicCall("Quit()");
    LOG("Вышли из Word");

    delete wordApp;
}

#endif // HANDLER_DOCX_H
