#ifndef REPORTTEXTELEMENT_H
#define REPORTTEXTELEMENT_H

#include "reportelement.h"

#include <QtCore/QString>

namespace Report {

/**
 * The Report::TextElement class represents text in the report.
 * This is one way to insert rich text into the report (the other way is with HtmlElement).
 */
class TextElement : public Element
{
public:
    enum TextType {
        p, h1, h2, h3, h4, h5, h6
    };

    /**
     * Creates an text report element.
     */
    explicit TextElement(const QString &text = QString(), TextType type = TextType::p) : Element() {
        m_Text = text;
        m_Type = type;
    }

    /**
     * Sets the full text in this element.
     */
    void setText(const QString &text, TextType type = TextType::p) {
        m_Text = text;
        m_Type = type;
    }

    /**
     * \return the full text in this element.
     */
    QString text() const {
        return m_Text;
    }

    /**
     * @internal
     * @reimp
     */
    void build(QTextCursor *cursor) const override {
        switch (m_Type) {
        case TextType::p:
            cursor->insertHtml(QString("<p>%1</p>").arg(m_Text));
            break;
        case TextType::h1:
            cursor->insertHtml(QString("<h1 align=\"center\">%1</h1>").arg(m_Text));
            break;
        case TextType::h2:
            cursor->insertHtml(QString("<h2>%1</h2>").arg(m_Text));
            break;
        case TextType::h3:
            cursor->insertHtml(QString("<h3>%1</h3>").arg(m_Text));
            break;
        case TextType::h4:
            cursor->insertHtml(QString("<h4>%1</h4>").arg(m_Text));
            break;
        case TextType::h5:
            cursor->insertHtml(QString("<h5>%1</h5>").arg(m_Text));
            break;
        case TextType::h6:
            cursor->insertHtml(QString("<h6>%1</h6>").arg(m_Text));
            break;
        }
    }

private:
    QString  m_Text;
    TextType m_Type;

};

} // namespace Report

#endif // REPORTTEXTELEMENT_H
