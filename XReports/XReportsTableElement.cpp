#include "XReportsTableElement.h"
#include "XReportsTextDocument.h"

#include <QTextTableCell>

void XReports::TableElement::build(ReportBuilder &builder) const
{
    if (d.m_cellContentMap.isEmpty())
        return;

    QTextCursor &textDocCursor = builder.cursor();

    int rowCount;
    int columnCount;
    d.m_cellContentMap.getSize(rowCount, columnCount);

    QTextTableFormat tableFormat;
    tableFormat.setHeaderRowCount(d.m_headerRowCount);
    tableFormat.setProperty(XReports::HeaderColumnsProperty, d.m_headerColumnCount);
    tableFormat.setAlignment(textDocCursor.blockFormat().alignment());
    tableFormat.setBackground(background());
    fillTableFormat(tableFormat, textDocCursor);
    QTextCharFormat charFormat = textDocCursor.charFormat();

    QTextTable *textTable = textDocCursor.insertTable(rowCount, columnCount, tableFormat);

    CellContentMap::const_iterator it = d.m_cellContentMap.constBegin();
    for (; it != d.m_cellContentMap.constEnd(); ++it) {
        const int row = it.key().first;
        const int column = it.key().second;
        const CellElement &cell = it.value();
        if (cell.columnSpan() > 1 || cell.rowSpan() > 1)
            textTable->mergeCells(row, column, cell.rowSpan(), cell.columnSpan());
        QTextTableCell tableCell = textTable->cellAt(row, column);
        Q_ASSERT(tableCell.isValid());
        QTextCursor cellCursor = tableCell.firstCursorPosition();
        QTextCharFormat tableCellFormat = charFormat;
        if (cell.background().style() != Qt::NoBrush)
            tableCellFormat.setBackground(cell.background());
        tableCellFormat.setTableCellColumnSpan(cell.columnSpan());
        tableCellFormat.setTableCellRowSpan(cell.rowSpan());
        tableCell.setFormat(tableCellFormat);
        cellCursor.setCharFormat(tableCellFormat);
        ReportBuilder cellBuilder(builder.currentDocumentData(), cellCursor, builder.report());
        cellBuilder.copyStateFrom(builder);
        cellBuilder.setDefaultFont(charFormat.font());
        cell.build(cellBuilder);
    }

    textDocCursor.movePosition(QTextCursor::End);

    builder.currentDocumentData().registerTable(textTable);
}
