#include "XReportsAutoTableElement.h"
#include "XReportsUtils.h"
#include "XReportsReportBuilder.h"
#include "XReportsTextDocumentData.h"

#include <QAbstractTextDocumentLayout>
#include <QDebug>
#include <QTextTable>
#include <QUrl>

void XReports::TextDocumentData::dumpTextValueCursors() const
{
    qDebug() << "Text value cursors:  (document size=" << d.document.characterCount() << ")";
    QMultiMap<QString, TextValueData>::const_iterator it = d.textValueCursors.begin();
    while (it != d.textValueCursors.end()) {
        const TextValueData &data = *it;
        if (data.cursor.isNull()) {
            qDebug() << it.key() << "unresolved cursor at pos" << data.initialPosition;
        } else {
            qDebug() << it.key() << "QTextCursor currently at pos" << data.cursor.position() << "length" << data.valueLength;
        }
        ++it;
    }
}

void XReports::TextDocumentData::resolveCursorPositions(ModificationMode mode)
{
    // We have to use QTextCursor in TextValueData so that it gets updated when
    // we modify the document later on, but we can't just store the QTextCursor
    // at insertion time; that cursor would be moved to the end of the document
    // while the insertion keeps happening...
    auto it = d.textValueCursors.begin();
    for (; it != d.textValueCursors.end(); ++it) {
        TextValueData &data = *it;
        if (data.cursor.isNull()) {
            // When appending, leave cursors "at end of document" unresolved.
            // Otherwise they'll keep moving with insertions.
            if (mode == Append && data.initialPosition >= d.document.characterCount() - 1) {
                continue;
            }
            data.cursor = QTextCursor(&(d.document));
            data.cursor.setPosition(data.initialPosition);
            // qDebug() << "Cursor for" << it.key() << "resolved at position" << data.initialPosition;
        }
    }
    // dumpTextValueCursors();
}

void XReports::TextDocumentData::updateTextValue(const QString &id, const QString &newValue)
{
    aboutToModifyContents(Modify);

    // qDebug() << "updateTextValue: looking for id" << id << "in doc" << m_document;

    QMultiMap<QString, TextValueData>::iterator it = d.textValueCursors.find(id);
    while (it != d.textValueCursors.end() && it.key() == id) {
        TextValueData &data = *it;
        // qDebug() << "Found at position" << data.cursor.position() << "length" << data.valueLength << "replacing with new value" << newValue;

        QTextCursor c(data.cursor);
        const int oldPos = data.cursor.position();
        c.setPosition(oldPos + data.valueLength, QTextCursor::KeepAnchor);
        const bool html = data.elementType == ElementTypeHtml;
        if (html)
            c.insertHtml(newValue);
        else
            c.insertText(newValue);
        // update data
        data.valueLength = c.position() - oldPos;
        data.cursor.setPosition(oldPos);
        // qDebug() << " stored new length" << data.valueLength;

        ++it;
    }

    // dumpTextValueCursors();
}

void XReports::TextDocumentData::updatePercentSizes(QSizeF size)
{
    if (!d.hasResizableImages && !d.usesTabPositions) {
        return;
    }
    QTextCursor c(&(d.document));
    c.beginEditBlock();
    if (d.hasResizableImages) {
        do {
            c.movePosition(QTextCursor::NextCharacter);
            QTextCharFormat format = c.charFormat();
            if (format.hasProperty(ResizableImageProperty)) {
                Q_ASSERT(format.isImageFormat());
                QTextImageFormat imageFormat = format.toImageFormat();
                updatePercentSize(imageFormat, size);
                // qDebug() << "updatePercentSizes: setting image to " << imageFormat.width() << "," << imageFormat.height();
                c.movePosition(QTextCursor::PreviousCharacter, QTextCursor::KeepAnchor);
                c.setCharFormat(imageFormat);
                c.movePosition(QTextCursor::NextCharacter);
            }
        } while (!c.atEnd());
    }

    if (d.usesTabPositions) {
        QTextFrameFormat rootFrameFormat = d.document.rootFrame()->frameFormat();
        const qreal rootFrameMargins = rootFrameFormat.leftMargin() + rootFrameFormat.rightMargin();
        QTextBlock block = d.document.firstBlock();
        do {
            QTextBlockFormat blockFormat = block.blockFormat();
            QList<QTextOption::Tab> tabs = blockFormat.tabPositions();
            // qDebug() << "Looking at block" << block.blockNumber() << "tabs:" << tabs.count();
            if (!tabs.isEmpty()) {
                for (int i = 0; i < tabs.count(); ++i) {
                    QTextOption::Tab &tab = tabs[i];
                    if (tab.delimiter == QLatin1Char('P') /* means Page -- see rightAlignedTab*/) {
                        if (tab.type == QTextOption::RightTab) {
                            // qDebug() << "Adjusted RightTab from" << tab.position << "to" << size.width();
                            tab.position = size.width() - rootFrameMargins;
                        } else if (tab.type == QTextOption::CenterTab) {
                            tab.position = (size.width() - rootFrameMargins) / 2;
                        }
                    }
                }
                blockFormat.setTabPositions(tabs);
                // qDebug() << "Adjusted tabs:" << tabs;
                c.setPosition(block.position());
                c.setBlockFormat(blockFormat);
            }
            block = block.next();
        } while (block.isValid());
    }
    c.endEditBlock();
}

