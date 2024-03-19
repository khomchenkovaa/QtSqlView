#include "XReportsReportLayout.h"

#include <QAbstractTextDocumentLayout>
#include <QDebug>
#include <QPainter>
#include <QTextBlock>

/***********************************************************/

void XReports::ReportLayout::paintPageContent(int pageNumber, QPainter &painter)
{
    painter.translate(0, -pageNumber * d.contentDocument().pageSize().height());

    // Instead of using drawContents directly, we have to fork it in order to fix the palette (to avoid white-on-white in dark color schemes)
    // d.contentDocument().drawContents(&painter, painter.clipRegion().boundingRect());
    // This even allows us to optimize it a bit (painter clip rect already set)

    QAbstractTextDocumentLayout::PaintContext ctx;
    ctx.clip = painter.clipRegion().boundingRect();
    ctx.palette.setColor(QPalette::Text, Qt::black);
    d.contentDocument().documentLayout()->draw(&painter, ctx);
}

/***********************************************************/

qreal XReports::ReportLayout::layoutAsOnePage(qreal docWidth)
{
    d.textDocument.layoutWithTextWidth(docWidth);
    qreal docHeight = d.contentDocument().size().height();

    // We need to get rid of all page breaks...
    // Unittest: PageLayout::testEndlessPrinterWithPageBreaks()
    QTextCursor c(&d.contentDocument());
    c.beginEditBlock();
    QTextBlock block = d.contentDocument().firstBlock();
    do {
        // qDebug() << "addBlock: Looking at block" << block.blockNumber();
        QTextBlockFormat format = block.blockFormat();
        if (format.pageBreakPolicy() != QTextBlockFormat::PageBreak_Auto)
            format.setPageBreakPolicy(QTextBlockFormat::PageBreak_Auto);
        c.setPosition(block.position());
        c.setBlockFormat(format);
        block = block.next();
    } while (block.isValid());
    c.endEditBlock();

    setPageContentSize(QSizeF(docWidth, docHeight));
    qDebug() << "d.textDocument.layoutDocument().setPageSize" << docWidth << "x" << docHeight << numberOfPages() << "pages";
    qreal newDocHeight = d.contentDocument().size().height();
    if (newDocHeight > docHeight) {
        // QTextDocument is playing tricks on us; it was able to layout as docWidth x docHeight
        // but once we set that as the page size, we end up with more height...
        // Unittest: PageLayout::testEndlessPrinterBug()
        qDebug() << "newDocHeight=" << newDocHeight << "expected" << docHeight;
        setPageContentSize(QSizeF(docWidth, newDocHeight));
        newDocHeight = d.contentDocument().size().height();
        qDebug() << "final newDocHeight=" << newDocHeight << numberOfPages() << "pages";
    }

    /* cppcheck-suppress assertWithSideEffect */
    Q_ASSERT(numberOfPages() == 1);
    return newDocHeight;
}

/***********************************************************/
