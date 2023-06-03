QT += core gui sql xml printsupport

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

CONFIG += c++11

# Use KD Reports library
#DEFINES += KD_REPORTS

# The following define makes your compiler emit warnings if you use
# any Qt feature that has been marked deprecated (the exact warnings
# depend on your compiler). Please consult the documentation of the
# deprecated API in order to know how to port your code away from it.
DEFINES += QT_DEPRECATED_WARNINGS

# You can also make your code fail to compile if it uses deprecated APIs.
# In order to do so, uncomment the following line.
# You can also select to disable deprecated APIs only up to a certain version of Qt.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

TARGET = QtSqlView

include("database/database.pri")
include("report/report.pri")
include("ext/ext.pri")
include("syntax/syntax.pri")

SOURCES += \
    ConnectionDlg.cpp \
    MainWindow.cpp \
    QueryParamDlg.cpp \
    TableHeadersDlg.cpp \
    main.cpp

HEADERS += \
    ConnectionDlg.h \
    MainWindow.h \
    QueryParamDlg.h \
    TableHeadersDlg.h

FORMS += \
    ConnectionDlg.ui \
    MainWindow.ui

# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target

contains(DEFINES, KD_REPORTS) {
    win32:CONFIG(release, debug|release): LIBS += -L$$PWD/../KDReports-2.2.1/lib/release/ -lkdreports
    else:win32:CONFIG(debug, debug|release): LIBS += -L$$PWD/../KDReports-2.2.1/lib/debug/ -lkdreports
    else:unix: LIBS += -L/usr/local/KDAB/KDReports-2.2.1/lib/ -lkdreports

    INCLUDEPATH += /usr/local/KDAB/KDReports-2.2.1/include/KDReports
    DEPENDPATH += /usr/local/KDAB/KDReports-2.2.1/include/KDReports
}

RESOURCES += \
    QtSqlView.qrc

RC_FILE = resources.rc
