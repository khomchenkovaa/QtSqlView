#ifndef XREPORTS_REPORTLAYOUT_H
#define XREPORTS_REPORTLAYOUT_H

#include "XReportsTextDocument.h"
#include "XReportsReportBuilder.h"

#include <QAbstractTextDocumentLayout>
#include <QPainter>

namespace XReports {

class ReportLayout
{
    struct ReportLayoutPrivate {
        TextDocument  textDocument;
        ReportBuilder builder;

        ReportLayoutPrivate (XReports::Report *report)
            : builder(textDocument,
                      QTextCursor(&textDocument.document()),
                      report)
        {}
    };

public:
    explicit ReportLayout(XReports::Report *report) : d(report) {}

    void setDefaultFont(const QFont &font)  {
        d.textDocument.document().setDefaultFont(font);
        d.builder.setDefaultFont(font);
    }

    QFont defaultFont() const  {
        return d.textDocument.defaultFont();
    }

    void paintPageContent(int pageNumber, QPainter &painter) {
        painter.translate(0, -pageNumber * d.textDocument.document().pageSize().height());

        // Instead of using drawContents directly, we have to fork it in order to fix the palette (to avoid white-on-white in dark color schemes)
        // d.contentDocument().drawContents(&painter, painter.clipRegion().boundingRect());
        // This even allows us to optimize it a bit (painter clip rect already set)

        QAbstractTextDocumentLayout::PaintContext ctx;
        ctx.clip = painter.clipRegion().boundingRect();
        ctx.palette.setColor(QPalette::Text, Qt::black);
        d.textDocument.document().documentLayout()->draw(&painter, ctx);

    }

    int numberOfPages()  {
        return d.textDocument.document().pageCount();
    }

    /**
     * Returns the width that could be used when exporting to an image, for instance.
     * Unrelated to any paper sizes, just from the contents.
     * The returned width is in pixels.
     */
    qreal idealWidth()  {
        // See Database example for the +1, the right border was missing otherwise.
        return d.textDocument.document().idealWidth() + 1; // in pixels
    }

    /**
     * Sets the size of the "page content": i.e. the body of the report,
     * not including the page headers+footers.
     * The size is in pixels.
     */
    void setPageContentSize(QSizeF size)  {
        d.textDocument.setPageSize(size);
    }

    QString anchorAt(int pageNumber, QPoint pos)  {
        const QPointF posInPage = pos + QPointF(0, pageNumber * d.textDocument.document().pageSize().height());
        return d.textDocument.document().documentLayout()->anchorAt(posInPage);
    }

    QString toHtml() const  {
        return d.textDocument.asHtml();
    }

    void finishHtmlExport()  {
        d.textDocument.saveResourcesToFiles();
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
