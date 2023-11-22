#ifndef REPORTHTMLELEMENT_H
#define REPORTHTMLELEMENT_H

#include "reportelement.h"

#include <QtCore/QString>

namespace Report {

/**
 * The Report::HtmlElement class represents any part of the report that can be expressed in HTML.
 * This is one way to insert rich text into the report (the other way is with TextElement).
 * This can be used to create bullet-point lists and numbered lists.
 */
class HtmlElement : public Element
{
public:
    /**
     * Creates an HTML report element.
     */
    explicit HtmlElement(const QString &html = QString()) : Element() {
        m_Html = html;
    }

    /**
     * Sets the full HTML text in this element.
     */
    void setHtml(const QString &html) {
        m_Html = html;
    }

    /**
     * \return the full HTML text in this element.
     */
    QString html() const {
        return m_Html;
    }

    /**
     * @internal
     * @reimp
     */
    void build(QTextCursor *cursor) const override {
        cursor->insertHtml(m_Html);
    }

private:
    QString m_Html;

};

}

#endif // REPORTHTMLELEMENT_H
