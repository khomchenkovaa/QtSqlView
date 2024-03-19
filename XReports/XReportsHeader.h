#ifndef XREPORTS_HEADER_H
#define XREPORTS_HEADER_H

#include "XReportsEnums.h"
#include "XReportsTextDocument.h"
#include "XReportsReportBuilder.h"

#include <QTextOption>

namespace XReports {

/**
 * This class presents a header or footer in a report.
 * To add an element to a header or footer, first get hold of the
 * right header/footer using Report::header() or Report::footer(),
 * then add elements to it.
 */
class Header
{
    struct HeaderPrivate
    {
        TextDocument  textDocument;
        ReportBuilder builder;
        Report       *report;

        explicit HeaderPrivate(XReports::Report *report)
            : textDocument()
            , builder(textDocument.contentDocumentData(), QTextCursor(&textDocument.contentDocument()), report)
            , report(report)
        { }
    };

public:
    //! Sets the default font used for text in this header
    void setDefaultFont(const QFont &font) {
        QFont f(font);
        f.setStyleStrategy(QFont::ForceOutline); // bitmap fonts look awful in printed documents
        d.textDocument.contentDocument().setDefaultFont(f);
        d.builder.setDefaultFont(f);
    }

    //! \return the default font used for text in this header
    QFont defaultFont() {
        return d.textDocument.contentDocument().defaultFont();
    }

    //! Adds an element to the report, next to the previous element, in the same paragraph.
    void addInlineElement(const Element &element) {
        d.builder.addInlineElement(element);
        d.report->setHeaderChanged();
    }

    /**
     * Adds an element to the report, creating a new paragraph for it.
     * You can specify the alignment of that paragraph.
     */
    void addElement(const Element &element, Qt::AlignmentFlag horizontalAlignment = Qt::AlignLeft) {
        d.builder.addBlockElement(element, horizontalAlignment);
        d.report->setHeaderChanged();
    }

    /**
     * Adds a variable in the text of the current paragraph.
     * Use addInlineElement before and/or after to complete the text of the paragraph.
     * Example: "Page 1 / 3":
     * @code
        header.addInlineElement( XReports::TextElement( "Page " ) );
        header.addVariable( XReports::PageNumber );
        header.addInlineElement( XReports::TextElement( " / " ) );
        header.addVariable( XReports::PageCount );
     * @endcode
     */
    void addVariable(VariableType variable) {
        d.builder.addVariable(variable);
    }

    /**
     * Adds vertical spacing between paragraphs.
     * Make sure to call addElement after that, not addInlineElement.
     * @param space the space in millimeters
     */
    void addVerticalSpacing(qreal space) {
        d.builder.addVerticalSpacing(space);
        d.report->setHeaderChanged();
    }

    /**
     * Set the list of tabs (tabulations) to use when adding block elements.
     * Those tabs will be set for any block element that is added from now on,
     * in this header.
     * For more details see Report::setTabPositions.
     */
    void setTabPositions(const QList<QTextOption::Tab> &tabs) {
        d.builder.setTabPositions(tabs);
    }

    //! \internal Returns the current builder cursor position.
    int currentPosition() {
        return d.builder.currentPosition();
    }

private:
    friend class Report;
    friend class ReportPrivate;

    explicit Header(Report *report) : d(report) {
        d.textDocument.contentDocument().setDefaultFont(report->defaultFont());
    }

//    friend class Test;
    friend class HeaderMap;

    TextDocument &doc() {
        return d.textDocument;
    }

    void preparePaintingPage(int pageNumber) {
        QTextCursor c(&d.textDocument.contentDocument());
        do {
            c.movePosition(QTextCursor::NextCharacter);
            QTextCharFormat format = c.charFormat();
            if (format.hasProperty(VariableTypeProperty)) {
                // go back one char, due to charFormat() being the format of the character -before- the cursor!
                c.movePosition(QTextCursor::PreviousCharacter);
                const VariableType variableType = static_cast<VariableType>(format.property(VariableTypeProperty).toInt());
                const int oldLength = format.property(VariableLengthProperty).toInt();
                const QString value = XReports::variableValue(pageNumber, d.report->numberOfPages(), variableType);
                const int startPos = c.position();
                c.setPosition(c.position() + oldLength, QTextCursor::KeepAnchor);
                c.insertText(value); // update variable value
                // update marker
                setVariableMarker(d.textDocument.contentDocument(), startPos, variableType, value.length());
            }
        } while (!c.atEnd());
    }

    Q_DISABLE_COPY(Header)

private:
    HeaderPrivate d;
};

typedef Header Footer;

}

#endif /* XREPORTS_HEADER_H */
