#include "listreport.h"

#include "xcsvmodel.h"

#include <QCoreApplication>
#include <QAbstractTableModel>
#include <QTextCursor>
#include <QTextTable>
#include <QFileDialog>

#include <QPrinter>
#include <QPrintPreviewDialog>

/******************************************************************/

ListReport::ListReport(QObject *parent)
    : QObject(parent)
    , m_Model(Q_NULLPTR)
{
    b_WithHeaders = true;
}

/******************************************************************/

void ListReport::setModel(QAbstractTableModel *model)
{
    m_Model = model;
}

/******************************************************************/

void ListReport::setWithHeaders(bool withHeaders)
{
    b_WithHeaders = withHeaders;
}

/******************************************************************/

void ListReport::toCsv(const QString& fileName) const
{
    if (fileName.isEmpty() || m_Model == Q_NULLPTR) return;

    XCsvModel csvModel;
    csvModel.importFromModel(m_Model);
    csvModel.toCSV(fileName, b_WithHeaders);
}

/******************************************************************/

void ListReport::printTo(QTextCursor *cursor) const
{
    cursor->clearSelection();
    cursor->movePosition(QTextCursor::End, QTextCursor::KeepAnchor);

    const int rows = m_Model->rowCount();
    const int cols = m_Model->columnCount();

    QTextTable* table = cursor->insertTable(rows+1, cols);
    // print header
    for (int j=0; j<cols; ++j) {
        auto pos = table->cellAt(0, j).firstCursorPosition();
        pos.insertHtml("<p align=\"center\"><b>" +
                       m_Model->headerData(j, Qt::Horizontal).toString() +
                       "</b>");
    }
    // print data
    for (int i=0; i<rows; ++i) {
        for (int j=0; j<cols; ++j) {
            auto pos = table->cellAt(i+1, j).firstCursorPosition();
            QModelIndex idx = m_Model->index(i, j);
            pos.insertHtml(m_Model->data(idx).toString());
        }
    }
}

/******************************************************************/

QSharedPointer<QTextDocument> ListReport::toDocument() const
{
    QSharedPointer<QTextDocument> document(new QTextDocument());
    QTextCursor cursor(document.data());
    printTo(&cursor);
    return document;
}

/******************************************************************/

void ListReport::toPreview() const
{
    QPrinter printer(QPrinter::HighResolution);
    printer.setFullPage( true );
    QPrintPreviewDialog preview(&printer);
    preview.setWindowFlags ( Qt::Window );
    connect(&preview, &QPrintPreviewDialog::paintRequested,
            this, [this](QPrinter *printer){
        auto document = toDocument();
        document->print(printer);
    });
    preview.exec();
}

/******************************************************************/

void ListReport::toPdf(const QString &fileName) const
{
    QPrinter printer(QPrinter::HighResolution);
    printer.setOutputFormat(QPrinter::PdfFormat);
    printer.setOutputFileName(fileName);
    auto document = toDocument();
    document->print(&printer);
}

/******************************************************************/

void ListReport::exportToCsv(QAbstractTableModel *model)
{
    QString filters("CSV files (*.csv);;All files (*.*)");
    QString defaultFilter("CSV files (*.csv)");
    QString fileName = QFileDialog::getSaveFileName(Q_NULLPTR, tr("Export to CSV-file"),
                                                    QCoreApplication::applicationDirPath(),
                                                    filters, &defaultFilter);
    if (fileName.isEmpty()) return;

    ListReport report;
    report.setModel(model);
    report.toCsv(fileName);
}

/******************************************************************/

void ListReport::printTable(QTextCursor *cursor, QAbstractTableModel *model)
{
    ListReport report;
    report.setModel(model);
    report.printTo(cursor);
}

/******************************************************************/
