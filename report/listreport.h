#ifndef LISTREPORT_H
#define LISTREPORT_H

#include "reportbase.h"

QT_BEGIN_NAMESPACE
class QAbstractTableModel;
QT_END_NAMESPACE

class ListReport : public Report::ReportBase
{
    Q_OBJECT
public:
    explicit ListReport(QObject *parent = nullptr);
    void setModel(QAbstractTableModel *model);
    void setWithHeaders(bool withHeaders);

    virtual bool toCsvFile(const QString& fileName) const;
    virtual QSharedPointer<QTextDocument> toTextDocument() const;

protected:
    QAbstractTableModel *m_Model;
    bool b_WithHeaders;

};

#endif // LISTREPORT_H
