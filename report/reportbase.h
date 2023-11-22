#ifndef REPORTBASE_H
#define REPORTBASE_H

#include "reportelement.h"

#include <QObject>

#include <QFile>
#include <QTextStream>
#include <QTextDocument>

#include <QPrinter>
#include <QPrintDialog>
#include <QPrintPreviewDialog>

namespace Report {

class ReportBase : public QObject
{
    Q_OBJECT
public:
    explicit ReportBase(QObject *parent = nullptr) : QObject(parent) {}

    virtual QSharedPointer<QTextDocument> toTextDocument() const {
        QSharedPointer<QTextDocument> document(new QTextDocument());
        return document;
    }

    virtual bool toPrinter() const {
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

    virtual int  toPreviewDialog() const {
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

    virtual bool toPdfFile(const QString& fileName) const {
        if (fileName.isEmpty()) return false;

        QPrinter printer(QPrinter::HighResolution);
        printer.setOutputFormat(QPrinter::PdfFormat);
        printer.setOutputFileName(fileName);
        auto document = toTextDocument();
        document->print(&printer);
        return true;
    }

    virtual bool toHtmlFile(const QString& fileName) const {
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
};

} // Report

#endif // REPORTBASE_H
