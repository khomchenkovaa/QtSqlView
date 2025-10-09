#ifndef XREPORTSREPORT_H
#define XREPORTSREPORT_H

#include "XReportsEnums.h"

#include <QColor>
#include <QFont>
#include <QObject>
#include <QPrinter>
#include <QSizeF>
#include <QTextOption>

#include <functional>
#include <memory>

QT_BEGIN_NAMESPACE
class QTextDocumentFragment;
class QTextDocument;
class QAbstractItemModel;
class QIODevice;
class QRect;
class QPainter;
class ReportData;
class EditorData;
QT_END_NAMESPACE

namespace XReports {

class MainTable;
class Element;
class ReportPrivate;
class ReportBuilder;
class TextDocument;
class TableBreakingSettingsDialog;
class AutoTableElement;

/**
 * The Report class represents a report.
 *
 * Initially the report is empty.
 * Content can be added to the report using the addElement API
 *
 * The report can then be previewed with PreviewDialog, printed, exported to PDF, or exported to an image.
 */
class Report : public QObject
{
    Q_OBJECT
    Q_PROPERTY(QString documentName READ documentName WRITE setDocumentName)

public:
    /**
     * Constructs a report.
     * \param parent optional parent object, for memory management
     */
    explicit Report(QObject *parent = nullptr);

    //! Destroys the report.
    ~Report() override;

    //! Sets the default font used for text in this report.
    void setDefaultFont(const QFont &font);

    //! \return the default font used for text in this report
    QFont defaultFont() const;

    /**
     * Adds an element to the report, next to the previous element, in the same paragraph.
     *
     * An element defines what is added to the report. The report does not store
     * the element, though. For this reason, any changes to the element after insertion
     * will have no effect on the report. Set all settings in the element before calling addInlineElement.
     */
    void addInlineElement(const Element &element);

    /**
     * Adds an element to the report, creating a new paragraph for it.
     * You can specify the alignment of that paragraph.
     *
     * An element defines what is added to the report. The report does not store
     * the element, though. For this reason, any changes to the element after insertion
     * will have no effect on the report. Set all settings in the element before calling addElement.
     *
     * \param element the definition of what to add to the report
     * \param horizontalAlignment the alignment of the paragraph created for this element
     * \param backgroundColor the background color of the paragraph created for this element
     * The difference with Element::setBackground is that it affects the whole paragraph, including
     * any inline elements as well as the unused area up to the right margin of the page.
     */
    void addElement(const Element &element, Qt::AlignmentFlag horizontalAlignment = Qt::AlignLeft, const QColor &backgroundColor = QColor());

    /**
     * Adds vertical spacing between paragraphs.
     * Make sure to call addElement after that, not addInlineElement.
     * \param space the space in millimeters
     */
    void addVerticalSpacing(qreal space);

    /**
     * Adds a piece of formatted text from a QTextDocument.
     * This method is an optimization for the case where the initial text
     * is in a QTextDocument (e.g. in a QTextEdit). In general you
     * probably want to use addElement(HtmlElement()) instead.
     */
    void addFragment(const QTextDocumentFragment &fragment);

    /**
     * Adds a page break to the report.
     * The next element added to the report will be at the top of a page.
     *
     * Note that you cannot call addPageBreak twice in a row, it will only insert one page break.
     * To get a completely empty page you need to insert an empty text element between both calls
     * to addPageBreak.
     *
     * Also note that exporting to an image, or using an endless printer, will remove
     * all page breaks in the report.
     */
    void addPageBreak();

    /**
     * Sets the paragraph margins for all paragraph elements to be created from now on.
     * The default margins are 0,0,0,0.
     */
    void setParagraphMargins(qreal left, qreal top, qreal right, qreal bottom);

#if QT_VERSION < QT_VERSION_CHECK(6, 0, 0)
    /**
     * Sets the orientation, QPrinter::Portrait or QPrinter::Landscape.
     * Deprecated, use setPageOrientation
     */
    Q_DECL_DEPRECATED void setOrientation(QPrinter::Orientation orientation);
    /**
     * \return the orientation set by setOrientation. The default orientation is QPrinter::Portrait.
     * Deprecated, use pageOrientation
     */
    Q_DECL_DEPRECATED QPrinter::Orientation orientation() const;
#endif

    /**
     * Sets the orientation, QPageLayout::Portrait or QPageLayout::Landscape.
     */
    void setPageOrientation(QPageLayout::Orientation orientation);
    /**
     * \return the orientation set by setPageOrientation. The default orientation is QPageLayout::Portrait.
     */
    QPageLayout::Orientation pageOrientation() const;

