#ifndef SQLSYNTAXHIGHLIGHTER_H
#define SQLSYNTAXHIGHLIGHTER_H

#include "highlightingrule.h"

#include <QSyntaxHighlighter>

class SQLHighlighter : public QSyntaxHighlighter
{
    Q_OBJECT

public:
    SQLHighlighter(class QTextDocument *parent = NULL);

protected:
    virtual void highlightBlock(const QString &text);

private:
    void setupComments();
    void setupQuotedStrings();
    void setupPrimaryKeywords();
    void setupTypeKeywords();
    void setupExoticKeywords();

private:
    QList<HighlightingRule> highlightingRules;

    QRegExp commentStartExpression;
    QRegExp commentEndExpression;

    QTextCharFormat commentFormat;
};

#endif // SQLSYNTAXHIGHLIGHTER_H
