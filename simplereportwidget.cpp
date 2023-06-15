#include "simplereportwidget.h"
#include "ui_simplereportwidget.h"

#include "xtextedit.h"

#include <QFileDialog>

#include <QSqlQueryModel>

#include "simplereport.h"
#ifdef KD_REPORTS
#include "kdsimplereport.h"
#endif

/******************************************************************/

SimpleReportWidget::SimpleReportWidget(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::SimpleReportWidget),
    m_Model(Q_NULLPTR)
{
    ui->setupUi(this);
    setupUI();
}

/******************************************************************/

SimpleReportWidget::~SimpleReportWidget()
{
    delete ui;
}

/******************************************************************/

void SimpleReportWidget::setUserQueryModel(QSqlQueryModel *model)
{
    m_Model = model;
    ui->querySrTable->setModel(m_Model);
}

/******************************************************************/

void SimpleReportWidget::updateView()
{
    ui->querySrTable->resizeColumnsToContents();
    ui->querySrTable->resizeRowsToContents();
}

/******************************************************************/

void SimpleReportWidget::printReport()
{
    QString title = ui->editSrTitle->text();
    QString header;
    QString footer;
    auto textSrHeader = findChild<XTextEdit*>("textSrHeader");
    if (textSrHeader) {
        header = textSrHeader->text();
    }
    auto textSrFooter = findChild<XTextEdit*>("textSrFooter");
    if (textSrFooter) {
        footer = textSrFooter->text();
    }

#ifdef KD_REPORTS
    KdSimpleReport report;
#else
    SimpleReport report;
#endif
    report.setTitle(title);
    report.setHeader(header);
    report.setModel(m_Model);
    report.setFooter(footer);
    report.toPreviewDialog();
}

/******************************************************************/

void SimpleReportWidget::exportToPdf()
{
    QString title = ui->editSrTitle->text();
    QString header;
    QString footer;
    auto textSrHeader = findChild<XTextEdit*>("textSrHeader");
    if (textSrHeader) {
        header = textSrHeader->text();
    }
    auto textSrFooter = findChild<XTextEdit*>("textSrFooter");
    if (textSrFooter) {
        footer = textSrFooter->text();
    }

    QFileDialog fileDialog(this, tr("Export PDF"));
    fileDialog.setAcceptMode(QFileDialog::AcceptSave);
    fileDialog.setMimeTypeFilters(QStringList("application/pdf"));
    fileDialog.setDefaultSuffix("pdf");
    if (fileDialog.exec() != QDialog::Accepted)
        return;
    QString fileName = fileDialog.selectedFiles().first();

    SimpleReport report;
    report.setTitle(title);
    report.setHeader(header);
    report.setModel(m_Model);
    report.setFooter(footer);
    report.toPdfFile(fileName);
}

/******************************************************************/

void SimpleReportWidget::clearFields()
{
    ui->editSrTitle->clear();
    for (auto widget : findChildren<XTextEdit*>()) {
        widget->clear();
    }
}

/******************************************************************/

void SimpleReportWidget::setupUI()
{
    ui->formSimpleReport->removeRow(2);
    ui->formSimpleReport->removeRow(1);

    auto textSrHeader = new XTextEdit(this);
    textSrHeader->setObjectName("textSrHeader");
    ui->formSimpleReport->addRow(tr("Header"), textSrHeader);

    auto textSrFooter = new XTextEdit(this);
    textSrFooter->setObjectName("textSrFooter");
    ui->formSimpleReport->addRow(tr("Footer"), textSrFooter);

#ifdef KD_REPORTS
    ui->exportToPdfButton->setHidden(true);
#endif

    connect(ui->setTblHeadersButton, &QToolButton::clicked,
            this, &SimpleReportWidget::tableHeaders);
    connect(ui->printReportButton, &QToolButton::clicked,
            this, &SimpleReportWidget::printReport);
    connect(ui->exportToPdfButton, &QToolButton::clicked,
            this, &SimpleReportWidget::exportToPdf);
    connect(ui->clearSrPropertiesButton, &QToolButton::clicked,
            this, &SimpleReportWidget::clearFields);
}

/******************************************************************/
