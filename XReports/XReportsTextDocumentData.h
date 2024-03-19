#ifndef XREPORTS_TEXTDOCUMENTDATA_H
#define XREPORTS_TEXTDOCUMENTDATA_H

#include "XReportsAutoTableElement.h"
#include "XReportsHLineTextObject.h"

#include <QMultiMap>
#include <QTextCursor>
#include <QTextDocument>

namespace XReports {

/**
 * @internal
 * Contains a QTextDocument and its associated data.
 */
class TextDocumentData
{
    typedef QHash<QTextTable*, AutoTableElement> AutoTablesMaps;

    enum ElementType
    {
        ElementTypeText,
        ElementTypeHtml
    };

    struct TextValueData
    {
        int         valueLength;
        ElementType elementType;
        QTextCursor cursor;
        int         initialPosition;

        TextValueData(int valLength, bool html, int pos) {
            valueLength     = valLength;
            elementType     = html ? ElementTypeHtml : ElementTypeText;
            initialPosition = pos;
        }
    };

    struct TextDocumentDataPrivate
    {
        QTextDocument                     document;
        QMultiMap<QString, TextValueData> textValueCursors;
        QList<QTextTable *>               tables;
        QList<QString>                    resourceNames;
        bool                              usesTabPositions   = false;
        bool                              hasResizableImages = false;
    };

public:
    enum ModificationMode
    {
        Append,
        Modify
    };

    explicit TextDocumentData() {
        d.document.setUseDesignMetrics(true);
        HLineTextObject::registerHLineObjectHandler(&(d.document));
    }

    ~TextDocumentData() {}

    TextDocumentData(const TextDocumentData &) = delete;

    TextDocumentData &operator=(const TextDocumentData &) = delete;

    QTextDocument &document() {
        return d.document;
    }

    const QTextDocument &document() const {
        return d.document;
    }

    void setUsesTabPositions(bool usesTabs) {
        d.usesTabPositions = usesTabs;
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

    void aboutToModifyContents(ModificationMode mode) {
        resolveCursorPositions(mode);
    }

    void updateTextValue(const QString &id, const QString &newValue);

    void setTextWidth(qreal w) {
        if (w != d.document.textWidth()) {
            d.document.setTextWidth(w);
            updatePercentSizes(d.document.size());
        }
    }

    void setPageSize(QSizeF size) {
        if (size != d.document.pageSize()) {
            d.document.setPageSize(size);
            updatePercentSizes(size);
        }
    }

    void scaleFontsBy(qreal factor); // TODO remove?

    void updatePercentSizes(QSizeF size);

    void setTextValueMarker(int pos, const QString &id, int valueLength, bool html) {
        TextValueData val(valueLength, html, pos);
        d.textValueCursors.insert(id, val);
    }

    void registerTable(QTextTable *table) {
        d.tables.append(table);
    }

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
    void resolveCursorPositions(ModificationMode mode);

    void setFontSizeHelper(QTextCursor &lastCursor, int endPosition, qreal pointSize, qreal factor);

    void dumpTextValueCursors() const;

private:
    TextDocumentDataPrivate d;
};

} // namespace XReports

#endif /* XREPORTS_TEXTDOCUMENTDATA_P_H */
