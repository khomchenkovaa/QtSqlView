#ifndef XREPORTS_HTMLELEMENT_H
#define XREPORTS_HTMLELEMENT_H

#include "XReportsElement.h"
#include "XReportsReportBuilder.h"

#include <QString>
#include <QTextCursor>

namespace XReports {

/**
 * The XReports::HtmlElement class represents any part of the report that can be expressed in HTML.
 * This is one way to insert rich text into the report (the other way is with TextElement).
 * This can be used to create bullet-point lists and numbered lists.
 */
class HtmlElement : public Element
{
    struct HtmlElementPrivate
    {
        QString html;
        QString id;
    };

public:
    //! Creates an HTML report element.
    explicit HtmlElement(const QString &html = QString()) : Element() {
        d.html = html;
    }

    //! Destroys this element.
    ~HtmlElement() override {}

    //! Copies an HTML element. The HTML and settings are copied over.
    HtmlElement(const HtmlElement &other) : Element(), d(other.d) {}

    //! Copies the HTML and settings from another HTML element.
    HtmlElement &operator=(const HtmlElement &other) {
        if (&other == this) return *this;
        Element::operator=(other);
        d = other.d;
        return *this;
    }

    //! Sets the full HTML text in this element.
    void setHtml(const QString &html) {
        d.html = html;
    }

    //! \return the full HTML text in this element.
    QString html() const {
        return d.html;
    }

    //! Adds more text to the HTML in this element.
    HtmlElement &operator<<(const QString &str) {
        d.html.append(str);
        return *this;
    }

    //! Set the ID associated with this text element.
    void setId(const QString &id) {
        d.id = id;
    }

    //! @return the ID associated with this text element.
    QString id() const {
        return d.id;
    }

    /**
     * @internal
     * @reimp
     */
    void build(ReportBuilder &builder) const override {
        QTextCursor &cursor = builder.cursor();
        const int charPosition = cursor.position();
        cursor.insertHtml(d.html);
        if (!d.id.isEmpty())
            builder.currentDocumentData().setTextValueMarker(charPosition, d.id, cursor.position(), false);
    }

    /**
     * @internal
     * @reimp
     */
    Element *clone() const override {
        return new HtmlElement(*this);
    }

private:
    HtmlElementPrivate d;
};

} // namespace XReports

#endif /* XREPORTS_HTMLELEMENT_H */
