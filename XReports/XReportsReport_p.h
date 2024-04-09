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

#include <QHash>
#include <QMap>

namespace XReports {

class Report;

class ReportPrivate
{
public:
    explicit ReportPrivate(Report *report);

    void paintPage(int pageNumber, QPainter &painter);
    bool doPrint(QPrinter *printer, QWidget *parent);
    bool hasNonLayoutedTextDocument() const;
    ReportBuilder *builder();

    ReportPrivate(const ReportPrivate &) = delete;
    ReportPrivate &operator=(const ReportPrivate &) = delete;

    qreal m_layoutWidth; // in pixels; used for layoutAsOnePage only
    QPageLayout::Orientation m_orientation;
    QString m_documentName;
    int m_firstPageNumber;
    ReportLayout m_layout;
};

}

#endif /* XREPORTS_REPORT_P_H */