void XReports::TextDocumentData::updatePercentSize(QTextImageFormat &imageFormat, QSizeF size)
{
    // "W50" means W=50%.  "H60" means H=60%.
    QString prop = imageFormat.property(ResizableImageProperty).toString();
    const qreal imageRatio = imageFormat.height() / imageFormat.width();
    const qreal pageWidth = size.width();
    const qreal pageHeight = size.height();
    const qreal pageRatio = pageWidth ? pageHeight / pageWidth : 0;
    if (prop[0] == QLatin1Char('T')) {
        // qDebug() << "updatePercentSize fitToPage" << imageRatio << pageRatio;
        if (imageRatio < pageRatio) {
            prop = QStringLiteral("W100");
        } else {
            prop = QStringLiteral("H100");
        }
    }
    const qreal percent = prop.mid(1).toDouble();
    switch (prop[0].toLatin1()) {
    case 'W': {
        const qreal newWidth = pageWidth * percent / 100.0;
        imageFormat.setWidth(newWidth);
        imageFormat.setHeight(newWidth * imageRatio);
        // ### I needed to add this -2 here for 100%-width images to fit in
        if (percent == 100.0)
            imageFormat.setWidth(imageFormat.width() - 2);
    } break;
    case 'H':
        imageFormat.setHeight(pageHeight * percent / 100.0);
        // ### I needed to add -6 here for 100%-height images to fit in (with Qt-4.4)
        // and it became -9 with Qt-4.5, and even QtSw doesn't know why.
        // Task number 241890
        if (percent == 100.0)
            imageFormat.setHeight(imageFormat.height() - 10);
        imageFormat.setWidth(imageRatio ? imageFormat.height() / imageRatio : 0);
        // qDebug() << "updatePercentSize" << size << "->" << imageFormat.width() << "x" << imageFormat.height();
        break;
    default:
        qWarning("Unhandled image format property type - internal error");
    }
}

void XReports::TextDocumentData::scaleFontsBy(qreal factor)
{
    QTextCursor cursor(&d.document);
    qreal currentPointSize = -1.0;
    QTextCursor lastCursor(&d.document);
    Q_FOREVER
    {
        qreal cursorFontPointSize = cursor.charFormat().fontPointSize();
        // qDebug() << cursorFontPointSize << "last=" << currentPointSize << cursor.block().text() << "position=" << cursor.position();
        if (cursorFontPointSize != currentPointSize) {
            if (currentPointSize != -1.0) {
                setFontSizeHelper(lastCursor, cursor.position() - 1, currentPointSize, factor);
                lastCursor.setPosition(cursor.position() - 1, QTextCursor::MoveAnchor);
            }
            currentPointSize = cursorFontPointSize;
        }
        if (cursor.atEnd())
            break;
        cursor.movePosition(QTextCursor::NextCharacter);
    }
    if (currentPointSize != -1.0) {
        setFontSizeHelper(lastCursor, cursor.position(), currentPointSize, factor);
    }

    // Also adjust the padding in the cells so that it remains proportional,
    // and the column constraints.
    Q_FOREACH (QTextTable *table, d.tables) {
        QTextTableFormat format = table->format();
        format.setCellPadding(format.cellPadding() * factor);

        QVector<QTextLength> constraints = format.columnWidthConstraints();
        for (int i = 0; i < constraints.size(); ++i) {
            if (constraints[i].type() == QTextLength::FixedLength) {
                constraints[i] = QTextLength(QTextLength::FixedLength, constraints[i].rawValue() * factor);
            }
        }
        format.setColumnWidthConstraints(constraints);

        table->setFormat(format);
    }
}

void XReports::TextDocumentData::setFontSizeHelper(QTextCursor &lastCursor, int endPosition, qreal pointSize, qreal factor)
{
    if (pointSize == 0) {
        pointSize = d.document.defaultFont().pointSize();
    }
    pointSize *= factor;
    QTextCharFormat newFormat;
    newFormat.setFontPointSize(pointSize);
    // qDebug() << "Applying" << pointSize << "from" << lastCursor.position() << "to" << endPosition;
    lastCursor.setPosition(endPosition, QTextCursor::KeepAnchor);
    lastCursor.mergeCharFormat(newFormat);
}