    /**
     * Sets the name of the report.
     * This is used for QPrinter::setDocName(), which gives a name to the print job.
     */
    void setDocumentName(const QString &name);

    //! Returns the name of the report.
    QString documentName() const;

    /**
     * Show the print dialog to let the user choose a printer, and print.
     * \param parent the parent widget for the progress dialog that appears when printing
     * \return false if the print dialog was cancelled
     */
    bool printWithDialog(QWidget *parent);

    /**
     * Show the preview dialog to let the user choose a printer, and print.
     * \param parent the parent widget for the progress dialog that appears when printing
     * \return false if the print dialog was cancelled
     */
    bool printWithPreview(QWidget *parent);

    /**
     * Print directly using the setup values from a preexisting
     * QPrinter object.
     * Notice that this will only change settings like page sizes or
     * orientation temporarily, during this particular print
     * operation. After the operation, these values will be reset to
     * their previous values.
     *
     * Returns false if cancelled (not possible yet).
     * \param printer the printer to use for printing
     * \param parent the parent widget for the progress dialog that appears when printing
     */
    bool print(QPrinter *printer, QWidget *parent = nullptr);

    /**
     * Export the whole report to a PS or PDF file.
     * If the file name has the suffix ".ps" then PostScript is automatically
     * selected as output format.
     * If the file name has the ".pdf" suffix PDF is generated.
     */
    bool exportToFile(const QString &fileName, QWidget *parent = nullptr);

    /**
     * Export the whole report to an image file.
     * \param size the size of the image in pixels
     * \param fileName the name of the image file
     * \param format the format of the image, for instance: BMP, JPG, PNG.
     */
    bool exportToImage(QSize size, const QString &fileName, const char *format);

    /**
     * Export the whole report to HTML.
     * Note that HTML export does not include headers and footers, nor watermark.
     *
     * Images are saved into separate files.
     */
    bool exportToHtml(const QString &fileName);

    /**
     * Paints a given page of the report into the painter.
     * Note that this method can be used for printing and for on-screen rendering
     * so it doesn't fill the background with white, the caller has do to that.
     * @param pageNumber the page to paint, starting at 0
     * @param painter the QPainter to paint into
     */
    void paintPage(int pageNumber, QPainter &painter);

    /**
     * Sets the number of the first page, so that the variable PageNumber
     * starts at another value than 1. This is useful when splitting a
     * large report into smaller documents.
     */
    void setFirstPageNumber(int num);

    //! \return the number given to setFirstPageNumber
    int firstPageNumber() const;

    //! \return the report's number of pages (with the current page size).
    int numberOfPages() const;

    //! \internal Returns the current builder cursor position.
    int currentPosition() const;

    /**
     * Indicates that the elements are about to be added to the report.
     * This makes report generation much faster, since the report doesn't
     * have to be relayouted until endEdit() is called.
     * It is possible to nest calls to beginEdit()/endEdit(), in which case
     * only the topmost pair of calls will have an effect.
     */
    void beginEdit();

    /**
     * Indicates that the end of a block of editing operations.
     * It is possible to call beginEdit()/endEdit() again afterwards, for
     * another block of editing operations.
     */
    void endEdit();

    /**
     * Returns the target of the anchor at position pos, or an empty string
     * if no anchor exists at that position.
     * @param pageNumber The page on which the mouse is, starting at 0
     * @param pos The position of the mouse, in pixels
     */
    QString anchorAt(int pageNumber, QPoint pos) const;

    /**
     * Returns the QTextDocument that contains the main part of the report.
     * Note that this does not include any headers or footers.
     *
     * This method can be used to display the report in a readonly QTextEdit
     * or in a QTextBrowser.
     *
     * Modifying this QTextDocument will lead to undefined behaviour in XReports.
     */
    QTextDocument *mainTextDocument() const;

private:
    friend class Test;
    friend class ::ReportData;
    friend class ::EditorData;
    XReports::TextDocument &doc() const;

    QString asHtml() const;

    void setupPrinter(QPrinter *printer);

private:
    Q_DISABLE_COPY(Report)
    friend class ImageElement; // for textDocumentWidth()
    friend class ChartElement; // for textDocumentWidth()
    friend class ReportPrivate; // setupPrinter
    std::unique_ptr<ReportPrivate> d;
};

}

#endif /* XREPORTSREPORT_H */
