#include "XReportsReport.h"

#include "XReportsElement.h"
#include "XReportsHeader.h"
#include "XReportsUtils.h"
#include "XReportsReport_p.h"

#include <QApplication>
#include <QPainter>
#include <QPointer>
#include <QPrintDialog>
#include <QPrintPreviewDialog>
#include <QProgressDialog>
#include <QThread>

#include <memory>

XReports::ReportPrivate::ReportPrivate(Report *report)
    : m_layoutWidth(0)
    , m_endlessPrinterWidth(0)
    , m_orientation(QPageLayout::Portrait)
    , m_pageSize(QPageSize::A4)
    , m_marginTop(20.0)
    , m_marginLeft(20.0)
    , m_marginBottom(20.0)
    , m_marginRight(20.0)
    , m_headerBodySpacing(0)
    , m_footerBodySpacing(0)
    , m_headers()
    , m_footers()
    , m_firstPageNumber(1)
    , m_pageContentSizeDirty(true)
    , m_layout(report)
{
}

bool XReports::ReportPrivate::wantEndlessPrinting() const
{
    return m_layoutWidth > 0;
}

QSizeF XReports::ReportPrivate::paperSize() const
{
    // determine m_paperSize from m_pageSize if needed
    if (m_paperSize.isEmpty()) {
        const auto mmSize = m_pageSize.size(QPageSize::Millimeter);
        m_paperSize = QSizeF {mmToPixels(mmSize.width()), mmToPixels(mmSize.height())};
        if (m_orientation == QPageLayout::Landscape) {
            m_paperSize.transpose();
        }
    }
    // qDebug() << "m_paperSize=" << m_paperSize;
    return m_paperSize;
}

void XReports::ReportPrivate::ensureLayouted()
{
    // We need to do a layout if
    // m_pageContentSizeDirty is true, i.e. page size has changed etc.
    if (m_pageContentSizeDirty) {
        if (!wantEndlessPrinting()) {
            setPaperSizeFromPrinter(paperSize());
        } else {
            // Get the document to fit into one page
            Q_ASSERT(m_layoutWidth != 0);
            qreal textDocWidth = m_layoutWidth - mmToPixels(m_marginLeft + m_marginRight);
            m_paperSize = layoutAsOnePage(textDocWidth);

            qDebug() << "setPaperSizeFromPrinter: endless printer. m_layoutWidth=" << m_layoutWidth << "textDocWidth=" << textDocWidth << "single page has size" << m_paperSize << "pixels";

            /* cppcheck-suppress assertWithSideEffect */
            Q_ASSERT(m_layout.numberOfPages() == 1);
        }
        // at this point m_pageContentSizeDirty has been set to false in all cases
    }
}

// The height of the text doc, by calculation. Adjusted by caller, if negative.
qreal XReports::ReportPrivate::rawMainTextDocHeight() const
{
    qreal textDocHeight = paperSize().height() - mmToPixels(m_marginTop + m_marginBottom);
    const qreal headerHeight = m_headers.height();
    textDocHeight -= headerHeight;
    textDocHeight -= mmToPixels(m_headerBodySpacing);
    const qreal footerHeight = m_footers.height();
    textDocHeight -= mmToPixels(m_footerBodySpacing);
    textDocHeight -= footerHeight;
    // qDebug() << "pageContent height (pixels): paper size" << m_paperSize.height() << "minus margins" << mmToPixels( m_marginTop + m_marginBottom )
    //        << "minus headerHeight" << headerHeight << "minus footerHeight" << footerHeight << "and spacings =" << textDocHeight;
    return textDocHeight;
}

bool XReports::ReportPrivate::skipHeadersFooters() const
{
    const bool skip = rawMainTextDocHeight() <= 0;
    if (skip) {
        qDebug() << "Not enough height for headers and footers in this page size, hiding headers and footers.";
    }
    return skip;
}

qreal XReports::ReportPrivate::mainTextDocHeight() const
{
    const qreal height = rawMainTextDocHeight();
    const bool skip = height <= 0;
    if (skip) {
        qreal textDocHeight = paperSize().height() - mmToPixels(m_marginTop + m_marginBottom);
        textDocHeight -= mmToPixels(m_headerBodySpacing);
        textDocHeight -= mmToPixels(m_footerBodySpacing);
        return textDocHeight;
    }
    return height;
}

