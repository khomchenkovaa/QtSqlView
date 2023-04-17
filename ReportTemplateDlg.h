#ifndef REPORTTEMPLATEDLG_H
#define REPORTTEMPLATEDLG_H

#include <QVariant>

#include <QDialog>

QT_BEGIN_NAMESPACE
class QLineEdit;
QT_END_NAMESPACE

/******************************************************************/

struct ReportTemplate {
    QString title;
    QString header;
    QString footer;

    void load(QObject *obj) {
        title  = obj->property("TR_TITLE").toString();
        header = obj->property("TR_HEADER").toString();
        footer = obj->property("TR_FOOTER").toString();
    }

    void save(QObject *obj) {
        obj->setProperty("TR_TITLE", title);
        obj->setProperty("TR_HEADER", header);
        obj->setProperty("TR_FOOTER", footer);
    }
};

/******************************************************************/

class XTextEdit;

/******************************************************************/

class ReportTemplateDlg : public QDialog
{
    Q_OBJECT

public:
    explicit ReportTemplateDlg(QWidget *parent = nullptr);

    void setReportTemplate(const ReportTemplate& tpl);
    ReportTemplate reportTemplate() const;

private:
    void setupUI();

private:
    QLineEdit *editTitle;
    XTextEdit *textHeader;
    XTextEdit *textFooter;
};

/******************************************************************/

#endif // REPORTTEMPLATEDLG_H
