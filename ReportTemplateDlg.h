#ifndef REPORTTEMPLATEDLG_H
#define REPORTTEMPLATEDLG_H

#include <QDialog>

QT_BEGIN_NAMESPACE
class QLineEdit;
QT_END_NAMESPACE

/******************************************************************/

struct ReportTemplate {
    QString title;
    QString header;
    QString footer;
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