QRect XReports::ReportPrivate::mainTextDocRect() const
{
    const int left = qRound(mmToPixels(m_marginLeft));
    const int top = qRound(mmToPixels(m_marginTop));
    const int headerHeightWithSpacing = qRound((skipHeadersFooters() ? 0 : m_headers.height()) + mmToPixels(m_headerBodySpacing));
    const int textDocWidth = qRound(m_paperSize.width() - mmToPixels(m_marginLeft + m_marginRight));
    const int textDocHeight = qRound(mainTextDocHeight());
    return {left, top + headerHeightWithSpacing, textDocWidth, textDocHeight};
}

/*
   [top margin]
   [header]
   [m_headerBodySpacing]
   [body]
   [m_footerBodySpacing]
   [footer]
   [bottom margin]
 */
void XReports::ReportPrivate::setPaperSizeFromPrinter(QSizeF paperSize)
{
    Q_ASSERT(!wantEndlessPrinting()); // call ensureLayouted instead!

    m_paperSize = paperSize;
    const qreal marginsInPixels = mmToPixels(m_marginLeft + m_marginRight);
    qreal textDocWidth = m_paperSize.width() - marginsInPixels;

    m_headers.layoutWithTextWidth(textDocWidth);
    m_footers.layoutWithTextWidth(textDocWidth);

    const qreal textDocHeight = mainTextDocHeight();

    // Font scaling
    // Problem: how to re-implement this without a layout document?
    // We would risk cumulating rounding problems...?
    // ### 2nd problem: what about fonts in headers and footers? shouldn't they scale too?
    // if ( m_scaleFontsBy != 1.0 )
    //    m_textDocument.scaleFontsBy( m_scaleFontsBy );

    m_layout.setPageContentSize(QSizeF(textDocWidth, textDocHeight));

    m_pageContentSizeDirty = false;
}

//@cond PRIVATE
void XReports::ReportPrivate::paintPage(int pageNumber, QPainter &painter)
{
    ensureLayouted();

    const int pageCount = m_layout.numberOfPages();
    XReports::Header *header = m_headers.headerForPage(pageNumber + 1, pageCount);
    if (header) {
        header->preparePaintingPage(pageNumber + m_firstPageNumber - 1);
    }
    XReports::Header *footer = m_footers.headerForPage(pageNumber + 1, pageCount);
    if (footer) {
        footer->preparePaintingPage(pageNumber + m_firstPageNumber - 1);
    }

    const QRect textDocRect = mainTextDocRect();
    const bool skipHeadersFooters = this->skipHeadersFooters();

    painter.save();
    // painter.setClipRect( textDocRect, Qt::IntersectClip ); // triggers a Qt-Windows bug when printing
    painter.setClipRect(textDocRect);
    painter.translate(textDocRect.topLeft());
    m_layout.paintPageContent(pageNumber, painter);
    painter.restore();

    QAbstractTextDocumentLayout::PaintContext ctx;
    ctx.palette.setColor(QPalette::Text, Qt::black);
    if (header && !skipHeadersFooters) {
        painter.save();
        const int top = qRound(mmToPixels(m_marginTop));
        painter.translate(textDocRect.left(), top);
        ctx.clip = painter.clipRegion().boundingRect();
        header->doc().contentDocument().documentLayout()->draw(&painter, ctx);
        painter.restore();
    }
    if (footer && !skipHeadersFooters) {
        painter.save();
        const int bottom = qRound(mmToPixels(m_marginBottom));
        const int footerHeight = qRound(m_footers.height());
        painter.translate(textDocRect.left(), m_paperSize.height() - bottom - footerHeight);
        ctx.clip = painter.clipRegion().boundingRect();
        footer->doc().contentDocument().documentLayout()->draw(&painter, ctx);
        painter.restore();
    }
}
//@endcond

