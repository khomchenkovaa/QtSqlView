#include "kdsimplereport.h"

#include <QAbstractItemModel>

/******************************************************************/

KdSimpleReport::KdSimpleReport(QObject *parent)
    : SimpleReport(parent)
{

}

/******************************************************************/

bool KdSimpleReport::toPrinter() const
{
    KDReports::Report report;
    createReport(&report);
    return report.printWithDialog(Q_NULLPTR);
}

/******************************************************************/

int KdSimpleReport::toPreviewDialog() const
{
    KDReports::Report report;
    createReport(&report);
    KDReports::PreviewDialog preview(&report);
    return preview.exec();
}

/******************************************************************/

bool KdSimpleReport::toPdfFile(const QString &fileName) const
{
    KDReports::Report report;
    createReport(&report);
    return report.exportToFile(fileName);
}

/******************************************************************/

bool KdSimpleReport::toHtmlFile(const QString &fileName) const
{
    KDReports::Report report;
    createReport(&report);
    return report.exportToHtml(fileName);
}

/******************************************************************/

void KdSimpleReport::createReport(KDReports::Report *report) const
{
    if (!m_Title.isEmpty()) {
        // Add a text element for the title
        KDReports::TextElement titleElement(m_Title);
        titleElement.setPointSize(18);
        report->addElement(titleElement, Qt::AlignHCenter);
    }
    if (!m_Header.isEmpty()) {
        // add 20 mm of vertical space:
        report->addVerticalSpacing(20);
        // add html text
        KDReports::HtmlElement textElement(m_Header);
        report->addElement(textElement);
    }

    if (m_Model) {
        KDReports::AutoTableElement tableElement(m_Model);
        tableElement.setVerticalHeaderVisible(false);
        report->addElement(tableElement);
    }

    if (!m_Footer.isEmpty()) {
        // add 20 mm of vertical space:
        report->addVerticalSpacing(20);
        // add html text
        KDReports::HtmlElement textElement(m_Footer);
        report->addElement(textElement);
    }
}

/******************************************************************/
