#ifndef HANDLER_DOCX_H
#define HANDLER_DOCX_H

#include "my_logger.h"
#include "add_contract_plan_month_docx.h"

#include <QAxObject>
#include <QDir>
#include <QCoreApplication>
#include <QFileInfo>
#include <windows.h>
#include <string>
#include <filesystem>

// Создаем папку "save", если ее нет, и создаём путь к файлу
QString CreatePathDokument() {
    // 1. Создаем папку "save" в месте расположения программы
    std::string exe_path;
    char buffer[MAX_PATH];
    GetModuleFileNameA(NULL, buffer, MAX_PATH);
    exe_path = buffer;

    std::filesystem::path exe_dir = std::filesystem::path(exe_path).parent_path();
    std::filesystem::path save_folder = exe_dir / "save";

    // Создаем папку (STL filesystem)
    if (!std::filesystem::exists(save_folder)) {
        std::filesystem::create_directory(save_folder);
        LOG("Папка save создалась");
    }
    else {
        LOG("Папка save существует");
    }
    // Полный путь к файлу Word
    std::filesystem::path doc_path = save_folder / "document.docx";
    return QString::fromStdWString(doc_path.wstring());
}

// Функция для перевода сантиметров в пункты
double CmToPoints(double cm)
{
    // 1 дюйм = 72 пункта. Например, 72 пункта = 1 дюйм = 2.54 см ИТОГО cm * 72.0 / 2.54;
    return cm * 72.0 / 2.54;
}

// Функция установки свойств страницы
void SetPageSetup(QAxObject* page_setup) {
    // Устанавливаем альбомную ориентацию (горизонтальную)
    page_setup->setProperty("Orientation", "wdOrientLandscape");
    LOG("Поменяли ориентацию страницы");

    // Верхнее поле: 2 см ≈ 56.7 пунктов
    page_setup->setProperty("TopMargin", CmToPoints(2.0));
    // Нижнее поле: 2 см
    page_setup->setProperty("BottomMargin", CmToPoints(2.0));
    // Левое поле: 3 см ≈ 85.0 пунктов
    page_setup->setProperty("LeftMargin", CmToPoints(3.0));
    // Правое поле: 0.5 см ≈ 14.2 пунктов
    page_setup->setProperty("RightMargin", CmToPoints(0.5));
    LOG("Установили значения полей страницы");
    // можно проверить значения установленных свойств и сравнить с тем, что хотел сделать для проверки
    // и вернуть bool....но это потом
}