QSizeF XReports::ReportPrivate::layoutAsOnePage(qreal docWidth)
{
    m_headers.layoutWithTextWidth(docWidth);
    m_footers.layoutWithTextWidth(docWidth);

    const qreal docHeight = m_layout.layoutAsOnePage(docWidth);

    qreal pageWidth = docWidth + mmToPixels(m_marginLeft + m_marginRight);
    qreal pageHeight = docHeight + mmToPixels(m_marginTop + m_marginBottom);
    pageHeight += m_headers.height();
    pageHeight += m_footers.height();

    m_pageContentSizeDirty = false;

    // qDebug() << "One-page document is" << pageWidth << "x" << pageHeight;
    return QSizeF(pageWidth, pageHeight);
}

bool XReports::ReportPrivate::doPrint(QPrinter *printer, QWidget *parent)
{
    // caller has to ensure that we have been layouted for this printer already
    const int pageCount = m_layout.numberOfPages();
    std::unique_ptr<QProgressDialog> dialog;
    if (QThread::currentThread() == qApp->thread()) {
        dialog.reset(new QProgressDialog(QObject::tr("Printing"), QObject::tr("Cancel"), 0, pageCount, parent));
        dialog->setWindowModality(Qt::ApplicationModal);
    }
    QPainter painter;
    if (!painter.begin(printer)) {
        qWarning() << "QPainter failed to initialize on the given printer";
        return false;
    }

    int fromPage = 0;
    int toPage = pageCount;
    if (printer->printRange() == QPrinter::PageRange) {
        fromPage = printer->fromPage() - 1; // it starts at 1
        toPage = printer->toPage(); // -1 because it starts at 1, and +1 because of '<'
        if (toPage == 0)
            toPage = pageCount;
    }

    bool firstPage = true;
    for (int pageIndex = fromPage; pageIndex < toPage; ++pageIndex) {
        if (dialog) {
            dialog->setValue(pageIndex);
            if (dialog->wasCanceled())
                break;
        }

        if (!firstPage)
            printer->newPage();

        paintPage(pageIndex, painter);
        firstPage = false;
    }

    return true;
}

qreal XReports::ReportPrivate::textDocumentWidth() const
{
    return paperSize().width() - mmToPixels(m_marginLeft + m_marginRight);
}

XReports::ReportBuilder *XReports::ReportPrivate::builder()
{
    return m_layout.builder();
}

////

XReports::Report::Report(QObject *parent)
    : QObject(parent)
    , d(new ReportPrivate(this))
{
    setPageSize(QPageSize::A4);
}

XReports::Report::~Report()
{

}

void XReports::Report::addInlineElement(const Element &element)
{
    d->builder()->addInlineElement(element);
}

void XReports::Report::addElement(const Element &element, Qt::AlignmentFlag horizontalAlignment, const QColor &backgroundColor)
{
    d->builder()->addBlockElement(element, horizontalAlignment, backgroundColor);
}

void XReports::Report::addVerticalSpacing(qreal space)
{
    d->builder()->addVerticalSpacing(space);
}

#if QT_VERSION < QT_VERSION_CHECK(6, 0, 0)
void XReports::Report::setPageSize(QPrinter::PageSize size)
{
    setPageSize(static_cast<QPageSize::PageSizeId>(size));
}
#endif

void XReports::Report::setPageSize(QPageSize::PageSizeId size)
{
    setPageSize(QPageSize {size});
}

void XReports::Report::setPageSize(const QPageSize &size)
{
    d->m_pageSize = size;
    d->m_paperSize = QSizeF();
    d->m_pageContentSizeDirty = true;
}

void XReports::Report::setPaperSize(QSizeF paperSize, QPrinter::Unit unit)
{
    qreal factor = 1.0;
    switch (unit) {
    case QPrinter::DevicePixel:
        break;
    case QPrinter::Millimeter:
        factor = mmToPixels(1.0);
        break;
    case QPrinter::Point:
        factor = 72.0 * qt_defaultDpi();
        break;
    case QPrinter::Inch:
        factor = qt_defaultDpi();
        break;
    default:
        qWarning("Unsupported printer unit %d", unit);
    }
    d->m_paperSize = QSizeF(paperSize.width() * factor, paperSize.height() * factor);
    d->m_pageContentSizeDirty = true;
}

