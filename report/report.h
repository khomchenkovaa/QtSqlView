#ifndef REPORT_H
#define REPORT_H

#include "reporttypes.h"

#include "reportdoc.h"
#include "simplereport.h"
#ifdef KD_REPORTS
#include "kdxmlreport.h"
#endif

#include <QFileDialog>

#include <QRegularExpression>

namespace Report {

/******************************************************************/

inline QStringList findBindings(const QString &sql)
{
    QStringList result;
    QRegularExpression re(":[\\d\\w]+");
    QRegularExpressionMatchIterator i = re.globalMatch(sql);
    while (i.hasNext()) {
        QRegularExpressionMatch match = i.next();
        QString word = match.captured(0);
        if (!result.contains(word)) {
            result << word;
        }
    }
    return result;
}

/******************************************************************/

inline QString exportToPdfDlg(QWidget *parent = Q_NULLPTR, const QString &caption = "Export PDF")
{
    QFileDialog fileDialog(parent, caption);
    fileDialog.setAcceptMode(QFileDialog::AcceptSave);
    fileDialog.setMimeTypeFilters(QStringList("application/pdf"));
    fileDialog.setDefaultSuffix("pdf");
    if (fileDialog.exec() != QDialog::Accepted) {
        return QString();
    }
    return fileDialog.selectedFiles().constFirst();
}

/******************************************************************/

inline QString exportToHtmlDlg(QWidget *parent = Q_NULLPTR, const QString &caption = "Export HTML")
{
    QFileDialog fileDialog(parent, caption);
    fileDialog.setAcceptMode(QFileDialog::AcceptSave);
    fileDialog.setMimeTypeFilters(QStringList("text/html"));
    fileDialog.setDefaultSuffix("html");
    if (fileDialog.exec() != QDialog::Accepted) {
        return QString();
    }
    return fileDialog.selectedFiles().constFirst();
}

/******************************************************************/

inline QString exportToCsvDlg(QWidget *parent = Q_NULLPTR, const QString &caption = "Export CSV")
{
    QFileDialog fileDialog(parent, caption);
    fileDialog.setAcceptMode(QFileDialog::AcceptSave);
    fileDialog.setMimeTypeFilters(QStringList("text/csv"));
    fileDialog.setDefaultSuffix("csv");
    if (fileDialog.exec() != QDialog::Accepted) {
        return QString();
    }
    return fileDialog.selectedFiles().constFirst();
}

/******************************************************************/

} // namespace Report

#endif // REPORT_H
