QT       += core gui

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets charts

CONFIG += c++17

# You can make your code fail to compile if it uses deprecated APIs.
# In order to do so, uncomment the following line.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

SOURCES += \
    ChartAlgo.cpp \
    excelmanager.cpp \
    main.cpp \
    mainwindow.cpp

HEADERS += \
    Callback.h \
    Chart.h \
    ChartAlgo.h \
    CsvParser.h \
    Record.h \
    excelmanager.h \
    mainwindow.h

QXLSX_PARENTPATH=./         # current QXlsx path is . (. means curret directory)
QXLSX_HEADERPATH=./Libs/QXlsx/header/  # current QXlsx header path is ./header/
QXLSX_SOURCEPATH=./Libs/QXlsx/source/  # current QXlsx source path is ./source/
include(./Libs/QXlsx/QXlsx.pri)

# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target
