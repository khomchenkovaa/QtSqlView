#ifndef REPORTDOC_H
#define REPORTDOC_H

#include "reportbase.h"

#include <QFile>
#include <QTextStream>
#include <QPrintDialog>
#include <QPrintPreviewDialog>

namespace Report {

class ReportDoc : public ReportBase
{
    Q_OBJECT

public:
    explicit ReportDoc(QObject *parent = nullptr)
        : ReportBase(parent)
        , m_Document(new QTextDocument())
        {}

    virtual QSharedPointer<QTextDocument> toTextDocument() const {
        return m_Document;
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
            out << toHtml("utf-8");
            file.close();
            return true;
        }
        return false;
    }

    //! Clears the document
    void clear() {
        m_Document->clear();
    }

    //! \return a string containing an HTML representation of the document
    QString toHtml(const QByteArray &encoding = QByteArray()) const {
        return m_Document->toHtml(encoding);
    }

    //! Replaces the entire contents of the document with the given HTML-formatted text in the html string
    void setHtml(const QString& html) {
        m_Document->setHtml(html);
    }

    //! \return the plain text contained in the document
    QString toPlainText() const {
        return m_Document->toPlainText();
    }

    //! Replaces the entire contents of the document with the given plain text.
    void setPlainText(const QString& text) {
        m_Document->setPlainText(text);
    }

private:
    QSharedPointer<QTextDocument> m_Document;
};

} // namespace Report

#endif // REPORTDOC_H
