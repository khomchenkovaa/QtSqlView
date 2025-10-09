#ifndef XMLSYNTAXHIGHLIGHTER_H
#define XMLSYNTAXHIGHLIGHTER_H

#include "highlightingrule.h"

#include <QSyntaxHighlighter>

class XmlHighlighter : public QSyntaxHighlighter
{
    Q_OBJECT

public:
    explicit XmlHighlighter(QTextDocument *parent = nullptr);

protected:
    virtual void highlightBlock(const QString &text);

private:
    void setupComments();
    void setupTagFormat();
    void setupAttributeFormat();
    void setupAttributeContentFormat();

private:
    QList<HighlightingRule> highlightingRules;

    QRegExp commentStartExpression;
    QRegExp commentEndExpression;

    QTextCharFormat commentFormat;
};

#endif
