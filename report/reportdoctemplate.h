#ifndef REPORTDOCTEMPLATE_H
#define REPORTDOCTEMPLATE_H

#include "reportdoc.h"
#include "xtexttemplate.h"

namespace Report {

class ReportDocTemplate : public ReportDoc
{
    Q_OBJECT

public:
    explicit ReportDocTemplate(QObject *parent = nullptr)
        : ReportDoc(parent)
        , m_Tpl(toHtml())
    {}

    void loadSource(const QString& fileName) {
        QFile file(fileName);
        if (file.open(QIODevice::ReadOnly | QIODevice::Text)) {
            m_Tpl.setSource(file.readAll());
            file.close();
        }
    }

    void setSource(const QString& source) {
        m_Tpl.setSource(source);
    }

    void setPatternSyntax(XTextTemplate::PatternSyntax syntax) {
        m_Tpl.setPatternSyntax(syntax);
    }

    void renderHtml(const QVariantMap& values) {
        m_Tpl.render(values);
        setHtml(m_Tpl.source());
    }

    void renderHtml(QObject *item) {
        m_Tpl.render(item);
        setHtml(m_Tpl.source());
    }

    void renderPlainText(const QVariantMap& values) {
        m_Tpl.render(values);
        setPlainText(m_Tpl.source());
    }

    void renderPlainText(QObject *item) {
        m_Tpl.render(item);
        setPlainText(m_Tpl.source());
    }

private:
    XTextTemplate m_Tpl;
};

} // namespace Report

#endif // REPORTDOCTEMPLATE_H
