#include "XReportsAutoTableElement.h"
#include "XReportsUtils.h"
#include "XReportsReportBuilder.h"
#include "XReportsReport_p.h" // modelForKey

#include <QAbstractItemModel>
#include <QBitArray>
#include <QDateTime>
#include <QDebug>
#include <QIcon>
#include <QTextCursor>
#include <QTextTableCell>
#include <QUrl>
#include <QVector>

namespace XReports {

//! Helper for fillCellFromHeaderData and fillTableCell
class FillCellHelper
{
    QSize iconSize;
    QVariant cellDecoration;
    QVariant cellFont;
    QString cellText;
    QVariant foreground;
    QVariant background;
    Qt::Alignment alignment;
    QVariant decorationAlignment;
    bool nonBreakableLines;
    QSize span;
    QTextCursor cellCursor;

public:
    FillCellHelper(QAbstractItemModel *tableModel, int section, Qt::Orientation orientation, QSize iconSz)
        : iconSize(iconSz)
        , cellDecoration(tableModel->headerData(section, orientation, Qt::DecorationRole))
        , cellFont(tableModel->headerData(section, orientation, Qt::FontRole))
        , cellText(tableModel->headerData(section, orientation, Qt::DisplayRole).toString())
        , foreground(tableModel->headerData(section, orientation, Qt::ForegroundRole))
        , background(tableModel->headerData(section, orientation, Qt::BackgroundRole))
        , alignment(Qt::Alignment(tableModel->headerData(section, orientation, Qt::TextAlignmentRole).toInt()))
        , decorationAlignment(tableModel->headerData(section, orientation, XReports::AutoTableElement::DecorationAlignmentRole))
        , nonBreakableLines(tableModel->headerData(section, orientation, XReports::AutoTableElement::NonBreakableLinesRole).toBool())
        , span(1, 1)
    {
    }
    FillCellHelper(QAbstractItemModel *tableModel, const QModelIndex &index, QSize _span, QSize iconSz)
        : iconSize(iconSz)
        , cellDecoration(tableModel->data(index, Qt::DecorationRole))
        , cellFont(tableModel->data(index, Qt::FontRole))
        , cellText(displayText(tableModel->data(index, Qt::DisplayRole)))
        , foreground(tableModel->data(index, Qt::ForegroundRole))
        , background(tableModel->data(index, Qt::BackgroundRole))
        , alignment(Qt::Alignment(tableModel->data(index, Qt::TextAlignmentRole).toInt()))
        , decorationAlignment(tableModel->data(index, XReports::AutoTableElement::DecorationAlignmentRole))
        , nonBreakableLines(tableModel->data(index, XReports::AutoTableElement::NonBreakableLinesRole).toBool())
        , span(_span)
    {
    }
    void fill(QTextTable *textTable, XReports::ReportBuilder &builder, QTextDocument &textDoc, QTextTableCell &cell);

private:
    void insertDecoration(XReports::ReportBuilder &builder, QTextDocument &textDoc);
};

void FillCellHelper::fill(QTextTable *textTable, XReports::ReportBuilder &builder, QTextDocument &textDoc, QTextTableCell &cell)
{
    cellCursor = cell.firstCursorPosition();
    QTextCharFormat cellFormat = cell.format();
    if (background.canConvert<QBrush>()) {
        cellFormat.setBackground(qvariant_cast<QBrush>(background));
    }
    cellFormat.setVerticalAlignment(XReports::toVerticalAlignment(alignment));
    cell.setFormat(cellFormat);

    QTextBlockFormat blockFormat = cellCursor.blockFormat();
    blockFormat.setAlignment(alignment);
    blockFormat.setNonBreakableLines(nonBreakableLines);
    builder.setupBlockFormat(blockFormat);

    cellCursor.setBlockFormat(blockFormat);

    const bool hasIcon = !cellDecoration.isNull();
    const bool iconAfterText = decorationAlignment.isValid() && (decorationAlignment.toInt() & Qt::AlignRight);
    if (hasIcon && !iconAfterText) {
        insertDecoration(builder, textDoc);
    }

    QTextCharFormat charFormat = cellCursor.charFormat();
    if (cellFont.isValid()) {
        QFont cellQFont = qvariant_cast<QFont>(cellFont);
#if QT_VERSION >= QT_VERSION_CHECK(5, 3, 0)
        charFormat.setFont(cellQFont, QTextCharFormat::FontPropertiesSpecifiedOnly);
#else
        charFormat.setFont(cellQFont);
#endif
    } else {
        charFormat.setFont(builder.defaultFont());
    }
    if (foreground.canConvert<QBrush>()) {
        charFormat.setForeground(qvariant_cast<QBrush>(foreground));
    }
    cellCursor.setCharFormat(charFormat);

    if (hasIcon && !iconAfterText) {
        cellCursor.insertText(QChar::fromLatin1(' ')); // spacing between icon and text
    }

    // qDebug() << cellText;
    if (cellText.startsWith(QLatin1String("<qt>")) || cellText.startsWith(QLatin1String("<html>")))
        cellCursor.insertHtml(cellText);
    else
        cellCursor.insertText(cellText);

    if (hasIcon && iconAfterText) {
        cellCursor.insertText(QChar::fromLatin1(' ')); // spacing between icon and text
        insertDecoration(builder, textDoc);
    }

    if (span.width() > 1 || span.height() > 1)
        textTable->mergeCells(cell.row(), cell.column(), span.height(), span.width());
}

void FillCellHelper::insertDecoration(XReports::ReportBuilder &builder, QTextDocument &textDoc)
{
    QImage img = qvariant_cast<QImage>(cellDecoration);
    if (img.isNull()) {
        img = qvariant_cast<QIcon>(cellDecoration).pixmap(iconSize).toImage();
    }
    if (!img.isNull()) {
        static int imageNumber = 0;
        const QString name = QStringLiteral("cell-image%1.png").arg(++imageNumber);
        textDoc.addResource(QTextDocument::ImageResource, QUrl(name), img);
        builder.currentDocumentData().addResourceName(name);
        cellCursor.insertImage(name);
    }
}

} // namespace XReports

