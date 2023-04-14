#ifndef PRINTTEMPLATEDLG_H
#define PRINTTEMPLATEDLG_H

#include <QDialog>

namespace Ui {
class PrintTemplateDlg;
}

class PrintTemplateDlg : public QDialog
{
    Q_OBJECT

public:
    explicit PrintTemplateDlg(QWidget *parent = nullptr);
    ~PrintTemplateDlg();

    QString title() const;
    QString header() const;
    QString footer() const;

private:
    Ui::PrintTemplateDlg *ui;
};

#endif // PRINTTEMPLATEDLG_H
