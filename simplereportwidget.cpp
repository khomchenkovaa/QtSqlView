#include "simplereportwidget.h"
#include "ui_simplereportwidget.h"

#include "report.h"

#include <QMenu>

#include <QMimeDatabase>
#include <QMimeType>

#include <QSqlQueryModel>

/******************************************************************/

SimpleReportWidget::SimpleReportWidget(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::SimpleReportWidget),
    m_Model(Q_NULLPTR)
{
    ui->setupUi(this);
    setupUI();
    setupActions();
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

void SimpleReportWidget::print()
{
    auto report = createReport();
    report->toPrinter();
}

/******************************************************************/

void SimpleReportWidget::preview()
{
    auto report = createReport();
    report->toPreviewDialog();
}

/******************************************************************/

void SimpleReportWidget::exportToPdf()
{
    QString fileName = Report::exportToPdfDlg(this, tr("Export PDF"));
    if (fileName.isEmpty()) return;

    auto report = createReport();
    report->toPdfFile(fileName);
}

/******************************************************************/

void SimpleReportWidget::exportToHtml()
{
    QString fileName = Report::exportToHtmlDlg(this, tr("Export HTML"));
    if (fileName.isEmpty()) return;

    auto report = createReport();
    report->toHtmlFile(fileName);
}

/******************************************************************/

void SimpleReportWidget::exportToCsv()
{
    QString fileName = Report::exportToCsvDlg(this, tr("Export CSV"));
    if (fileName.isEmpty()) return;

    auto report = createReport();
    report->toCsvFile(fileName);
}

/******************************************************************/

void SimpleReportWidget::clearFields()
{
    ui->editTitle->clear();
    ui->textHeader->clear();
    ui->textFooter->clear();
}

/******************************************************************/

void SimpleReportWidget::setupUI()
{
    connect(ui->setTblHeadersButton, &QToolButton::clicked,
            this, &SimpleReportWidget::tableHeaders);
    connect(ui->clearSrPropertiesButton, &QToolButton::clicked,
            this, &SimpleReportWidget::clearFields);
}

/******************************************************************/

void SimpleReportWidget::setupActions()
{
    auto actPreview = new QAction(QIcon::fromTheme("zoom-original"), tr("Preview"), this);
    ui->previewButton->setDefaultAction(actPreview);

    auto actPrint = new QAction(QIcon::fromTheme("printer"), tr("Print"), this);
    ui->printButton->setDefaultAction(actPrint);

    auto actPdf  = new QAction(iconForMimeType("application/pdf"), tr("Export to PDF"), this);
    auto actHtml = new QAction(iconForMimeType("text/html"), tr("Export to HTML"), this);
    auto actCsv  = new QAction(iconForMimeType("text/csv"), tr("Export to CSV"), this);

    auto menu = new QMenu(tr("Export"),this);
    menu->setIcon(QIcon::fromTheme("document-open"));
    menu->addAction(actPdf);
    menu->addAction(actHtml);
    menu->addAction(actCsv);

    ui->exportButton->setDefaultAction(menu->menuAction());

    connect(actPreview, &QAction::triggered,
            this, &SimpleReportWidget::preview);
    connect(actPrint, &QAction::triggered,
            this, &SimpleReportWidget::print);
    connect(actPdf, &QAction::triggered,
            this, &SimpleReportWidget::exportToPdf);
    connect(actHtml, &QAction::triggered,
            this, &SimpleReportWidget::exportToHtml);
    connect(actCsv, &QAction::triggered,
            this, &SimpleReportWidget::exportToCsv);

}

/******************************************************************/

QSharedPointer<ListReport> SimpleReportWidget::createReport() const
{
    QString title  = ui->editTitle->text();
    QString header = ui->textHeader->text();
    QString footer = ui-> textFooter->text();

    auto report = QSharedPointer<SimpleReport>::create();
    report->setTitle(title);
    report->setHeader(header);
    report->setModel(m_Model);
    report->setFooter(footer);
    return report;
}

/******************************************************************/

QIcon SimpleReportWidget::iconForMimeType(const QString &mime, const QIcon &fallback)
{
    QMimeDatabase db;
    QMimeType t = db.mimeTypeForName(mime);
    if (QIcon::hasThemeIcon(t.iconName())) {
        return QIcon::fromTheme(t.iconName());
    }
    if (QIcon::hasThemeIcon(t.genericIconName())) {
        return QIcon::fromTheme(t.genericIconName());
    }
    return fallback;
}

/******************************************************************/