#if QT_VERSION < QT_VERSION_CHECK(6, 0, 0)
QPrinter::PageSize XReports::Report::pageSize() const
{
    return static_cast<QPrinter::PageSize>(d->m_pageSize.id());
}
#else
QPageSize XReports::Report::pageSize() const
{
    return d->m_pageSize;
}
#endif

#if QT_VERSION < QT_VERSION_CHECK(6, 0, 0)
void XReports::Report::setOrientation(QPrinter::Orientation orientation)
{
    d->m_orientation = static_cast<QPageLayout::Orientation>(orientation);
    d->m_paperSize = QSizeF();
    d->m_pageContentSizeDirty = true;
}

QPrinter::Orientation XReports::Report::orientation() const
{
    return static_cast<QPrinter::Orientation>(d->m_orientation);
}
#endif

void XReports::Report::setPageOrientation(QPageLayout::Orientation orientation)
{
    d->m_orientation = orientation;
    d->m_paperSize = QSizeF();
    d->m_pageContentSizeDirty = true;
}

QPageLayout::Orientation XReports::Report::pageOrientation() const
{
    return d->m_orientation;
}

void XReports::Report::setMargins(qreal top, qreal left, qreal bottom, qreal right)
{
    d->m_marginTop = top;
    d->m_marginLeft = left;
    d->m_marginBottom = bottom;
    d->m_marginRight = right;

    // We'll need to call setPaperSizeFromPrinter, to update % sizes in all text documents.
    d->m_pageContentSizeDirty = true;
}

void XReports::Report::getMargins(qreal *top, qreal *left, qreal *bottom, qreal *right) const
{
    *top = d->m_marginTop;
    *left = d->m_marginLeft;
    *bottom = d->m_marginBottom;
    *right = d->m_marginRight;
}

void XReports::Report::setLeftPageMargin(qreal left)
{
    d->m_marginLeft = left;

    // We'll need to call setPaperSizeFromPrinter, to update % sizes in all text documents.
    d->m_pageContentSizeDirty = true;
}

qreal XReports::Report::leftPageMargins() const
{
    return d->m_marginLeft;
}

void XReports::Report::setRightPageMargin(qreal right)
{
    d->m_marginRight = right;

    // We'll need to call setPaperSizeFromPrinter, to update % sizes in all text documents.
    d->m_pageContentSizeDirty = true;
}

qreal XReports::Report::rightPageMargins() const
{
    return d->m_marginRight;
}

void XReports::Report::setTopPageMargin(qreal top)
{
    d->m_marginTop = top;

    // We'll need to call setPaperSizeFromPrinter, to update % sizes in all text documents.
    d->m_pageContentSizeDirty = true;
}

qreal XReports::Report::topPageMargins() const
{
    return d->m_marginTop;
}

void XReports::Report::setBottomPageMargin(qreal bottom)
{
    d->m_marginBottom = bottom;

    // We'll need to call setPaperSizeFromPrinter, to update % sizes in all text documents.
    d->m_pageContentSizeDirty = true;
}

qreal XReports::Report::bottomPageMargins() const
{
    return d->m_marginBottom;
}

void XReports::Report::setWidthForEndlessPrinter(qreal widthMM)
{
    if (widthMM) {
        d->m_endlessPrinterWidth = widthMM;
        d->m_layoutWidth = mmToPixels(widthMM);
        d->m_pageContentSizeDirty = true;
        d->ensureLayouted();
    } else {
        d->m_layoutWidth = 0;
        d->m_pageContentSizeDirty = true;
        // caller will call setPageSize...
    }
}

void XReports::Report::paintPage(int pageNumber, QPainter &painter)
{
    d->paintPage(pageNumber, painter);
}

int XReports::Report::numberOfPages() const
{
    d->ensureLayouted();
    return d->m_layout.numberOfPages();
}

QString XReports::Report::asHtml() const
{
    return d->m_layout.toHtml();
}

bool XReports::Report::printWithDialog(QWidget *parent)
{
    QPrinter printer;
    setupPrinter(&printer);
    QPointer<QPrintDialog> dialog = new QPrintDialog(&printer, parent);
    dialog->setWindowFlags(Qt::Window);
    dialog->setMinMax(1, numberOfPages());
    bool ok = false;
    if (dialog->exec() == QDialog::Accepted) {
        // Well, the user can modify the page size in the printer dialog too - ensure layout matches
        d->ensureLayouted();
        ok = d->doPrint(&printer, parent);
    }
    delete dialog;
    return ok;
}

