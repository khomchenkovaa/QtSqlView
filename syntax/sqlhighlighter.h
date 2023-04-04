#ifndef SQLHIGHLIGHTER_H
#define SQLHIGHLIGHTER_H

#include <QList>
#include <QRegExp>

#include <QSyntaxHighlighter>
#include <QTextCharFormat>

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
    struct Rule
    {
        QRegExp 	    pattern;
        QTextCharFormat format;

        inline Rule()
        { }

        inline Rule(QString p, QTextCharFormat f)
            : pattern(p, Qt::CaseInsensitive), format(f)
        { }
    };

    QList<Rule> 	rules;

    QRegExp 		commentStartExpression;
    QRegExp 		commentEndExpression;

    QTextCharFormat commentFormat;
};

#endif // SQLHIGHLIGHTER_H
