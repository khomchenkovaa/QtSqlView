#include "listreport.h"

#include "xcsvmodel.h"
#include "reportutils.h"

#include <QCoreApplication>
#include <QTextStream>
#include <QFileDialog>

#include <QPrinter>
#include <QPrintDialog>
#include <QPrintPreviewDialog>

/******************************************************************/

ListReport::ListReport(QObject *parent)
    : QObject(parent)
    , m_Model(Q_NULLPTR)
{
    b_WithHeaders = true;
}

/******************************************************************/

void ListReport::setModel(QAbstractItemModel *model)
{
    m_Model = model;
}

/******************************************************************/

void ListReport::setWithHeaders(bool withHeaders)
{
    b_WithHeaders = withHeaders;
}

/******************************************************************/

bool ListReport::toCsvFile(const QString& fileName) const
{
    if (fileName.isEmpty() || m_Model == Q_NULLPTR) return false;

    XCsvModel csvModel;
    csvModel.importFromModel(m_Model);
    csvModel.toCSV(fileName, b_WithHeaders);
    return true;
}

/******************************************************************/

void ListReport::toTextCursor(QTextCursor *cursor) const
{
    Report::printTable(cursor, m_Model);
}

/******************************************************************/

QSharedPointer<QTextDocument> ListReport::toTextDocument() const
{
    QSharedPointer<QTextDocument> document(new QTextDocument());
    QTextCursor cursor(document.data());
    toTextCursor(&cursor);
    return document;
}

/******************************************************************/

bool ListReport::toPrinter() const
{
    QPrinter printer(QPrinter::HighResolution);
    printer.setFullPage( true );
    QPrintDialog dlg(&printer);
    dlg.setWindowFlags ( Qt::Window );
    if (dlg.exec() == QDialog::Accepted) {
        auto document = toTextDocument();
        document->print(&printer);
        return true;
    }
    return false;
}

/******************************************************************/

int ListReport::toPreviewDialog() const
{
    QPrinter printer(QPrinter::HighResolution);
    printer.setFullPage( true );
    QPrintPreviewDialog preview(&printer);
    preview.setWindowFlags ( Qt::Window );
    connect(&preview, &QPrintPreviewDialog::paintRequested,
            this, [this](QPrinter *printer){
        auto document = toTextDocument();
        document->print(printer);
    });
    return preview.exec();
}

/******************************************************************/

bool ListReport::toPdfFile(const QString &fileName) const
{
    if (fileName.isEmpty()) return false;

    QPrinter printer(QPrinter::HighResolution);
    printer.setOutputFormat(QPrinter::PdfFormat);
    printer.setOutputFileName(fileName);
    auto document = toTextDocument();
    document->print(&printer);
    return true;
}

/******************************************************************/

bool ListReport::toHtmlFile(const QString &fileName) const
{
    if (fileName.isEmpty()) return false;

    QFile file(fileName);
    if (file.open(QIODevice::WriteOnly | QIODevice::Text)) {
        QTextStream out(&file);
        auto document = toTextDocument();
        out << document->toHtml();
        file.close();
        return true;
    }
    return false;
}

/******************************************************************/
