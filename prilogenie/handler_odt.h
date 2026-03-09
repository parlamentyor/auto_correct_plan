#ifndef HANDLER_ODT_H
#define HANDLER_ODT_H

#include <QTextDocument>
#include <QTextCursor>
#include <QTextTable>
#include <QTextTableFormat>
#include <QTextCharFormat>
#include <QTextBlockFormat>
#include <QFile>
#include <QTextDocumentWriter>
#include <QApplication>
#include <QDebug>

// Для работы с датами и временем
#include <QDate>        // QDate - для работы с датами
#include <QTime>        // QTime - для работы с временем
#include <QDateTime>    // QDateTime - для работы с датой и временем вместе

// Для работы с файловой информацией
#include <QFileInfo>    // QFileInfo - информация о файлах

void CreateOdtWithTable() {
    // Создаем документ
    QTextDocument *doc = new QTextDocument;
    QTextCursor cursor(doc);

    // 1. Настраиваем формат для заголовка документа
    QTextCharFormat titleFormat;
    titleFormat.setFontWeight(QFont::Bold);
    titleFormat.setFontPointSize(16);
    titleFormat.setForeground(Qt::darkBlue);

    QTextBlockFormat centerAlign;
    centerAlign.setAlignment(Qt::AlignCenter);

    cursor.insertBlock(centerAlign, titleFormat);
    cursor.insertText("Таблица данных отчета");
    cursor.insertBlock();
    cursor.insertBlock();

    // 2. Возвращаем выравнивание влево для таблицы
    QTextBlockFormat leftAlign;
    leftAlign.setAlignment(Qt::AlignLeft);
    cursor.insertBlock(leftAlign);

    // 3. Создаем таблицу 3x2
    QTextTable *table = cursor.insertTable(3, 2);

    // 4. Настраиваем формат таблицы
    QTextTableFormat tableFormat = table->format();
    tableFormat.setBorder(1);
    tableFormat.setBorderStyle(QTextFrameFormat::BorderStyle_Solid);
    tableFormat.setCellSpacing(0);
    tableFormat.setCellPadding(5);
    tableFormat.setWidth(QTextLength(QTextLength::PercentageLength, 80)); // 80% ширины страницы
    table->setFormat(tableFormat);

    // 5. Настраиваем формат для заголовков ячеек
    QTextCharFormat headerFormat;
    headerFormat.setFontWeight(QFont::Bold);
    headerFormat.setBackground(Qt::lightGray);
    headerFormat.setForeground(Qt::black);

    QTextBlockFormat headerAlign;
    headerAlign.setAlignment(Qt::AlignCenter);

    // 6. Настраиваем формат для данных
    QTextCharFormat dataFormat;
    dataFormat.setFontPointSize(11);

    QTextBlockFormat dataLeftAlign;
    dataLeftAlign.setAlignment(Qt::AlignLeft);

    QTextBlockFormat dataRightAlign;
    dataRightAlign.setAlignment(Qt::AlignRight);

    // 7. ЗАПОЛНЯЕМ ТАБЛИЦУ
    // Строка 0 - Заголовки
    QTextCursor cellCursor = table->cellAt(0, 0).firstCursorPosition();
    cellCursor.setBlockFormat(headerAlign);
    cellCursor.setBlockCharFormat(headerFormat);
    cellCursor.insertText("Параметр");

    cellCursor = table->cellAt(0, 1).firstCursorPosition();
    cellCursor.setBlockFormat(headerAlign);
    cellCursor.setBlockCharFormat(headerFormat);
    cellCursor.insertText("Значение");

    // Строка 1 - Данные 1
    cellCursor = table->cellAt(1, 0).firstCursorPosition();
    cellCursor.setBlockFormat(dataLeftAlign);
    cellCursor.setBlockCharFormat(dataFormat);
    cellCursor.insertText("Версия приложения");

    cellCursor = table->cellAt(1, 1).firstCursorPosition();
    cellCursor.setBlockFormat(dataLeftAlign);
    cellCursor.setBlockCharFormat(dataFormat);
    cellCursor.insertText("1.0.5");

    // Строка 2 - Данные 2
    cellCursor = table->cellAt(2, 0).firstCursorPosition();
    cellCursor.setBlockFormat(dataLeftAlign);
    cellCursor.setBlockCharFormat(dataFormat);
    cellCursor.insertText("Дата сборки");

    cellCursor = table->cellAt(2, 1).firstCursorPosition();
    cellCursor.setBlockFormat(dataLeftAlign);
    cellCursor.setBlockCharFormat(dataFormat);

    // Вставляем текущую дату
    QDate currentDate = QDate::currentDate();
    cellCursor.insertText(currentDate.toString("dd.MM.yyyy"));

    // 8. Добавляем еще один блок текста после таблицы
    cursor.movePosition(QTextCursor::End);
    cursor.insertBlock();

    QTextCharFormat noteFormat;
    noteFormat.setFontItalic(true);
    noteFormat.setForeground(Qt::darkGray);
    noteFormat.setFontPointSize(9);

    QTextBlockFormat noteAlign;
    noteAlign.setAlignment(Qt::AlignRight);

    cursor.insertBlock(noteAlign, noteFormat);
    cursor.insertText("* Таблица создана автоматически " +
                      QDateTime::currentDateTime().toString("dd.MM.yyyy hh:mm"));

    // 9. СОХРАНЯЕМ ДОКУМЕНТ В ФОРМАТЕ ODT
    QString fileName = "output_document.odt";

    QTextDocumentWriter writer;
    writer.setFormat("ODF"); // OpenDocument Format
    writer.setFileName(fileName);

    bool success = writer.write(doc);

    if (success) {
        qDebug() << "Документ успешно сохранен как:" << fileName;
        qDebug() << "Путь к файлу:" << QFileInfo(fileName).absoluteFilePath();
    } else {
        qDebug() << "Ошибка при сохранении документа!";
    }

    // Очистка
    delete doc;
}

#endif // HANDLER_ODT_H
