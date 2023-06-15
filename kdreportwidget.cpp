#include "kdreportwidget.h"
#include "ui_kdreportwidget.h"

#include "xmlhighlighter.h"

#include <QCoreApplication>

#include <QMenu>
#include <QMessageBox>
#include <QFileDialog>
#include <QTextStream>

#include <QSqlQueryModel>

#ifdef KD_REPORTS
#include "kdxmlreport.h"
#endif

/******************************************************************/

KdReportWidget::KdReportWidget(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::KdReportWidget),
    m_Model(Q_NULLPTR)
{
    ui->setupUi(this);
    setupUI();
    setupActions();
}

/******************************************************************/

KdReportWidget::~KdReportWidget()
{
    delete ui;
}

/******************************************************************/

void KdReportWidget::setUserQueryModel(QSqlQueryModel *model)
{
    m_Model = model;
    ui->queryKdTable->setModel(m_Model);
}

/******************************************************************/

void KdReportWidget::updateView()
{
    ui->queryKdTable->resizeColumnsToContents();
    ui->queryKdTable->resizeRowsToContents();
}

/******************************************************************/

void KdReportWidget::print()
{
    auto report = createReport();
    if (!report.isNull()) {
        report->toPrinter();
    }
}

/******************************************************************/

void KdReportWidget::preview()
{
    auto report = createReport();
    if (!report.isNull()) {
        report->toPreviewDialog();
    }
}

/******************************************************************/

void KdReportWidget::exportToPdf()
{
    QFileDialog fileDialog(this, tr("Export PDF"));
    fileDialog.setAcceptMode(QFileDialog::AcceptSave);
    fileDialog.setMimeTypeFilters(QStringList("application/pdf"));
    fileDialog.setDefaultSuffix("pdf");
    if (fileDialog.exec() != QDialog::Accepted)
        return;
    QString fileName = fileDialog.selectedFiles().first();

    auto report = createReport();
    if (!report.isNull()) {
        report->toPdfFile(fileName);
    }
}

/******************************************************************/

void KdReportWidget::exportToHtml()
{
    QFileDialog fileDialog(this, tr("Export HTML"));
    fileDialog.setAcceptMode(QFileDialog::AcceptSave);
    fileDialog.setMimeTypeFilters(QStringList("text/html"));
    fileDialog.setDefaultSuffix("html");
    if (fileDialog.exec() != QDialog::Accepted)
        return;
    QString fileName = fileDialog.selectedFiles().first();

    auto report = createReport();
    if (!report.isNull()) {
        report->toHtmlFile(fileName);
    }
}

/******************************************************************/

void KdReportWidget::exportToCsv()
{
    QFileDialog fileDialog(this, tr("Export CSV"));
    fileDialog.setAcceptMode(QFileDialog::AcceptSave);
    fileDialog.setMimeTypeFilters(QStringList("text/csv"));
    fileDialog.setDefaultSuffix("csv");
    if (fileDialog.exec() != QDialog::Accepted)
        return;
    QString fileName = fileDialog.selectedFiles().first();

    auto report = createReport();
    if (!report.isNull()) {
        report->toCsvFile(fileName);
    }
}

/******************************************************************/

void KdReportWidget::clearXml()
{
    ui->editXml->clear();
}

/******************************************************************/

void KdReportWidget::loadXml()
{
    QString filename = QFileDialog::getOpenFileName(this, tr("Choose a XML file"),
                                                    QString(),
                                                    "XML files (*.xml);;All Files (*.*)");

    if (filename.isEmpty()) return;

    QFile file(filename);
    if (!file.open(QIODevice::ReadOnly | QIODevice::Text)) {
        QMessageBox::critical(this, QCoreApplication::applicationName(),
                              tr("Could not load XML file"));
        return;
    }

    ui->editXml->setPlainText( file.readAll() );
}

/******************************************************************/

void KdReportWidget::saveXml()
{
    QString filename = QFileDialog::getSaveFileName(this, tr("Choose a XML file"),
                                                    QString(),
                                                    "XML files (*.xml);;All Files (*.*)");

    if (filename.isEmpty()) return;

    QFile file(filename);
    if (!file.open(QIODevice::WriteOnly | QIODevice::Text)) {
        QMessageBox::critical(this, QCoreApplication::applicationName(),
                              tr("Could not save XML file"));
        return;
    }

    QTextStream out(&file);
    out << ui->editXml->toPlainText();
}

/******************************************************************/

void KdReportWidget::setupUI()
{
    QFont font("Courier", 10);
    font.setFixedPitch(true);

    ui->editXml->setFont(font);

    new XmlHighlighter(ui->editXml->document());

    const QString defaultXml =
            "<?xml version=\"1.0\" encoding=\"UTF-8\"?>\n"
            "<report xmlns=\"https://www.kdab.com/kdreports\">\n"
            "  <table model=\"TableModel\" verticalHeaderVisible=\"false\"/>\n"
            "</report>";
    ui->editXml->setPlainText(defaultXml);

    connect(ui->headersButton, &QToolButton::clicked,
            this, &KdReportWidget::tableHeaders);
    connect(ui->clearXmlButton, &QToolButton::clicked,
            this, &KdReportWidget::clearXml);
    connect(ui->loadXmlButton, &QToolButton::clicked,
            this, &KdReportWidget::loadXml);
    connect(ui->saveXmlButton, &QToolButton::clicked,
            this, &KdReportWidget::saveXml);
}

/******************************************************************/

void KdReportWidget::setupActions()
{
    auto actPreview = new QAction(QIcon::fromTheme("zoom-original"), tr("Preview"), this);
    ui->previewButton->setDefaultAction(actPreview);

    auto actPrint = new QAction(QIcon::fromTheme("printer"), tr("Print"), this);
    ui->printButton->setDefaultAction(actPrint);

    auto actPdf = new QAction(QIcon::fromTheme("x-office-document"), tr("Export to PDF"), this);
    auto actHtml = new QAction(QIcon::fromTheme("text-html"), tr("Export to HTML"), this);
    auto actCsv = new QAction(QIcon::fromTheme("x-office-spreadsheet"), tr("Export to CSV"), this);

    auto menu = new QMenu(tr("Export"),this);
    menu->setIcon(QIcon::fromTheme("document-open"));
    menu->addAction(actPdf);
    menu->addAction(actHtml);
    menu->addAction(actCsv);

    ui->exportButton->setDefaultAction(menu->menuAction());

    connect(actPreview, &QAction::triggered,
            this, &KdReportWidget::preview);
    connect(actPrint, &QAction::triggered,
            this, &KdReportWidget::print);
    connect(actPdf, &QAction::triggered,
            this, &KdReportWidget::exportToPdf);
    connect(actHtml, &QAction::triggered,
            this, &KdReportWidget::exportToHtml);
    connect(actCsv, &QAction::triggered,
            this, &KdReportWidget::exportToCsv);
}

/******************************************************************/

QSharedPointer<ListReport> KdReportWidget::createReport()
{
#ifdef KD_REPORTS
    auto report = QSharedPointer<KdXmlReport>::create();
    QString errorMsg;
    if (!report->setXml(ui->editXml->toPlainText(), &errorMsg)) {
        QMessageBox::critical(this, QCoreApplication::applicationName(), errorMsg);
        return Q_NULLPTR;
    }

    report->setModel(m_Model);
    return report;
#else
    return Q_NULLPTR;
#endif
}

/******************************************************************/
