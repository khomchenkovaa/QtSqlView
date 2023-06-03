#include "kdsimplereport.h"

#include <QAbstractItemModel>

#include <KDReports>

/******************************************************************/

KdSimpleReport::KdSimpleReport(QObject *parent)
    : SimpleReport(parent)
{

}

/******************************************************************/

void KdSimpleReport::toPreview() const
{
    KDReports::Report report;

    if (!m_Title.isEmpty()) {
        // Add a text element for the title
        KDReports::TextElement titleElement(m_Title);
        titleElement.setPointSize(18);
        report.addElement(titleElement, Qt::AlignHCenter);
    }
    if (!m_Header.isEmpty()) {
        // add 20 mm of vertical space:
        report.addVerticalSpacing(20);
        // add some more text
        KDReports::HtmlElement textElement(m_Header);
        report.addElement(textElement);
    }
    KDReports::AutoTableElement tableElement(m_Model);
    tableElement.setVerticalHeaderVisible(false);
    report.addElement(tableElement);

    if (!m_Footer.isEmpty()) {
        // add 20 mm of vertical space:
        report.addVerticalSpacing(20);
        // add some more text
        KDReports::HtmlElement textElement(m_Footer);
        report.addElement(textElement);        \
    }

    KDReports::PreviewDialog preview(&report);
    preview.exec();
}

/******************************************************************/
