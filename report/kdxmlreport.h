#ifndef KDXMLREPORT_H
#define KDXMLREPORT_H

#include "listreport.h"

#include <QDomDocument>
#include <KDReports>

class KdXmlReport : public ListReport
{
    Q_OBJECT
public:
    explicit KdXmlReport(QObject *parent = nullptr);

    bool setXml(const QString& text, QString *errorMsg=nullptr, int *errorLine=nullptr, int *errorColumn=nullptr);

    bool toPrinter() const;
    int toPreviewDialog() const;
    bool toPdfFile(const QString& fileName) const;
    bool toHtmlFile(const QString& fileName) const;

private:
    bool createReport(KDReports::Report* report) const;

private:
    QDomDocument m_Xml;
};

#endif // KDXMLREPORT_H
