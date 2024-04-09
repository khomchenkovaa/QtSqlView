#include "XReportsAutoTableElement.h"
#include "XReportsUtils.h"
#include "XReportsReportBuilder.h"
#include "XReportsTextDocument.h"

#include <QAbstractTextDocumentLayout>
#include <QDebug>
#include <QTextTable>
#include <QUrl>

void XReports::TextDocument::updatePercentSizes(QSizeF size)
{
    if (!d.hasResizableImages) {
        return;
    }
    QTextCursor c(&(d.document));
    c.beginEditBlock();
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
    c.endEditBlock();
}

void XReports::TextDocument::updatePercentSize(QTextImageFormat &imageFormat, QSizeF size)
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

void XReports::TextDocument::setFontSizeHelper(QTextCursor &lastCursor, int endPosition, qreal pointSize, qreal factor)
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
