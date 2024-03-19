#ifndef XREPORTS_REPORTLAYOUT_H
#define XREPORTS_REPORTLAYOUT_H

#include "XReportsTextDocument.h"
#include "XReportsReportBuilder.h"

QT_BEGIN_NAMESPACE
class QFont;
class QPainter;
class QPoint;
class QSizeF;
QT_END_NAMESPACE

namespace XReports {

class ReportLayout
{
    struct ReportLayoutPrivate {
        TextDocument  textDocument;
        ReportBuilder builder;

        ReportLayoutPrivate (XReports::Report *report)
            : textDocument()
            , builder(textDocument.contentDocumentData(),
                      QTextCursor(&textDocument.contentDocument()),
                      report)
        {}

        QTextDocument &contentDocument() {
            return textDocument.contentDocument();
        }
    };

public:
    explicit ReportLayout(XReports::Report *report) : d(report) {}

    void setDefaultFont(const QFont &font)  {
        d.textDocument.contentDocument().setDefaultFont(font);
        d.builder.setDefaultFont(font);
    }

    QFont defaultFont() const  {
        return d.textDocument.defaultFont();
    }

    void paintPageContent(int pageNumber, QPainter &painter);

    int numberOfPages()  {
        return d.textDocument.contentDocument().pageCount();
    }

    /**
     * Returns the width that could be used when exporting to an image, for instance.
     * Unrelated to any paper sizes, just from the contents.
     * The returned width is in pixels.
     */
    qreal idealWidth()  {
        // See Database example for the +1, the right border was missing otherwise.
        return d.textDocument.contentDocument().idealWidth() + 1; // in pixels
    }

    /**
     * Sets the size of the "page content": i.e. the body of the report,
     * not including the page headers+footers.
     * The size is in pixels.
     */
    void setPageContentSize(QSizeF size)  {
        d.textDocument.setPageSize(size);
    }

    void updateTextValue(const QString &id, const QString &newValue)  {
        d.textDocument.updateTextValue(id, newValue);
    }

    qreal layoutAsOnePage(qreal width) ;

    QString anchorAt(int pageNumber, QPoint pos)  {
        const QPointF posInPage = pos + QPointF(0, pageNumber * d.contentDocument().pageSize().height());
        return d.contentDocument().documentLayout()->anchorAt(posInPage);
    }

    QString toHtml() const  {
        return d.textDocument.asHtml();
    }

    void finishHtmlExport()  {
        d.textDocument.contentDocumentData().saveResourcesToFiles();
    }

    TextDocument &textDocument() {
        return d.textDocument;
    }

    ReportBuilder *builder() {
        return &d.builder;
    }

private:
    ReportLayoutPrivate d;
};

} // namespace XReports

#endif // XREPORTS_REPORTLAYOUT_H
