#ifndef HIGHLIGHTINGRULE_H
#define HIGHLIGHTINGRULE_H

#include <QRegExp>
#include <QTextCharFormat>

struct HighlightingRule
{
    QRegExp 	    pattern;
    QTextCharFormat format;

    inline HighlightingRule()
    { }

    inline HighlightingRule(QString p, QTextCharFormat f)
        : pattern(p, Qt::CaseInsensitive), format(f)
    { }

    inline HighlightingRule(QRegExp p, QTextCharFormat f)
        : pattern(p), format(f)
    { }
};

#endif // HIGHLIGHTINGRULE_H
