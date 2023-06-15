#include "kdreportwidget.h"
#include "ui_kdreportwidget.h"

#include "xmlhighlighter.h"

#include <QCoreApplication>

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

void KdReportWidget::printReport()
{
#ifdef KD_REPORTS
    KdXmlReport report;
    QString errorMsg;
    if (!report.setXml(ui->editXml->toPlainText(), &errorMsg)) {
        QMessageBox::critical(this, QCoreApplication::applicationName(), errorMsg);
        return;
    }

    report.setModel(m_Model);
    report.toPreviewDialog();
#endif
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

#ifndef KD_REPORTS
    ui->printKdReportButton->setHidden(true);
#endif

    connect(ui->setKdHeadersButton, &QToolButton::clicked,
            this, &KdReportWidget::tableHeaders);
    connect(ui->printKdReportButton, &QToolButton::clicked,
            this, &KdReportWidget::printReport);
    connect(ui->clearXmlButton, &QToolButton::clicked,
            this, &KdReportWidget::clearXml);
    connect(ui->loadXmlButton, &QToolButton::clicked,
            this, &KdReportWidget::loadXml);
    connect(ui->saveXmlButton, &QToolButton::clicked,
            this, &KdReportWidget::saveXml);
}

/******************************************************************/
