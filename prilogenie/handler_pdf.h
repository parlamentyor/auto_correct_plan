#pragma once

#include <QTextDocument>
#include <QTextCursor>
#include <QTextTable>
#include <QPrinter>
#include <QFile>

void CreatePdfReport() {
    QTextDocument *doc = new QTextDocument;
    QTextCursor cursor(doc);

    // 1. Настраиваем формат для заголовка
    QTextCharFormat titleFormat;
    titleFormat.setFontWeight(QFont::Bold);
    titleFormat.setFontPointSize(18);
    titleFormat.setForeground(Qt::darkBlue);

    // 2. Вставляем заголовок
    cursor.insertText("Отчет о работе приложения", titleFormat);
    cursor.insertBlock(); // Новая строка

    // 3. Вставляем обычный текст
    cursor.insertText("Этот документ был сгенерирован программой на Qt.");
    cursor.insertBlock();

    // 4. Создаем таблицу 3x2
    QTextTable *table = cursor.insertTable(3, 2);
    // Заполняем таблицу (код опущен для краткости)

    // 5. Сохраняем как PDF
    QPrinter printer(QPrinter::HighResolution);
    printer.setOutputFormat(QPrinter::PdfFormat);
    printer.setOutputFileName("output_report.pdf");
    doc->print(&printer);

    delete doc;
}
