#include "simplereport.h"

#include "reporttextelement.h"
#include "reporthtmlelement.h"
#include "reporttableelement.h"

/******************************************************************/

SimpleReport::SimpleReport(QObject *parent)
    : ListReport(parent)
{

}

/******************************************************************/

void SimpleReport::setTitle(const QString &title)
{
    m_Title = title;
}

/******************************************************************/

void SimpleReport::setHeader(const QString &header)
{
    m_Header = header;
}

/******************************************************************/

void SimpleReport::setFooter(const QString &footer)
{
    m_Footer = footer;
}

/******************************************************************/

QSharedPointer<QTextDocument> SimpleReport::toTextDocument() const
{
    auto document = ReportBase::toTextDocument();
    QTextCursor cursor(document.data());
    if (!m_Title.isEmpty()) {
        Report::TextElement title(m_Title, Report::TextElement::h1);
        title.build(&cursor);
    }
    if (!m_Header.isEmpty()) {
        cursor.movePosition(QTextCursor::End, QTextCursor::MoveAnchor);
        cursor.insertBlock();
        Report::HtmlElement header(m_Header);
        header.build(&cursor);
    }
    if (m_Model) {
        cursor.movePosition(QTextCursor::End, QTextCursor::MoveAnchor);
        cursor.insertBlock();
        Report::TableElement table(m_Model, b_WithHeaders);
        table.build(&cursor);
    }
    if (!m_Footer.isEmpty()) {
        cursor.movePosition(QTextCursor::End, QTextCursor::MoveAnchor);
        cursor.insertBlock();
        Report::HtmlElement footer(m_Footer);
        footer.build(&cursor);
    }
    return document;
}

/******************************************************************/
