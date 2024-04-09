INCLUDEPATH += $$PWD
DEPENDPATH += $$PWD

!contains(DEFINES, KD_REPORTS) {
    include(../XReports/xreports.pri)
}

HEADERS += \
    $$PWD/listreport.h \
    $$PWD/report.h \
    $$PWD/reportbase.h \
    $$PWD/reportdoc.h \
    $$PWD/reportdoctemplate.h \
    $$PWD/reporttypes.h \
    $$PWD/reportutils.h \
    $$PWD/simplereport.h

SOURCES +=

contains(DEFINES, KD_REPORTS) {
    HEADERS += \
        $$PWD/kdxmlreport.h

    SOURCES +=
}

