#include "listreport.h"

#include "reporttableelement.h"

#include <QCoreApplication>
#include <QFileDialog>

/******************************************************************/

ListReport::ListReport(QObject *parent)
    : Report::ReportBase(parent)
    , m_Model(Q_NULLPTR)
{
    b_WithHeaders = true;
}

/******************************************************************/

void ListReport::setModel(QAbstractTableModel *model)
{
    m_Model = model;
}

/******************************************************************/

void ListReport::setWithHeaders(bool withHeaders)
{
    b_WithHeaders = withHeaders;
}

/******************************************************************/

bool ListReport::toCsvFile(const QString& fileName) const
{
    Report::TableElement table(m_Model, b_WithHeaders);
    return table.toCsvFile(fileName);
}

/******************************************************************/

QSharedPointer<QTextDocument> ListReport::toTextDocument() const
{
    auto document = ReportBase::toTextDocument();
    QTextCursor cursor(document.data());
    Report::TableElement table(m_Model, b_WithHeaders);
    table.build(&cursor);
    return document;
}

/******************************************************************/
