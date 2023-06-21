#ifndef REPORTTYPES_H
#define REPORTTYPES_H

#include <QString>
#include <QStringList>

namespace Report {

/******************************************************************/

enum ReportType {
    FOLDER, LIST, KD_XML
};

/******************************************************************/

inline QStringList reportTypeList() {
    return QString("FOLDER,LIST,KD_XML").split(",");
}

/******************************************************************/

enum ParamType {
    Null, String, Integer, Real, Date, DateTime, Ref
};

/******************************************************************/

inline QStringList paramTypeList() {
    return QString("Null,String,Integer,Real,Date,DateTime,Ref").split(",");
}

/******************************************************************/

} // namespace Report

#endif // REPORTTYPES_H
