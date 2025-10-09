#ifndef XREPORTS_TEXTDOCUMENT_H
#define XREPORTS_TEXTDOCUMENT_H

#include "XReportsHLineTextObject.h"

#include "XReportsReport.h"

#include <QFont>
#include <QMap>
#include <QTextCursor>
#include <QTextDocument>
#include <QTextFormat>

QT_BEGIN_NAMESPACE
class QTextImageFormat;
class QTextTable;
QT_END_NAMESPACE

namespace XReports {

/**
 * @internal  (exported for unit tests)
 * Contains a QTextDocument and its associated data
 */
class TextDocument
{
    struct TextDocumentPrivate
    {
        QTextDocument  document;
        QList<QString> resourceNames;
        bool           hasResizableImages = false;
    };

public:
    explicit TextDocument() {
        d.document.setUseDesignMetrics(true);
        HLineTextObject::registerHLineObjectHandler(&(d.document));
    }

    ~TextDocument() {}

    // like QTextDocument::setTextWidth but also takes care of objects with % sizes
    void setTextWidth(qreal w) {
        if (w != d.document.textWidth()) {
            d.document.setTextWidth(w);
            updatePercentSizes(d.document.size());
        }
    }

    // like QTextDocument::setPageSize but also takes care of objects with % sizes
    void setPageSize(QSizeF size) {
        if (size != d.document.pageSize()) {
            d.document.setPageSize(size);
            updatePercentSizes(size);
        }
    }

    QFont defaultFont() const {
        return d.document.defaultFont();
    }

    QTextDocument &document() {
        return d.document;
    }

    const QTextDocument &document() const {
        return d.document;
    }

    void saveResourcesToFiles() {
        for (const QString &name: d.resourceNames) {
            const QVariant v = d.document.resource(QTextDocument::ImageResource, QUrl(name));
            QPixmap pix = v.value<QPixmap>();
            if (!pix.isNull()) {
                pix.save(name);
            }
        }
    }

    void updatePercentSizes(QSizeF size);

    QString asHtml() const {
#if QT_VERSION < QT_VERSION_CHECK(6, 0, 0)
        QString htmlText = d.document.toHtml("utf-8");
#else
        QString htmlText = d.document.toHtml();
#endif
        htmlText.remove(QLatin1String("margin-top:0px; margin-bottom:0px; margin-left:0px; margin-right:0px; "));
        htmlText.remove(QLatin1String("-qt-block-indent:0; "));
        htmlText.remove(QLatin1String("text-indent:0px;"));
        htmlText.remove(QLatin1String("style=\"\""));
        htmlText.remove(QLatin1String("style=\" \""));
        return htmlText;
    }

    void addResourceName(const QString &resourceName) {
        d.resourceNames.append(resourceName);
    }

    void setHasResizableImages() {
        d.hasResizableImages = true;
    }

    static void updatePercentSize(QTextImageFormat &format, QSizeF size);

private:
    void setFontSizeHelper(QTextCursor &lastCursor, int endPosition, qreal pointSize, qreal factor);

private:
    TextDocumentPrivate d;
};

}

#endif /* XREPORTS_TEXTDOCUMENT_H */
