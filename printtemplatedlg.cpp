#include "printtemplatedlg.h"

#include "xtextedit.h"

#include <QLineEdit>
#include <QFormLayout>
#include <QHBoxLayout>
#include <QDialogButtonBox>

/******************************************************************/

PrintTemplateDlg::PrintTemplateDlg(QWidget *parent) :
    QDialog(parent)
{
    setupUI();
}

/******************************************************************/

QString PrintTemplateDlg::title() const
{
    return editTitle->text();
}

/******************************************************************/

QString PrintTemplateDlg::header() const
{
    return textHeader->text();
}

/******************************************************************/

QString PrintTemplateDlg::footer() const
{
    return textFooter->text();
}

/******************************************************************/

void PrintTemplateDlg::setupUI()
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
