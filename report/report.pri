INCLUDEPATH += $$PWD
DEPENDPATH += $$PWD

HEADERS += \
    $$PWD/listreport.h \
    $$PWD/simplereport.h

SOURCES += \
    $$PWD/listreport.cpp \
    $$PWD/simplereport.cpp

contains(DEFINES, KD_REPORTS) {
    HEADERS += \
        $$PWD/kdsimplereport.h \
        $$PWD/kdxmlreport.h

    SOURCES += \
        $$PWD/kdsimplereport.cpp \
        $$PWD/kdxmlreport.cpp
}
