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
class Header;
typedef Header Footer;
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
     * Set the list of tabs (tabulations) to use when adding paragraph elements.
     * Those tabs will be set for any paragraph element that is added from now on,
     * until calling this method again.
     *
     * This method allows to align numbers based on their decimal separator, for instance.
     * Example:
     * \code
     QList<QTextOption::Tab> tabs;
     QTextOption::Tab tab;
     tab.position = 50; // in mm
     tab.type = QTextOption::DelimiterTab;
     tab.delimiter = QLatin1Char( '.' );
     tabs.append( tab );
     report.setTabPositions( tabs );
     * \endcode
     * Then insert text using a tab, like TextElement("\t123.456").
     */
    void setTabPositions(const QList<QTextOption::Tab> &tabs);

    /**
     * Sets the paragraph margins for all paragraph elements to be created from now on.
     * The default margins are 0,0,0,0.
     */
    void setParagraphMargins(qreal left, qreal top, qreal right, qreal bottom);

#if QT_VERSION < QT_VERSION_CHECK(6, 0, 0)
    /**
     * Sets the page size of the report. Example: setPageSize(QPrinter::A4) or setPageSize(QPrinter::Letter);
     * Note that for performance reasons, it is recommended to do this after
     * adding all the elements to the report rather than before.
     * Deprecated, use setPageSize(QPageSize::PageSizeId), or setPageSize(QPageSize)
     */
    void setPageSize(QPrinter::PageSize size);
#endif

    /**
     * Sets the page size of the report. Example: setPageSize(QPageSize::A4) or setPageSize(QPageSize::Letter);
     * Note that for performance reasons, it is recommended to do this after
     * adding all the elements to the report rather than before.
     */
    void setPageSize(QPageSize::PageSizeId size);

    /**
     * Sets the page size of the report. Example: setPageSize(QPageSize::A4) or setPageSize(QPageSize::Letter);
     * Note that for performance reasons, it is recommended to do this after
     * adding all the elements to the report rather than before.
     */
    void setPageSize(const QPageSize &size);

#if QT_VERSION < QT_VERSION_CHECK(6, 0, 0)
    /**
     * \return the page size set by setPageSize. The default page size is A4.
     */
    QPrinter::PageSize pageSize() const;
#else
    /**
     * \return the page size set by setPageSize. The default page size is A4.
     */
    QPageSize pageSize() const;
#endif

    /**
     * Sets a custom paper size for the report.
     * Supported units are DevicePixel, Millimeter, Point and Inch.
     */
    void setPaperSize(QSizeF paperSize, QPrinter::Unit unit);

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
     * Set a custom page size for an endless printer.
     * The page width is known, the document is laid out without pagination
     * within that width. The page height is set automatically so that the
     * entire document fits within one page.
     *
     * When calling setWidthForEndlessPrinter you don't have to call setPageSize or setOrientation.
     *
     * Call setWidthForEndlessPrinter(0) to reset to normal (pagesize-dependent) behavior.
     */
    void setWidthForEndlessPrinter(qreal widthMM);

    //! Sets the page margins in mm. The default margins are 20 mm.
    void setMargins(qreal top, qreal left, qreal bottom, qreal right);

    void getMargins(qreal *top, qreal *left, qreal *bottom, qreal *right) const;

    //! Sets the top page margins in mm. The default margin is 20 mm.
    void setTopPageMargin(qreal top);

    //! \return the top page margin in mm.
    qreal topPageMargins() const;

    //! Sets the left page margins in mm. The default margin is 20 mm.
    void setLeftPageMargin(qreal left);

    //! \return the left page margin in mm.
    qreal leftPageMargins() const;

    //! Sets the right page margins in mm. The default margin is 20 mm.
    void setRightPageMargin(qreal right);

    //! \return the right page margin in mm.
    qreal rightPageMargins() const;

    //! Sets the bottom page margins in mm. The default margin is 20 mm.
    void setBottomPageMargin(qreal bottom);

    //! \return the bottom page margin in mm.
    qreal bottomPageMargins() const;

    /**
     * Sets the margin between the header and the body of the report, in mm.
     * This margin is 0 by default, i.e. the body starts immediately under the header.
     */
    void setHeaderBodySpacing(qreal spacing);

    //! \return the margin between the header and the body of the report, in mm.
    qreal headerBodySpacing() const;

    /**
     * Sets the margin between the footer and the body of the report, in mm.
     * This margin is 0 by default, i.e. the footer starts immediately under the body.
     */
    void setFooterBodySpacing(qreal spacing);

    //! \return the margin between the footer and the body of the report, in mm.
    qreal footerBodySpacing() const;

    /**
     * Returns a reference to a header object.
     * Calling this method makes the report have a header on the specified pages.
     *
     * Calling the method with different sets of intersecting flags leads to undefined behavior.
     * For instance header(EvenPages|FirstPage) and header(OddPages) is fine,
     * but header(EvenPages|FirstPage) and header(FirstPage) leads to two headers being
     * defined for the first page, anyone of the two could be picked.
     * Calling the method with the same set multiple times, for instance header(EvenPages|FirstPage),
     * is ok though: the same header instance will be returned every time.
     *
     * Note that all headers will occupy the same height: the height of the
     * tallest header.
     *
     * Specify the contents of the header by calling Header::addElement.
     */
    Header &header(HeaderLocations hl = AllPages);

    /**
     * \return a reference to the footer object.
     * Calling this method makes the report have a footer on the specified pages.
     * See header() for caveats.
     *
     * Specify the contents of the footer by calling Footer::addElement.
     */
    Footer &footer(HeaderLocations hl = AllPages);

    /**
     * Associate a text string with the id of a text or html element.
     *
     * \param id the id of the text or html element, specified using TextElement::setId().
     * \param value the text for this element, which will appear in the report.
     * The value can include newlines.
     * If associateTextValue was already called with the id \p id, the new value
     * replaces the old one.
     */
    void associateTextValue(const QString &id, const QString &value);

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

    //! \return the location for the given header.
    XReports::HeaderLocations headerLocation(XReports::Header *header) const;

    //! \return the location for the given footer.
    XReports::HeaderLocations footerLocation(XReports::Footer *footer) const;

    //! Set the header location
    void setHeaderLocation(HeaderLocations hl, Header *header);

    //! Set the footer location
    void setFooterLocation(HeaderLocations hl, Footer *footer);

    //! \internal Returns the paper size in pixels.
    QSizeF paperSize() const;

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
    void setHeaderChanged();

    QString asHtml() const;

    void setupPrinter(QPrinter *printer);

private:
    Q_DISABLE_COPY(Report)
    friend class ImageElement; // for textDocumentWidth()
    friend class ChartElement; // for textDocumentWidth()
    friend class Header; // doc(), headerChanged()
    friend class ReportPrivate; // setupPrinter
    std::unique_ptr<ReportPrivate> d;
};

}

#endif /* XREPORTSREPORT_H */