void ApplyMainCellFormatting(QAxObject* range, bool bold, int font_size,
                                    const QString& font_name, int alignment) {
    if (!range || range->isNull()) return;

    // Применяем форматирование шрифта
    QAxObject* font = range->querySubObject("Font");
    if (font && !font->isNull()) {
        font->setProperty("Name", font_name);
        font->setProperty("Size", font_size);
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

void SetTextMainCells(QAxObject* table) {
    // Заполняем каждую ячейку заголовка
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
        LOG("Применили фарматирование в ячейках заголовка");

        delete range;
        delete cell;
    }
}

// Функция установки границ в таблице
void SetBordersTable(QAxObject* borders) {
    // Типы границ (wdBorderType):
    // wdBorderTop = 1 (верхняя)
    // wdBorderLeft = 2 (левая)
    // wdBorderBottom = 3 (нижняя)
    // wdBorderRight = 4 (правая)
    // wdBorderHorizontal = 5 (внутренние горизонтальные)
    // wdBorderVertical = 6 (внутренние вертикальные)

    QList<int> border_types = {1, 2, 3, 4, 5, 6};

    for (int border_type : border_types) {
        // Получаем отдельную границу по её типу
        QAxObject* border = borders->querySubObject("Item(int)", border_type);

        if (border && !border->isNull()) {
            // Шаг 1: явно делаем границу видимой
            border->setProperty("Visible", true);

            // Шаг 2: устанавливаем стиль линии - сплошная (wdLineStyleSingle = 1)
            border->setProperty("LineStyle", 1);

            // Шаг 3: устанавливаем толщину 0.5 pt (wdLineWidth050pt = 4)
            border->setProperty("LineWidth", 4);

            // Шаг 4: устанавливаем чёрный цвет (wdColorBlack = 0)
            border->setProperty("Color", 0);

            QVariant visible = border->property("Visible");
            QVariant line_style = border->property("LineStyle");
            qDebug() << "Border visible:" << visible << "LineStyle:" << line_style;

            delete border;
        }
    }
}

// Функция установки свойств таблицы
void SetPropertyTable(QAxObject* table) {
    // Применяем автоформатирование таблицы
    table->setProperty("AutoFormat", 1); // 1 = wdAutoFormatSimple
    //    table->dynamicCall("AutoFormat(int)", 1); // если что, то можно и так писать
    // Свойство AutoFormat может быть недоступно в новых версиях Word
    // Более надежный способ — использовать стили таблиц:
    // table->dynamicCall("ApplyStyle(const QString&)", "Light Grid");

    // Устанавливаем автоподбор ширины по содержимому
    table->dynamicCall("AutoFitBehavior(int)", 1);  // 1 = wdAutoFitContent
    // Если нужно также подогнать таблицу по ширине окна (растянуть на всю ширину) 2 = wdAutoFitWindow
    // Если нужно также установить фиксированную ширину (по умолчанию) 0 = wdAutoFitFixed

    LOG("Применили автоформатирование таблицы");

    //--------------------------------------------------------------------------------------------
    // Получаем объект Table и отключаем обтекание
    QAxObject* tableRange = table->querySubObject("Range");
    if (tableRange && !tableRange->isNull()) {
        QAxObject* paragraphFormat = tableRange->querySubObject("ParagraphFormat");
        if (paragraphFormat && !paragraphFormat->isNull()) {
            // Устанавливаем выравнивание и отключаем обтекание
            paragraphFormat->setProperty("Alignment", 1); // wdAlignParagraphLeft = 1
            delete paragraphFormat;
        }
        delete tableRange;
    }

    // Устанавливаем обтекание как "In line with text" (wdWrapInline = 0)
    table->setProperty("WrapAroundText", false); // или table->setProperty("WrapAroundText", 0);
    QVariant wrap_around_text = table->property("WrapAroundText");
    qDebug() << "wrap_around_text:" << wrap_around_text;
    //------------------------------------------------------------------------------------------------------------
}

void SetRepeteMainRow(QAxObject* table) {
    // 1. НАСТРАИВАЕМ ПОВТОРЕНИЕ ПЕРВОЙ СТРОКИ НА ВСЕХ СТРАНИЦАХ
    // Это свойство нужно установить ДО добавления новых строк,
    // чтобы Word корректно применил форматирование ко всем строкам заголовка.

    // это ко всем существующим строкам......неработает

    QAxObject* rows = table->querySubObject("Rows");
    if (rows && !rows->isNull())
    {
        // Устанавливаем HeadingFormat в true (1), чтобы первая строка повторялась [citation:6]
        QAxObject* firstRow = table->querySubObject("Rows(int)", 1);
        firstRow->dynamicCall("Select()");
        //        rows->setProperty("HeadingFormat", true);
        rows->dynamicCall("HeadingFormat", 1);
        QVariant heading_format = rows->property("HeadingFormat");
        qDebug() << "heading_format_call:" << heading_format;
        delete firstRow;
        delete rows;
    }

    /*
    // к конкретной строке...тоже не работает
    // Шаг 3: Получаем первую строку
    QAxObject* firstRow = table->querySubObject("Rows(int)", 1);
    if (firstRow && !firstRow->isNull()) {
        // Шаг 4: Пробуем setProperty с boolean значением
        firstRow->dynamicCall("Select()");
        bool success = firstRow->setProperty("HeadingFormat", true);
        QVariant heading_format = firstRow->property("HeadingFormat");
        qDebug() << "heading_format_set:" << heading_format;
        if (!success) {
            // Если setProperty не сработал, пробуем dynamicCall
            firstRow->dynamicCall("HeadingFormat", true);
            QVariant heading_format = firstRow->property("HeadingFormat");
            qDebug() << "heading_format_call:" << heading_format;
        }

        delete firstRow;
    }
*/

    /*
    // эмулировать действие пользователя через интерфейс:
    // Выделяем первую строку
    QAxObject* firstRow = table->querySubObject("Rows(int)", 1);
    if (firstRow && !firstRow->isNull()) {
        firstRow->dynamicCall("Select()");
        delete firstRow;
    }

    // Получаем выделение и пытаемся установить свойство через Selection
    QAxObject* selection1 = document->querySubObject("Selection");
    if (selection1 && !selection1->isNull()) {
        QAxObject* selectedRows = selection1->querySubObject("Rows");
        if (selectedRows && !selectedRows->isNull()) {
            selectedRows->dynamicCall("HeadingFormat", true);
            QVariant heading_format = selectedRows->property("HeadingFormat");
            qDebug() << "heading_format_call:" << heading_format;
            delete selectedRows;
        }
        delete selection1;
    }
*/
}

// Функция создания таблицы
void CraeteTable(QAxObject* selection, QAxObject* document, const std::vector<model::Contract>& contracts) {
    // Явно получаем Range-объект из Selection
    // Это самый надежный способ получить "место" для вставки
    QAxObject* range = selection->querySubObject("Range");
    if (!range || range->isNull()) {
        LOG("Не удалось получить Range из Selection");
        return;
    }

    // Создаем таблицу, используя Range
    QAxObject* tables = document->querySubObject("Tables");
    QAxObject* table = tables->querySubObject("Add(QAxObject*, int, int)",
                                              range->asVariant(), 1, 7); // 1 строку, 7 столбцов
    if (!table || table->isNull()) {
        LOG("Не удалось создать таблицу");
        delete range;
        return;
    }

    delete range;
    LOG("Создали таблицу");

    // Устанавливаем свойства таблицы
    SetPropertyTable(table);

    // Пишем текст в ячейках заголовка и форматируем их
    SetTextMainCells(table);

    SetRepeteMainRow(table);

    // Шаг 5: Теперь можно добавлять строки
    QAxObject* rowsCollection = table->querySubObject("Rows");
    if (rowsCollection && !rowsCollection->isNull()) {
        for (int i = 0; i < 30; ++i) {
            rowsCollection->dynamicCall("Add()");
        }
        delete rowsCollection;
    }

    for (const auto& contract : contracts) {
        WordDocxEditor::modifyThirdTable(table, contract);
    }

    // Получаем объект Borders для таблицы (для установки границ в таблице)
    QAxObject* borders = table->querySubObject("Borders");
    if (borders && !borders->isNull()) {
        SetBordersTable(borders);
        delete borders;
    }
    else {
        LOG("Не удалось получить доступ к границам таблицы");
    }
}

void CreateDocxWithWord(const std::vector<model::Contract>& contracts) {

    QString doc_path_qstr = CreatePathDokument();

    // Запускаем приложение Word
    QAxObject *word_app = new QAxObject("Word.Application");

    // Проверка, запустился ли Word (установлен ли Office?)
    if (word_app->isNull()) {
        LOG("Microsoft Word не найден");
        return;
    }
    else {
        LOG("Microsoft Word открылся");
    }

    // Делаем Word невидимым для пользователя (или можно поставить true для отладки)
    word_app->setProperty("Visible", false);

    // Получаем коллекцию документов и добавляем новый
    QAxObject *documents = word_app->querySubObject("Documents");
    QAxObject *document = documents->querySubObject("Add()");
    LOG("Документ Word создался");

    // Устанавливаем свойства страницы
    QAxObject *page_setup = document->querySubObject("PageSetup");
    if (page_setup && !page_setup->isNull()) {
        SetPageSetup(page_setup);
        delete page_setup;
    }
    else {
        LOG("Не удалось получить объект PageSetup");
    }

    // Получаем текущее выделение (место, куда пойдет текст)
    QAxObject *selection = word_app->querySubObject("Selection");
    LOG("Получили текущее место выделения");

    // Печатаем текст (используем dynamicCall для вызова методов COM)
    selection->dynamicCall("TypeText(const QString&)", "Привет из Qt приложения!");
    selection->dynamicCall("TypeParagraph()"); // Перевод строки
    selection->dynamicCall("TypeText(const QString&)", "Это вторая строка документа.");
    selection->dynamicCall("TypeParagraph()"); // Перевод строки
    selection->dynamicCall("TypeParagraph()"); // Перевод строки
    LOG("Сделали первые две строки");

    CraeteTable(selection, document, contracts);

    delete selection;

    // Сохраняем документ. Формат 16 = wdFormatDocumentDefault (.docx)
    //QString savePath = QDir::toNativeSeparators("C:/Temp/MyDocument.docx");
    document->dynamicCall("SaveAs(const QString&, int)", doc_path_qstr, 16);
    LOG("Сохранили документ");

    // Закрываем документ и выгружаем Word
    document->dynamicCall("Close()");
    LOG("Закрыли документ");
    word_app->dynamicCall("Quit()");
    LOG("Вышли из Word");

    delete document;
    delete documents;
    delete word_app;
}

#endif // HANDLER_DOCX_H
