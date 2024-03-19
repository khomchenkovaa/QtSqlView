/****************************************************************************
**
** This file is part of the KD Reports library.
**
** SPDX-FileCopyrightText: 2007 Klarälvdalens Datakonsult AB, a KDAB Group company <info@kdab.com>
**
** SPDX-License-Identifier: MIT
**
****************************************************************************/

#ifndef XREPORTS_REPORT_P_H
#define XREPORTS_REPORT_P_H

//
//  W A R N I N G
//  -------------
//
// This file is not part of the KD Reports API.  It exists purely as an
// implementation detail.  This header file may change from version to
// version without notice, or even be removed.
//
// We mean it.
//

#include "XReportsReportLayout.h"
#include "XReportsHeaderMap.h"

#include <QHash>
#include <QMap>

namespace XReports {

class Report;

class ReportPrivate
{
public:
    explicit ReportPrivate(Report *report);

    void setPaperSizeFromPrinter(QSizeF paperSize);
    void ensureLayouted();
    QSizeF paperSize() const;
    void paintPage(int pageNumber, QPainter &painter);
    bool doPrint(QPrinter *printer, QWidget *parent);
    QSizeF layoutAsOnePage(qreal docWidth);
    bool wantEndlessPrinting() const;
    bool hasNonLayoutedTextDocument() const;
    ReportBuilder *builder();

    qreal textDocumentWidth() const; // called by ImageElement, ChartElement
    bool skipHeadersFooters() const;
    qreal rawMainTextDocHeight() const;
    qreal mainTextDocHeight() const;
    QRect mainTextDocRect() const;

    ReportPrivate(const ReportPrivate &) = delete;
    ReportPrivate &operator=(const ReportPrivate &) = delete;

    qreal m_layoutWidth; // in pixels; used for layoutAsOnePage only
    qreal m_endlessPrinterWidth; // in mm
    QPageLayout::Orientation m_orientation;
    mutable QSizeF m_paperSize; // in pixels
    QPageSize m_pageSize;
    QString m_documentName;
    qreal m_marginTop;
    qreal m_marginLeft;
    qreal m_marginBottom;
    qreal m_marginRight;
    qreal m_headerBodySpacing;
    qreal m_footerBodySpacing;
    HeaderMap m_headers;
    HeaderMap m_footers;
    int m_firstPageNumber;
    bool m_pageContentSizeDirty;
    ReportLayout m_layout;
};

}

#endif /* XREPORTS_REPORT_P_H */