/******************************************************/

void XReports::AutoTableElement::build(ReportBuilder &builder) const
{
    if (!d.m_tableModel) {
        return;
    }
    QTextDocument &textDoc = builder.currentDocument();
    QTextCursor &textDocCursor = builder.cursor();
    textDocCursor.beginEditBlock();

    QTextTableFormat tableFormat;
    const int headerRowCount = d.m_horizontalHeaderVisible ? 1 : 0;
    const int headerColumnCount = d.m_verticalHeaderVisible ? 1 : 0;
    tableFormat.setHeaderRowCount(headerRowCount);
    tableFormat.setProperty(XReports::HeaderColumnsProperty, headerColumnCount);

    tableFormat.setAlignment(textDocCursor.blockFormat().alignment());
    fillTableFormat(tableFormat, textDocCursor);

    while (d.m_tableModel->canFetchMore(QModelIndex()))
        d.m_tableModel->fetchMore(QModelIndex());

    const int rows = d.m_tableModel->rowCount();
    const int columns = d.m_tableModel->columnCount();

    QTextTable *textTable = textDocCursor.insertTable(rows + headerRowCount, columns + headerColumnCount, tableFormat);

    QTextCharFormat tableHeaderFormat;
    tableHeaderFormat.setBackground(d.m_headerBackground);
    // qDebug( "rows = %d, columns = %d", textTable->rows(), textTable->columns() );

    if (d.m_horizontalHeaderVisible) {
        for (int column = 0; column < columns; column++) {
            QTextTableCell cell = textTable->cellAt(0, column + headerColumnCount);
            Q_ASSERT(cell.isValid());
            cell.setFormat(tableHeaderFormat);
            fillCellFromHeaderData(column, Qt::Horizontal, cell, textDoc, textTable, builder);
        }
    }

    if (d.m_verticalHeaderVisible) {
        for (int row = 0; row < rows; row++) {
            QTextTableCell cell = textTable->cellAt(row + headerRowCount, 0);
            Q_ASSERT(cell.isValid());
            cell.setFormat(tableHeaderFormat);
            fillCellFromHeaderData(row, Qt::Vertical, cell, textDoc, textTable, builder);
        }
    }

    QVector<QBitArray> coveredCells;
    coveredCells.resize(rows);
    for (int row = 0; row < rows; row++)
        coveredCells[row].resize(columns);

    // The normal data
    for (int row = 0; row < rows; row++) {
        for (int column = 0; column < columns; column++) {
            if (coveredCells[row].testBit(column))
                continue;
            QTextTableCell cell = textTable->cellAt(row + headerRowCount, column + headerColumnCount);
            Q_ASSERT(cell.isValid());
            const QSize span = fillTableCell(row, column, cell, textDoc, textTable, builder);
            if (span.isValid()) {
                for (int r = row; r < row + span.height() && r < rows; ++r) {
                    for (int c = column; c < column + span.width() && c < columns; ++c) {
                        coveredCells[r].setBit(c);
                    }
                }
            }
        }
    }

    textDocCursor.movePosition(QTextCursor::End);
    textDocCursor.endEditBlock();
}

/******************************************************/

void XReports::AutoTableElement::fillCellFromHeaderData(int section, Qt::Orientation orientation, QTextTableCell &cell, QTextDocument &textDoc, QTextTable *textTable,
                                                                ReportBuilder &builder) const
{
    FillCellHelper helper(d.m_tableModel, section, orientation, d.m_iconSize);
    helper.fill(textTable, builder, textDoc, cell);
}

/******************************************************/

QSize XReports::AutoTableElement::fillTableCell(int row, int column, QTextTableCell &cell, QTextDocument &textDoc, QTextTable *textTable, ReportBuilder &builder) const
{
    const QModelIndex index = d.m_tableModel->index(row, column);
    const QSize span = d.m_tableModel->span(index);
    FillCellHelper helper(d.m_tableModel, index, span, d.m_iconSize);
    helper.fill(textTable, builder, textDoc, cell);
    return span;
}

/******************************************************/
