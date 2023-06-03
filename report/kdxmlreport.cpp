#include "kdxmlreport.h"

#include <QCoreApplication>
#include <QAbstractItemModel>
#include <QMessageBox>

#include <KDReports>

/******************************************************************/

KdXmlReport::KdXmlReport(QObject *parent)
    : ListReport(parent)
{

}

/******************************************************************/

bool KdXmlReport::setXml(const QString &text, QString *errorMsg, int *errorLine, int *errorColumn)
{
    return m_Xml.setContent(text, errorMsg, errorLine, errorColumn);
}

/******************************************************************/

void KdXmlReport::toPreview() const
{
    KDReports::Report report;
    report.associateModel("TableModel", m_Model);
    KDReports::ErrorDetails details;
    if (!report.loadFromXML(m_Xml, &details)) {
        QMessageBox::warning(Q_NULLPTR, QCoreApplication::applicationName(),
                             tr("Could not parse report description:\n%1")
                             .arg(details.message()));
        return;
    }

    KDReports::PreviewDialog preview(&report);
    preview.exec();
}

/******************************************************************/
