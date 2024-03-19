#ifndef SIMPLEREPORT_H
#define SIMPLEREPORT_H

#include "listreport.h"

class SimpleReport : public ListReport
{
    Q_OBJECT

public:
    explicit SimpleReport(QObject *parent = nullptr) : ListReport(parent) {}

    void setTitle(const QString& title) {
        m_Title = title;
    }

    void setHeader(const QString& header) {
        m_Header = header;
    }

    void setFooter(const QString& footer) {
        m_Footer = footer;
    }

protected:
    virtual bool createReport(XReport *report) const {
        if (!m_Title.isEmpty()) {
            // Add a text element for the title
            XTextElement titleElement(m_Title);
            titleElement.setPointSize(18);
            report->addElement(titleElement, Qt::AlignHCenter);
        }
        if (!m_Header.isEmpty()) {
            // add 20 mm of vertical space:
            report->addVerticalSpacing(20);
            // add html text
            XHtmlElement textElement(m_Header);
            report->addElement(textElement);
        }

        if (m_Model) {
            XAutoTableElement tableElement(m_Model);
            tableElement.setVerticalHeaderVisible(false);
            report->addElement(tableElement);
        }

        if (!m_Footer.isEmpty()) {
            // add 20 mm of vertical space:
            report->addVerticalSpacing(20);
            // add html text
            XHtmlElement textElement(m_Footer);
            report->addElement(textElement);
        }
        return true;
    }

protected:
    QString m_Title;
    QString m_Header;
    QString m_Footer;
};

#endif // SIMPLEREPORT_H
