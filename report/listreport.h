#ifndef LISTREPORT_H
#define LISTREPORT_H

#include "reportbase.h"
#include "xcsvmodel.h"

class ListReport : public Report::ReportBase
{
    Q_OBJECT
public:
    explicit ListReport(QObject *parent = nullptr)
        : Report::ReportBase(parent)
        , m_Model(Q_NULLPTR)
        , b_WithHeaders(true)
    { }

    void setModel(QAbstractItemModel *model) {
        m_Model = model;
    }

    void setWithHeaders(bool withHeaders) {
        b_WithHeaders = withHeaders;
    }

    virtual bool toCsvFile(const QString& fileName) const {
        XCsvModel csvModel;
        csvModel.importFromModel(m_Model);
        csvModel.toCSV(fileName, b_WithHeaders);
        return true;
    }

    virtual QSharedPointer<QTextDocument> toTextDocument() const {
        XReport report;
        createReport(&report);
        QSharedPointer<QTextDocument> result(report.mainTextDocument()->clone());
        return result;
    }

protected:
    virtual bool createReport(XReport* report) const {
        XAutoTableElement tableElement(m_Model);
        tableElement.setVerticalHeaderVisible(b_WithHeaders);
        report->addElement(tableElement);
        return true;
    }

protected:
    QAbstractItemModel *m_Model;
    bool b_WithHeaders;

};

#endif // LISTREPORT_H
