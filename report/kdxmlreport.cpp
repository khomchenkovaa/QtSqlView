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

bool KdXmlReport::setXml(const QString &text, QString *err)
{
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

/******************************************************************/

bool KdXmlReport::toPrinter() const
{
    KDReports::Report report;
    if (!createReport(&report)) return false;
    return report.printWithDialog(Q_NULLPTR);
}

/******************************************************************/

int KdXmlReport::toPreviewDialog() const
{
    KDReports::Report report;
    if (!createReport(&report)) return QDialog::Rejected;

    KDReports::PreviewDialog preview(&report);
    return preview.exec();
}

/******************************************************************/

bool KdXmlReport::toPdfFile(const QString &fileName) const
{
    KDReports::Report report;
    if (!createReport(&report)) return false;
    return report.exportToFile(fileName);
}

/******************************************************************/

bool KdXmlReport::toHtmlFile(const QString &fileName) const
{
    KDReports::Report report;
    if (!createReport(&report)) return false;
    return report.exportToFile(fileName);
}

/******************************************************************/

bool KdXmlReport::createReport(KDReports::Report *report) const
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

/******************************************************************/
