#include "XReportsImageElement.h"
#include "XReportsUtils.h"

#include "XReportsReport_p.h"

void XReports::ImageElement::build(ReportBuilder &builder) const
{
    if (d.pixmapSize.isNull())
        return;

    static int imageNumber = 0;
    const QString name = QStringLiteral("image%1.png").arg(++imageNumber);
    builder.document().addResource(QTextDocument::ImageResource, QUrl(name), d.pixmap);
    builder.textDocument().addResourceName(name);

    QTextImageFormat imageFormat;
    imageFormat.setName(name);
    imageFormat.setWidth(d.pixmapSize.width());
    imageFormat.setHeight(d.pixmapSize.height());
#if QT_VERSION >= QT_VERSION_CHECK(5, 12, 0)
    // Another workaround for https://codereview.qt-project.org/c/qt/qtbase/+/369642
    imageFormat.setQuality(100);
#endif

    if (d.width) {
        if (d.unit == Millimeters) {
            const qreal pixelWidth = mmToPixels(d.width);
            const qreal pixelHeight = pixelWidth * imageFormat.height() / imageFormat.width();
            imageFormat.setWidth(pixelWidth);
            imageFormat.setHeight(pixelHeight);
        } else {
            imageFormat.setProperty(ResizableImageProperty, QString(QLatin1Char('W') + QString::number(d.width)));
            builder.textDocument().setHasResizableImages();
        }
    } else if (d.height) {
        if (d.unit == Millimeters) {
            const qreal pixelHeight = qRound(mmToPixels(d.height));
            const qreal pixelWidth = pixelHeight * imageFormat.width() / imageFormat.height();
            imageFormat.setHeight(pixelHeight);
            imageFormat.setWidth(pixelWidth);
        } else {
            imageFormat.setProperty(ResizableImageProperty, QString(QLatin1Char('H') + QString::number(d.height)));
            builder.textDocument().setHasResizableImages();
            // can't calc size yet, will be done at layouting time... hopefully.
        }
    } else if (d.fitToPage) {
        imageFormat.setProperty(ResizableImageProperty, QString(QLatin1Char('T')));
        builder.textDocument().setHasResizableImages();
    }

    QTextCursor &cursor = builder.cursor();
    cursor.insertImage(imageFormat);
}
