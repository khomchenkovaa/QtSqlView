#ifndef XTEXTTEMPLATE_H
#define XTEXTTEMPLATE_H

#include <QStringList>
#include <QVariantMap>
#include <QMetaProperty>

/**
 * @brief The XTextTemplate class provides a basic text template engine
 *
 * set source text containing variables (django, php, win style)
 */
class XTextTemplate
{
public:
    enum PatternSyntax {
        DjangoStyle, ///< style like "{{ SOME_VAR }}"
        PhpStyle,    ///< style like "<?= SOME_VAR ?>"
        WinStyle     ///< style like "%SOME_VAR%"
    };

private:
    struct XTextTemplatePrivate {
        QString source;
        PatternSyntax syntax = PatternSyntax::DjangoStyle;

        QString searchPattern() const {
            switch (syntax) {
            case PatternSyntax::DjangoStyle: return "\\{\\{\\s*([\\w_]+[\\w_\\d]*)\\s*\\}\\}";
            case PatternSyntax::PhpStyle:    return "<\\?=\\s*([\\w_]+[\\w_\\d]*)\\s*\\?>";
            case PatternSyntax::WinStyle:    return "%([\\w_]+[\\w_\\d]*)%";
            }
            return QString();
        }

        QString replacePattern() const {
            switch (syntax) {
            case PatternSyntax::DjangoStyle: return "\\{\\{\\s*%1\\s*\\}\\}";
            case PatternSyntax::PhpStyle:    return "<\\?=\\s*%1\\s*\\?>";
            case PatternSyntax::WinStyle:    return "%%1%";
            }
            return QString();
        }
    };
public:
    explicit XTextTemplate(const QString& source, PatternSyntax syntax = DjangoStyle) {
        setSource(source);
        setPatternSyntax(syntax);
    }

    void setSource(const QString& source) {
        d.source  = source;
    }

    QString source() const {
        return d.source;
    }

    void setPatternSyntax(PatternSyntax varType) {
        d.syntax = varType;
    }

    /**
     * @brief parses the source for variables according to varType
     * @return the list of variables
     */
    QStringList findVars() const {
        return findVars(d.searchPattern());
    }

    /**
     * @brief parses the source for variables accorfing to QRegExp pattern
     * @param pattern
     * @return the list of variables
     */
    QStringList findVars(const QString& pattern) const {
        QStringList result;
        if (pattern.isEmpty()) return result;
        QRegExp rx(pattern);
        int pos = 0;
        while ((pos = rx.indexIn(d.source, pos)) != -1) {
            if (!result.contains(rx.cap(1))) {
                result.append(rx.cap(1));
            }
            pos += rx.matchedLength();
        }
        return result;
    }

    void render(const QVariantMap &values) {
        QMapIterator<QString, QVariant> it(values);
        while(it.hasNext()) {
            it.next();
            QRegExp rx(d.replacePattern().arg(it.key()));
            d.source.replace(rx,it.value().toString());
        }
    }

    void render(QObject *item) {
        const int count = item->metaObject()->propertyCount();
        for(int i=0; i<count; ++i) {
            const QMetaProperty metaproperty = item->metaObject()->property(i);
            const char* name = metaproperty.name();
            QRegExp rx(d.replacePattern().arg(name));
            d.source.replace(rx,item->property(name).toString());
        }
    }

private:
    XTextTemplatePrivate d;
};

#endif // XTEXTTEMPLATE_H