bool XReports::Report::printWithPreview(QWidget *parent)
{
    QPrinter printer;
    setupPrinter(&printer);
    QPrintPreviewDialog dialog(&printer);
    dialog.setWindowFlags(Qt::Window);
    connect(&dialog, &QPrintPreviewDialog::paintRequested,
            this, [this, parent](QPrinter *printer){
        d->doPrint(printer, parent);
    });
    return dialog.exec();
}

bool XReports::Report::print(QPrinter *printer, QWidget *parent)
{
    // save the old page size
    const auto savePageSize = pageSize();
    if (d->wantEndlessPrinting()) {
        // ensure that the printer is set up with the right size
        d->ensureLayouted();
        // was: printer->setPaperSize(d->m_paperSize, QPrinter::DevicePixel);
        printer->setPageSize(QPageSize(d->m_paperSize * pixelsToPointsMultiplier(printer->resolution()), QPageSize::Point));

    } else {
        // ensure that the layout matches the printer
        d->setPaperSizeFromPrinter(printer->pageLayout().fullRectPixels(printer->resolution()).size());
    }

    printer->setFullPage(true);

    // don't call ensureLayouted here, it would use the wrong printer!

    const bool ret = d->doPrint(printer, parent);

    // Reset the page size
    setPageSize(savePageSize);

    return ret;
}

bool XReports::Report::exportToFile(const QString &fileName, QWidget *parent)
{
    d->ensureLayouted();
    QPrinter printer;
    printer.setOutputFileName(fileName); // must be done before setupPrinter, since it affects DPI
    setupPrinter(&printer);
    const bool ret = d->doPrint(&printer, parent);
    printer.setOutputFileName(QString());
    return ret;
}

bool XReports::Report::exportToHtml(const QString &fileName)
{
    const QString html = asHtml();
    QFile file(fileName);
    if (file.open(QIODevice::WriteOnly)) {
        file.write(html.toUtf8());
        d->m_layout.finishHtmlExport();
        return true;
    }
    return false;
}

bool XReports::Report::exportToImage(QSize size, const QString &fileName, const char *format)
{
    // Get the document to fit into one page

    const auto savePageSize = pageSize();
    const qreal saveLayoutWidth = d->m_layoutWidth;
    d->m_layoutWidth = d->m_layout.idealWidth() + mmToPixels(d->m_marginLeft + d->m_marginRight);
    d->m_pageContentSizeDirty = true;
    d->ensureLayouted();

    const qreal zoomFactor = qMin(( qreal )size.width() / d->m_paperSize.width(), ( qreal )size.height() / d->m_paperSize.height());
    // qDebug() << "zoomFactor=" << zoomFactor;

    QImage image(size, QImage::Format_ARGB32_Premultiplied);
    image.fill(Qt::white);

    QPainter painter;
    if (!painter.begin(&image)) {
        qWarning() << "QPainter failed to initialize on the given image of size" << size;
        return false;
    }
    painter.setRenderHint(QPainter::Antialiasing);
    painter.setRenderHint(QPainter::SmoothPixmapTransform);
    painter.fillRect(QRectF(0, 0, size.width(), size.height()), QBrush(Qt::white));
    painter.scale(zoomFactor, zoomFactor);
    d->paintPage(0, painter);

    // restore textdoc size and header widths
    d->m_layoutWidth = saveLayoutWidth;
    setPageSize(savePageSize); // redo layout
    qDebug() << "Saving pixmap" << image.size() << "into" << fileName << "with format" << format;
    return image.save(fileName, format);
}

XReports::Header &XReports::Report::header(HeaderLocations hl)
{
    if (!d->m_headers.contains(hl))
        d->m_headers.insert(hl, new Header(this));
    return *d->m_headers.value(hl);
}

void XReports::Report::setHeaderLocation(HeaderLocations hl, Header *header)
{
    // Remove old entry for this header
    HeaderLocations loc = d->m_headers.headerLocation(header);
    d->m_headers.remove(loc);
    d->m_headers.insert(hl, header);
}

