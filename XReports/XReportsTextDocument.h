#ifndef XREPORTS_TEXTDOCUMENT_H
#define XREPORTS_TEXTDOCUMENT_H

#include "XReportsAutoTableElement.h"
#include "XReportsTextDocumentData.h"

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
 * There is one instance of TextDocument in the report,
 * but also one in each header/footer.
 */
class TextDocument
{
public:
    explicit TextDocument() {}

    ~TextDocument() {}

    // like QTextDocument::setTextWidth but also takes care of objects with % sizes
    void layoutWithTextWidth(qreal w) {
        m_contentDocument.setTextWidth(w);
    }

    // like QTextDocument::setPageSize but also takes care of objects with % sizes
    void setPageSize(QSizeF size) {
        m_contentDocument.setPageSize(size);
    }

    void updateTextValue(const QString &id, const QString &newValue) {
        m_contentDocument.updateTextValue(id, newValue);
    }

    void scaleFontsBy(qreal factor) {
        m_contentDocument.scaleFontsBy(factor);
    }

    QFont defaultFont() const {
        return m_contentDocument.document().defaultFont();
    }

    QTextDocument &contentDocument() {
        return contentDocumentData().document();
    }

    TextDocumentData &contentDocumentData() {
        return m_contentDocument;
    }

    QString asHtml() const {
        return m_contentDocument.asHtml();
    }

private:
    TextDocumentData m_contentDocument;
};

}

#endif /* XREPORTS_TEXTDOCUMENT_H */
