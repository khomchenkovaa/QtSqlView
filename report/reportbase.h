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
        {}

    virtual QSharedPointer<QTextDocument> toTextDocument() const {
        XReport report;
        if (!createReport(&report)) return QSharedPointer<QTextDocument>::create();
        return QSharedPointer<QTextDocument>(report.mainTextDocument()->clone());
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

protected:
    virtual bool createReport(XReport* report) const {
        Q_UNUSED(report)
        return false;
    }
};

} // Report

#endif // REPORTBASE_H
