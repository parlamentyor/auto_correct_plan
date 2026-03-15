QT += core gui
QT += printsupport
QT += axcontainer

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

CONFIG += c++17

# You can make your code fail to compile if it uses deprecated APIs.
# In order to do so, uncomment the following line.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

SOURCES += \
    app.cpp \
    authorization.cpp \
    handler_odt.cpp \
    main.cpp \
    mainwindow.cpp \
    model.cpp \
    windowmanager.cpp \
    workwindow.cpp

HEADERS += \
    handler_add_work.h \
    app.h \
    authorization.h \
    magic_defs.h \
    mainwindow.h \
    handler_odt.h \
    handler_pdf.h \
    handler_docx.h \
    model.h \
    windowmanager.h \
    workwindow.h

FORMS += \
    authorization.ui \
    mainwindow.ui \
    workwindow.ui

# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target
