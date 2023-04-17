#include "ReportTemplateDlg.h"

#include "xtextedit.h"

#include <QLineEdit>
#include <QFormLayout>
#include <QHBoxLayout>
#include <QDialogButtonBox>

/******************************************************************/

ReportTemplateDlg::ReportTemplateDlg(QWidget *parent) :
    QDialog(parent)
{
    setupUI();
}

/******************************************************************/

void ReportTemplateDlg::setReportTemplate(const ReportTemplate &tpl)
{
    editTitle->setText(tpl.title);
    textHeader->setText(tpl.header);
    textFooter->setText(tpl.footer);
}

/******************************************************************/

ReportTemplate ReportTemplateDlg::reportTemplate() const
{
    ReportTemplate tpl;
    tpl.title = editTitle->text();
    tpl.header = textHeader->text();
    tpl.footer = textFooter->text();
    return tpl;
}

/******************************************************************/

void ReportTemplateDlg::setupUI()
{
    editTitle = new QLineEdit(this);
    textHeader = new XTextEdit(this);
    textFooter = new XTextEdit(this);

    auto form = new QFormLayout();
    form->addRow(tr("Title"), editTitle);
    form->addRow(tr("Header"), textHeader);
    form->addRow(tr("Footer"), textFooter);

    auto buttonBox = new QDialogButtonBox(this);
    buttonBox->setOrientation(Qt::Vertical);
    buttonBox->setStandardButtons(QDialogButtonBox::Cancel|QDialogButtonBox::Ok);

    auto buttonLayout = new QVBoxLayout();
    buttonLayout->addWidget(buttonBox);
    buttonLayout->addItem(new QSpacerItem(20, 40, QSizePolicy::Minimum, QSizePolicy::Expanding));

    auto main = new QHBoxLayout(this);
    main->addLayout(form);
    main->addLayout(buttonLayout);

    QObject::connect(buttonBox, &QDialogButtonBox::accepted,
                     this, &QDialog::accept);
    QObject::connect(buttonBox, &QDialogButtonBox::rejected,
                     this, &QDialog::reject);
}

/******************************************************************/