XReports::Header &XReports::Report::footer(HeaderLocations hl)
{
    if (!d->m_footers.contains(hl))
        d->m_footers.insert(hl, new Header(this));
    return *d->m_footers.value(hl);
}

void XReports::Report::setFooterLocation(HeaderLocations hl, Footer *footer)
{
    // Remove old entry for this header
    HeaderLocations loc = d->m_footers.headerLocation(footer);
    d->m_footers.remove(loc);
    d->m_footers.insert(hl, footer);
}

XReports::TextDocument &XReports::Report::doc() const
{
    return d->m_layout.textDocument();
}

void XReports::Report::setHeaderChanged()
{
    d->m_pageContentSizeDirty = true;
}

void XReports::Report::beginEdit()
{
    d->builder()->contentDocumentCursor().beginEditBlock();
}

void XReports::Report::endEdit()
{
    d->builder()->contentDocumentCursor().endEditBlock();
}

QString XReports::Report::anchorAt(int pageNumber, QPoint pos) const
{
    const QRect textDocRect = d->mainTextDocRect();
    const QPoint textPos = pos - textDocRect.topLeft();
    return d->m_layout.anchorAt(pageNumber, textPos);
}

QTextDocument *XReports::Report::mainTextDocument() const
{
    return &d->m_layout.textDocument().contentDocument();
}


void XReports::Report::addPageBreak()
{
    d->builder()->addPageBreak();
}

void XReports::Report::associateTextValue(const QString &id, const QString &value)
{
    d->m_layout.updateTextValue(id, value);
    d->m_headers.updateTextValue(id, value);
    d->m_footers.updateTextValue(id, value);
}

QSizeF XReports::Report::paperSize() const
{
    return d->paperSize();
}

void XReports::Report::addFragment(const QTextDocumentFragment &fragment)
{
    d->builder()->insertFragment(fragment);
}

void XReports::Report::setDefaultFont(const QFont &font)
{
    d->m_layout.setDefaultFont(font);
    d->m_pageContentSizeDirty = true;
}

QFont XReports::Report::defaultFont() const
{
    return d->m_layout.defaultFont();
}

void XReports::Report::setHeaderBodySpacing(qreal spacing)
{
    d->m_headerBodySpacing = spacing;
    d->m_pageContentSizeDirty = true;
}

qreal XReports::Report::headerBodySpacing() const
{
    return d->m_headerBodySpacing;
}

void XReports::Report::setFooterBodySpacing(qreal spacing)
{
    d->m_footerBodySpacing = spacing;
    d->m_pageContentSizeDirty = true;
}

qreal XReports::Report::footerBodySpacing() const
{
    return d->m_footerBodySpacing;
}

void XReports::Report::setDocumentName(const QString &name)
{
    d->m_documentName = name;
}

QString XReports::Report::documentName() const
{
    return d->m_documentName;
}

void XReports::Report::setTabPositions(const QList<QTextOption::Tab> &tabs)
{
    d->builder()->setTabPositions(tabs);
}

void XReports::Report::setParagraphMargins(qreal left, qreal top, qreal right, qreal bottom)
{
    d->builder()->setParagraphMargins(left, top, right, bottom);
}

XReports::HeaderLocations XReports::Report::headerLocation(XReports::Header *header) const
{
    return d->m_headers.headerLocation(header);
}

XReports::HeaderLocations XReports::Report::footerLocation(XReports::Footer *footer) const
{
    return d->m_footers.headerLocation(footer);
}

int XReports::Report::currentPosition() const
{
    return d->builder()->currentPosition();
}

void XReports::Report::setFirstPageNumber(int num)
{
    d->m_firstPageNumber = num;
}

int XReports::Report::firstPageNumber() const
{
    return d->m_firstPageNumber;
}

void XReports::Report::setupPrinter(QPrinter *printer)
{
    printer->setFullPage(true);
    printer->setPageOrientation(d->m_orientation);
    // was: printer->setPaperSize(rawPaperSize(d->m_pageSize, printer), QPrinter::DevicePixel);
    printer->setPageSize(d->m_pageSize);
    printer->setDocName(d->m_documentName);
}
