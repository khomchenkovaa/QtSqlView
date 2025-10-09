#include "XReportsReport.h"

#include "XReportsElement.h"
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
    , m_orientation(QPageLayout::Portrait)
    , m_firstPageNumber(1)
    , m_layout(report)
{
}

//@cond PRIVATE
void XReports::ReportPrivate::paintPage(int pageNumber, QPainter &painter)
{
    m_layout.paintPageContent(pageNumber, painter);

    QAbstractTextDocumentLayout::PaintContext ctx;
    ctx.palette.setColor(QPalette::Text, Qt::black);
}
//@endcond

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

XReports::ReportBuilder *XReports::ReportPrivate::builder()
{
    return m_layout.builder();
}

////

XReports::Report::Report(QObject *parent)
    : QObject(parent)
    , d(new ReportPrivate(this))
{}


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
void XReports::Report::setOrientation(QPrinter::Orientation orientation)
{
    d->m_orientation = static_cast<QPageLayout::Orientation>(orientation);
}

QPrinter::Orientation XReports::Report::orientation() const
{
    return static_cast<QPrinter::Orientation>(d->m_orientation);
}
#endif

void XReports::Report::setPageOrientation(QPageLayout::Orientation orientation)
{
    d->m_orientation = orientation;
}

QPageLayout::Orientation XReports::Report::pageOrientation() const
{
    return d->m_orientation;
}


void XReports::Report::paintPage(int pageNumber, QPainter &painter)
{
    d->paintPage(pageNumber, painter);
}

int XReports::Report::numberOfPages() const
{
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
    printer->setFullPage(true);
    return d->doPrint(printer, parent);
}

bool XReports::Report::exportToFile(const QString &fileName, QWidget *parent)
{
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
    d->paintPage(0, painter);

    return image.save(fileName, format);
}

XReports::TextDocument &XReports::Report::doc() const
{
    return d->m_layout.textDocument();
}

void XReports::Report::beginEdit()
{
    d->builder()->cursor().beginEditBlock();
}

void XReports::Report::endEdit()
{
    d->builder()->cursor().endEditBlock();
}

QString XReports::Report::anchorAt(int pageNumber, QPoint pos) const
{
    return d->m_layout.anchorAt(pageNumber, pos);
}

QTextDocument *XReports::Report::mainTextDocument() const
{
    return &d->m_layout.textDocument().document();
}

void XReports::Report::addPageBreak()
{
    d->builder()->addPageBreak();
}

void XReports::Report::addFragment(const QTextDocumentFragment &fragment)
{
    d->builder()->insertFragment(fragment);
}

void XReports::Report::setDefaultFont(const QFont &font)
{
    d->m_layout.setDefaultFont(font);
}

QFont XReports::Report::defaultFont() const
{
    return d->m_layout.defaultFont();
}

void XReports::Report::setDocumentName(const QString &name)
{
    d->m_documentName = name;
}

QString XReports::Report::documentName() const
{
    return d->m_documentName;
}

void XReports::Report::setParagraphMargins(qreal left, qreal top, qreal right, qreal bottom)
{
    d->builder()->setParagraphMargins(left, top, right, bottom);
}

int XReports::Report::currentPosition() const
{
    return d->builder()->cursorPosition();
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
    printer->setDocName(d->m_documentName);
}
