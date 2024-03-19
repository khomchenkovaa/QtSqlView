#ifndef XREPORTS_TEXTELEMENT_H
#define XREPORTS_TEXTELEMENT_H

#include "XReportsElement.h"
#include "XReportsUtils.h"

#include <QTextCharFormat>
#include <QString>

namespace XReports {

/**
 * The XReports::TextElement class represents text in the report.
 * This is one way to insert rich text into the report (the other way is with HtmlElement).
 */
class TextElement : public Element
{
    friend class ReportBuilder;

    struct TextElementPrivate
    {
        QString string;
        QString id;

        QString fontFamily;
        bool    bold           = false;
        bool    boldIsSet      = false;
        bool    italic         = false;
        bool    italicIsSet    = false;
        bool    underline      = false;
        bool    underlineIsSet = false;
        bool    strikeout      = false;
        bool    strikeoutIsSet = false;
        bool    fontSet = false;
        qreal   pointSize = 0;
        QColor  foreground;
        QFont   font;
    };

public:

    //! Creates a text element.
    explicit TextElement(const QString &string = QString()) : Element()
    {
        d.string = string;
    }

    //! Destroys this text element.
    ~TextElement() override {}

    //! Copies a text element. The text and settings are copied over.
    TextElement(const TextElement &other) : Element(other), d(other.d)
    {
    }

    //! Copies the text and settings from another text element.
    TextElement &operator=(const TextElement &other) {
        if (&other == this) return *this;
        Element::operator=(other);
        d = other.d;
        return *this;
    }

    //! Sets the full text in this element.
    void setText(const QString &text) {
        d.string = text;
    }

    //! Adds more text to the text in this element.
    TextElement &operator<<(const QString &str) {
        d.string.append(str);
        return *this;
    }

    //! Set font attribute: family.
    void setFontFamily(const QString &family) {
        d.fontFamily = family;
    }

    //! Set font attribute: bold.
    void setBold(bool bold) {
        d.bold = bold;
        d.boldIsSet = true;
    }

    //! Set font attribute: italic.
    void setItalic(bool italic) {
        d.italic = italic;
        d.italicIsSet = true;
    }

    //! Set font attribute: underline.
    void setUnderline(bool underline) {
        d.underline = underline;
        d.underlineIsSet = true;
    }

    //! Set font attribute: strike out.
    void setStrikeOut(bool strikeout) {
        d.strikeout = strikeout;
        d.strikeoutIsSet = true;
    }

    //! Set font attribute: size in points. Can be integer or decimal.
    void setPointSize(qreal size) {
        d.pointSize = size;
    }

    /**
     * Set multiple font attributes with a single call.
     *
     * Note that, when Qt is 5.3 or more, the font attributes that are not
     * explicitly specified in this font, will be resolved against the report's default font,
     * rather than being resolved against the application's font.
     */
    void setFont(const QFont &font) {
        d.font = font;
        d.fontSet = true;
    }

    //! \return all font attribute
    QFont font() const {
        return d.font;
    }

    //! Set the text color.
    void setTextColor(const QColor &color) {
        d.foreground = color;
    }

    //! \return the text color.
    QColor textColor() const {
        return d.foreground;
    }

    //! Set the ID associated with this text element.
    void setId(const QString &id) {
        d.id = id;
    }

    //! \return the ID associated with this text element.
    QString id() const {
        return d.id;
    }

    //! \return the text associated with this text element.
    QString text() const {
        return d.string;
    }

    /**
     * @internal
     * @reimp
     */
    void build(ReportBuilder &builder) const override;

    /**
     * @internal
     * @reimp
     */
    Element *clone() const override {
        return new TextElement(*this);
    }

private:
    TextElementPrivate d;
};

} // namespace XReports

#endif /* XREPORTS_TEXTELEMENT_H */
