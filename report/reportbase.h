#ifndef REPORTBASE_H
#define REPORTBASE_H

#ifdef KD_REPORTS
#include <KDReports>
typedef KDReports::Report           XReport;
typedef KDReports::TextElement      XTextElement;
typedef KDReports::HtmlElement      XHtmlElement;
typedef KDReports::AutoTableElement XAutoTableElement;
#else
#include "XReports.h"
typedef XReports::Report            XReport;
typedef XReports::TextElement       XTextElement;
typedef XReports::HtmlElement       XHtmlElement;
typedef XReports::AutoTableElement  XAutoTableElement;
#endif

#include <QObject>
#include <QTextDocument>

namespace Report {

class ReportBase : public QObject
{
    Q_OBJECT
public:
    explicit ReportBase(QObject *parent = nullptr)
        : QObject(parent)
        , m_Document(new QTextDocument())
        {}

    virtual QSharedPointer<QTextDocument> toTextDocument() const {
        return m_Document;
    }

    virtual bool toPrinter() const {
        XReport report;
        if (!createReport(&report)) return false;
        return report.printWithDialog(Q_NULLPTR);
    }

#ifdef KD_REPORTS
    virtual int toPreviewDialog() const {
        XReport report;
        if (!createReport(&report)) return QDialog::Rejected;
        KDReports::PreviewDialog preview(&report);
        return preview.exec();
    }
#else
    virtual int toPreviewDialog() const {
        XReport report;
        if (!createReport(&report)) return false;
        return report.printWithPreview(Q_NULLPTR);
    }
#endif

    virtual bool toPdfFile(const QString& fileName) const {
        if (fileName.isEmpty()) return false;
        XReport report;
        if (!createReport(&report)) return false;
        return report.exportToFile(fileName);
    }

    virtual bool toHtmlFile(const QString& fileName) const {
        if (fileName.isEmpty()) return false;
        XReport report;
        if (!createReport(&report)) return false;
        return report.exportToHtml(fileName);
    }

    //! Clears the document
    void clear() {
        m_Document->clear();
    }

    //! Replaces the entire contents of the document with the given HTML-formatted text in the html string
    void setHtml(const QString& html) {
        m_Document->setHtml(html);
    }

    //! Replaces the entire contents of the document with the given plain text.
    void setPlainText(const QString& text) {
        m_Document->setPlainText(text);
    }

protected:
    virtual bool createReport(XReport* report) const {
        XHtmlElement htmlElement(m_Document->toHtml());
        report->addElement(htmlElement);
        return true;
    }

private:
    QSharedPointer<QTextDocument> m_Document;
};

} // Report

#endif // REPORTBASE_H
