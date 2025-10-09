#include "XReportsHLineElement.h"
#include "XReportsHLineTextObject.h"
#include "XReportsUtils.h" // mmToPixels
#include "XReportsReportBuilder.h"
#include <QDebug>

void XReports::HLineElement::build(XReports::ReportBuilder &builder) const
{
    QTextCursor &cursor = builder.cursor();

    QTextCharFormat fmt;

    fmt.setObjectType(HLineTextObject::HLineTextFormat);
    fmt.setProperty(HLineTextObject::Color, d.color);
    fmt.setProperty(HLineTextObject::Thickness, d.thickness);
    // qDebug() << "Setting Margin to" << d->m_margin << "mm" << mmToPixels( d->m_margin ) << "px";
    fmt.setProperty(HLineTextObject::Margin, double(mmToPixels(d.margin)));

    cursor.insertText(QString(QChar::ObjectReplacementCharacter), fmt);
}
