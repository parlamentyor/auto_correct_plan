#ifndef HANDLER_DOCX_H
#define HANDLER_DOCX_H

#include <QAxObject>
#include <QDir>

void CreateDocxWithWord() {
    // Запускаем приложение Word
    QAxObject *wordApp = new QAxObject("Word.Application");

    // Проверка, запустился ли Word (установлен ли Office?)
    if (wordApp->isNull()) {
        // Обработка ошибки: "Microsoft Word не найден"
        qDebug() << "Microsoft Word не найден";
        return;
    }

    // Делаем Word невидимым для пользователя (или можно поставить true для отладки)
    wordApp->setProperty("Visible", false);

    // Получаем коллекцию документов и добавляем новый
    QAxObject *documents = wordApp->querySubObject("Documents");
    QAxObject *document = documents->querySubObject("Add()");

    // Получаем текущее выделение (место, куда пойдет текст)
    QAxObject *selection = wordApp->querySubObject("Selection");

    // Печатаем текст (используем dynamicCall для вызова методов COM)
    selection->dynamicCall("TypeText(const QString&)", "Привет из Qt приложения!");
    selection->dynamicCall("TypeParagraph()"); // Перевод строки
    selection->dynamicCall("TypeText(const QString&)", "Это вторая строка документа.");

    // Сохраняем документ. Формат 16 = wdFormatDocumentDefault (.docx)
    QString savePath = QDir::toNativeSeparators("C:/Temp/MyDocument.docx");
    document->dynamicCall("SaveAs(const QString&, int)", savePath, 16);

    // Закрываем документ и выгружаем Word
    document->dynamicCall("Close()");
    wordApp->dynamicCall("Quit()");

    delete wordApp;
}

#endif // HANDLER_DOCX_H
