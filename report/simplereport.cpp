#include "simplereport.h"

#include <QTextCursor>

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

void SimpleReport::toTextCursor(QTextCursor *cursor) const
{
    if (!m_Title.isEmpty()) {
        cursor->insertHtml("<h1 align=\"center\">" + m_Title + "</h1>");
        cursor->insertBlock();
    }
    if (!m_Header.isEmpty()) {
        cursor->movePosition(QTextCursor::End, QTextCursor::MoveAnchor);
        cursor->insertHtml(m_Header);
        cursor->insertBlock();
    }
    ListReport::toTextCursor(cursor);
    if (!m_Footer.isEmpty()) {
        cursor->movePosition(QTextCursor::End, QTextCursor::MoveAnchor);
        cursor->insertBlock();
        cursor->insertHtml(m_Footer);
        cursor->insertBlock();
    }
}

/******************************************************************/
