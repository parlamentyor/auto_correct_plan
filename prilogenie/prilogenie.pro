QT += core gui
QT += printsupport
QT += axcontainer

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

CONFIG += c++17

# You can make your code fail to compile if it uses deprecated APIs.
# In order to do so, uncomment the following line.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

SOURCES += \
    addstage.cpp \
    app.cpp \
    authorization.cpp \
    contractmodel.cpp \
    contractview.cpp \
    handler_odt.cpp \
    main.cpp \
    mainwindow.cpp \
    model.cpp \
    serialization_qt_json.cpp \
    windowmanager.cpp \
    workwindow.cpp

HEADERS += \
    add_contract_plan_month_docx.h \
    addstage.h \
    contractmodel.h \
    contractview.h \
    general_functions.h \
    handler_add_work.h \
    app.h \
    authorization.h \
    magic_defs.h \
    mainwindow.h \
    handler_odt.h \
    handler_pdf.h \
    handler_docx.h \
    model.h \
    my_logger.h \
    serialization_qt_json.h \
    windowmanager.h \
    workwindow.h

FORMS += \
    addstage.ui \
    authorization.ui \
    mainwindow.ui \
    workwindow.ui

# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target
