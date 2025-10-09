#ifndef KDXMLREPORT_H
#define KDXMLREPORT_H

#include "listreport.h"

#include <QCoreApplication>
#include <QMessageBox>
#include <QDomDocument>

class KdXmlReport : public ListReport
{
    Q_OBJECT
public:
    explicit KdXmlReport(QObject *parent = nullptr) : ListReport(parent) { }

    bool setXml(const QString& text, QString *err=nullptr) {
        QString errorMsg;
        int errorLine, errorColumn;
        bool result = m_Xml.setContent(text, &errorMsg, &errorLine, &errorColumn);
        if (!result && err) {
            err->append(tr("Could not parse XML\n%1\n in line %2, column %3")
                        .arg(errorMsg)
                        .arg(errorLine)
                        .arg(errorColumn));
        }
        return result;
    }

private:
    bool createReport(XReport* report) const
    {
        report->associateModel("TableModel", m_Model);
        KDReports::ErrorDetails details;
        if (!report->loadFromXML(m_Xml, &details)) {
            QMessageBox::warning(Q_NULLPTR, QCoreApplication::applicationName(),
                                 tr("Could not parse report description:\n%1")
                                 .arg(details.message()));
            return false;
        }
        return true;
    }

private:
    QDomDocument m_Xml;
};

#endif // KDXMLREPORT_H
