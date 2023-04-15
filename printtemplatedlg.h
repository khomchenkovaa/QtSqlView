#ifndef PRINTTEMPLATEDLG_H
#define PRINTTEMPLATEDLG_H

#include <QDialog>

QT_BEGIN_NAMESPACE
class QLineEdit;
QT_END_NAMESPACE

class XTextEdit;

class PrintTemplateDlg : public QDialog
{
    Q_OBJECT

public:
    explicit PrintTemplateDlg(QWidget *parent = nullptr);

    QString title() const;
    QString header() const;
    QString footer() const;

private:
    void setupUI();

private:
    QLineEdit *editTitle;
    XTextEdit *textHeader;
    XTextEdit *textFooter;
};

#endif // PRINTTEMPLATEDLG_H
