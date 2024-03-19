#ifndef XREPORTS_REPORTBUILDER_H
#define XREPORTS_REPORTBUILDER_H

#include "XReportsTextDocumentData.h"
#include "XReportsReport.h"

#include <QTextCursor>

namespace XReports {

/**
 * @internal
 */
class ReportBuilder
{
    struct ReportBuilderPrivate {
        TextDocumentData &contentDocument;
        QTextCursor cursor;
        QTextCursor layoutDocCursor;
        QList<QTextOption::Tab> tabPositions;
        qreal   leftMargin   = 0;
        qreal   rightMargin  = 0;
        qreal   topMargin    = 0;
        qreal   bottomMargin = 0;
        Report *report;
        QFont   defaultFont;
        bool    first = true;

        ReportBuilderPrivate(TextDocumentData &contentDocument, const QTextCursor &cursor, Report *report)
            : contentDocument (contentDocument)
            , cursor(cursor)
            , report(report)
        {}

        void setMargins(qreal left, qreal top, qreal right, qreal bottom) {
            leftMargin   = mmToPixels(left);
            topMargin    = mmToPixels(top);
            rightMargin  = mmToPixels(right);
            bottomMargin = mmToPixels(bottom);
        }
    };

public:
    ReportBuilder(TextDocumentData &contentDocument, const QTextCursor &cursor, Report *report)
        : d(contentDocument, cursor, report)
    {}

    virtual ~ReportBuilder() {}

    QTextCursor &cursor() {
        d.contentDocument.aboutToModifyContents(TextDocumentData::Append);
        return d.cursor;
    }

    Report *report() {
        return d.report;
    }

    // Warning, changes to those virtual methods must be propagated to CellReportBuilder.
    virtual void addPageBreak() {
        QTextBlockFormat blockFormat = d.cursor.blockFormat();
        blockFormat.setPageBreakPolicy(QTextFormat::PageBreak_AlwaysAfter);
        d.cursor.setBlockFormat(blockFormat);
    }

    virtual void addInlineElement(const Element &element);

    virtual void addBlockElement(const Element &element, Qt::AlignmentFlag horizontalAlignment, const QColor &backgroundColor = QColor());

    virtual void addVariable(XReports::VariableType variable);

    virtual void addVerticalSpacing(qreal space);

    virtual void insertFragment(const QTextDocumentFragment &fragment) {
        d.cursor.insertFragment(fragment);
    }

    TextDocumentData &currentDocumentData() {
        return d.contentDocument;
    }

    QTextDocument &currentDocument() {
        return currentDocumentData().document();
    }

    // Store default font in builder, so that toplevel text elements use the report's default font
    // but table cells use the table's default font (if any)
    void setDefaultFont(const QFont &font) {
        d.defaultFont = font;
    }

    QFont defaultFont() const {
        return d.defaultFont;
    }

    // should not be used directly, apart for creating another builder (e.g. cell builder)
    TextDocumentData &contentDocumentData() {
        return d.contentDocument;
    }

    QTextCursor &contentDocumentCursor() {
        return d.cursor;
    }

    void setupBlockFormat(QTextBlockFormat &blockFormat) const {
        blockFormat.setTabPositions(d.tabPositions);
        blockFormat.setLeftMargin(d.leftMargin);
        blockFormat.setRightMargin(d.rightMargin);
        blockFormat.setTopMargin(d.topMargin);
        blockFormat.setBottomMargin(d.bottomMargin);
    }

    void setTabPositions(const QList<QTextOption::Tab> &tabs) { // in mm
        QList<QTextOption::Tab> tabsInPixels;
        for (QTextOption::Tab tab: tabs) {
            tab.position = mmToPixels(tab.position);
            tabsInPixels.append(tab);
        }
        d.tabPositions = tabsInPixels;
        d.contentDocument.setUsesTabPositions(true);
    }
    // const QList<QTextOption::Tab>& tabPositions() const { return m_tabPositions; }

    void setParagraphMargins(qreal left, qreal top, qreal right, qreal bottom) {
        d.setMargins(left, top, right, bottom); // in mm
    }

    void copyStateFrom(const ReportBuilder &parentBuilder) {
        d.tabPositions = parentBuilder.d.tabPositions;
        d.leftMargin   = parentBuilder.d.leftMargin;
        d.rightMargin  = parentBuilder.d.rightMargin;
        d.topMargin    = parentBuilder.d.topMargin;
        d.bottomMargin = parentBuilder.d.bottomMargin;
        d.defaultFont  = parentBuilder.d.defaultFont;
    }

    int currentPosition() {
        return d.cursor.position();
    }

private:
    ReportBuilder(const ReportBuilder &other) = delete;

    ReportBuilder &operator=(const ReportBuilder &other) = delete;

private:
    ReportBuilderPrivate d;
};

} // namespace XReports

/*********************************************************/

QDebug operator<<(QDebug &dbg, const QTextOption::Tab &tab);

#endif /* XREPORTS_REPORTBUILDER_H */
