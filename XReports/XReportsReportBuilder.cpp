#include "XReportsReportBuilder.h"

#include "XReportsUtils.h"
#include "XReportsTextElement.h"

#include <QDebug>
#include <QTextBlock>

/***********************************************************/

void XReports::ReportBuilder::addInlineElement(const Element &element)
{
    QTextCursor &cursor = this->cursor();
    cursor.beginEditBlock();
    // Save/restore the char format, otherwise insertHtml("<font size=\"+8\">My Title</font>")
    // leaves us with a large-font char format, which would affect subsequent insertText().
    // So: char format change must be done inside a single html element, not across.
    // This is also true for charts and images, and variables, which all set things into the char format.
    const QTextCharFormat origCharFormat = cursor.charFormat();
    element.build(*this);
    cursor.setCharFormat(origCharFormat);
    cursor.endEditBlock();
}

/***********************************************************/

void XReports::ReportBuilder::addBlockElement(const Element &element, Qt::AlignmentFlag horizontalAlignment, const QColor &backgroundColor)
{
    QTextCursor &cursor = this->cursor();
    cursor.beginEditBlock();

    if (!d.first) {
        cursor.insertBlock();
    } else {
        d.first = false;
    }

    // Set the default font again, the previous block should have no effect on this one
    QTextCharFormat charFormat = cursor.charFormat();
    charFormat.setFont(d.defaultFont);
    cursor.setCharFormat(charFormat);

    QTextBlockFormat blockFormat;
    blockFormat.setAlignment(horizontalAlignment);
    setupBlockFormat(blockFormat);

    if (backgroundColor.isValid())
        blockFormat.setBackground(backgroundColor);

    cursor.setBlockFormat(blockFormat);

    element.build(*this);

    cursor.setCharFormat(charFormat); // restore, we don't want addElement(bold text) + addInline(normal text) to make the normal text bold
    cursor.endEditBlock();

#if 0 // DEBUG CODE for tab positions
    if (!m_tabPositions.isEmpty()) {
        QTextBlock block = cursor.document()->firstBlock();
        do {
            qDebug() << "addBlock: Looking at block" << block.blockNumber() << "tabs:" << block.blockFormat().tabPositions().count();
            block = block.next();
        } while ( block.isValid() );
    }
#endif
}

/***********************************************************/

void XReports::ReportBuilder::addVariable(XReports::VariableType variable)
{
    QTextCursor &cursor = this->cursor();
    const int charPosition = cursor.position();
    // Don't ask for the value of PageCount yet -- it would create a documentlayout
    // which would make any later insertion into the textdocument much, much slower.
    const QString value = (variable == XReports::PageCount) ? QStringLiteral("UNKNOWN YET") : variableValue(0 /*pageNumber*/, d.report->numberOfPages(), variable);
    XReports::TextElement element(value);

    const QTextCharFormat origCharFormat = cursor.charFormat();

    // Keep the current font (KDRE-91).
    QTextCursor docCursor(&currentDocument());
    docCursor.setPosition(charPosition);
    cursor.setCharFormat(docCursor.charFormat());

    cursor.beginEditBlock();
    element.build(*this);
    cursor.endEditBlock();

    setVariableMarker(currentDocument(), charPosition, variable, value.length());

    cursor.setCharFormat(origCharFormat); // restore the orig format
}

/***********************************************************/

void XReports::ReportBuilder::addVerticalSpacing(qreal space)
{
    QTextCursor &cursor = this->cursor();
    cursor.beginEditBlock();

    if (!d.first) {
        cursor.insertBlock();
    } else {
        d.first = false;
    }

    QTextBlockFormat blockFormat;
    blockFormat.setTopMargin(XReports::mmToPixels(space / 2));
    blockFormat.setBottomMargin(XReports::mmToPixels(space / 2));
    cursor.setBlockFormat(blockFormat);
    QTextCharFormat charFormat;
    charFormat.setFontPointSize(1);
    cursor.setCharFormat(charFormat);
    cursor.insertText(QStringLiteral(" ")); // this ensures the point size 1 is actually used, making the paragraph much smaller
    cursor.endEditBlock();
}

/***********************************************************/

QDebug operator<<(QDebug &dbg, QTextOption::Tab tab) // clazy says: pass by value, small enough
{
    static const char *types[] = {"LeftTab", "RightTab", "CenterTab", "DelimiterTab"};
    dbg.space() << '(' << types[tab.type] << tab.position << "px" << ')';
    return dbg;
}

/***********************************************************/
