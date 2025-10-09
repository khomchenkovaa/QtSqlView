#include "xmlhighlighter.h"

// largely based on the highlight example in qt's docs

/******************************************************************/

XmlHighlighter::XmlHighlighter(QTextDocument *parent)
    : QSyntaxHighlighter(parent)
{
    setupTagFormat();
    setupAttributeFormat();
    setupAttributeContentFormat();
    setupComments();
}

/******************************************************************/

void XmlHighlighter::highlightBlock(const QString &text)
{
     for (const HighlightingRule &rule : qAsConst(highlightingRules)) {
         QRegExp expression(rule.pattern);
         int index = text.indexOf(expression);
         while (index >= 0) {
             int length = expression.matchedLength();
             setFormat(index, length, rule.format);
             index = text.indexOf(expression, index + length);
         }
     }

     setCurrentBlockState(0);

     int startIndex = 0;
     if (previousBlockState() != 1)
         startIndex = text.indexOf(commentStartExpression);

     while (startIndex >= 0) {
         int endIndex = text.indexOf(commentEndExpression, startIndex);
         int commentLength;
         if (endIndex == -1) {
             setCurrentBlockState(1);
             commentLength = text.length() - startIndex;
         } else {
             commentLength = endIndex - startIndex + commentEndExpression.matchedLength();
         }
         setFormat(startIndex, commentLength, commentFormat);
         startIndex = text.indexOf(commentStartExpression, startIndex + commentLength);
     }
}

/******************************************************************/

void XmlHighlighter::setupComments()
{
    commentFormat.setForeground(Qt::lightGray);
    commentFormat.setFontItalic(true);

    commentStartExpression = QRegExp("<!--");
    commentEndExpression = QRegExp("-->");
}

/******************************************************************/

void XmlHighlighter::setupTagFormat()
{
    QTextCharFormat format;
    format.setForeground(Qt::darkMagenta);
    format.setFontWeight(QFont::Bold);

    QRegExp rule("(<[a-zA-Z:]+\\b|<\\?[a-zA-Z:]+\\b|\\?>|>|/>|</[a-zA-Z:]+>)");

    highlightingRules << HighlightingRule(rule, format);
}

/******************************************************************/

void XmlHighlighter::setupAttributeFormat()
{
    QTextCharFormat format;
    format.setForeground(Qt::darkRed);

    QRegExp rule("[a-zA-Z:]+=");

    highlightingRules << HighlightingRule(rule, format);
}

/******************************************************************/

void XmlHighlighter::setupAttributeContentFormat()
{
    QTextCharFormat format;
    format.setForeground(Qt::darkBlue);

    QRegExp rule("(\"[^\"]*\"|'[^']*')");

    highlightingRules << HighlightingRule(rule, format);
}

/******************************************************************/
