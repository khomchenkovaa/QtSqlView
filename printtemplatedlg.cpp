#include "printtemplatedlg.h"
#include "ui_printtemplatedlg.h"

/******************************************************************/

PrintTemplateDlg::PrintTemplateDlg(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::PrintTemplateDlg)
{
    ui->setupUi(this);
}

/******************************************************************/

PrintTemplateDlg::~PrintTemplateDlg()
{
    delete ui;
}

/******************************************************************/

QString PrintTemplateDlg::title() const
{
    return ui->editTitle->text();
}

/******************************************************************/

QString PrintTemplateDlg::header() const
{
    return ui->textHeader->toHtml();
}

/******************************************************************/

QString PrintTemplateDlg::footer() const
{
    return ui->textFooter->toHtml();
}

/******************************************************************/
