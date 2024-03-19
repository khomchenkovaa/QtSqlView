#include "XReportsFrameElement.h"

#include "XReportsUtils.h"
#include "XReportsReportBuilder.h"

#include <QTextFrame>

#include <QDebug>

void XReports::FrameElement::build(ReportBuilder &builder) const
{
    // prepare the frame
    QTextFrameFormat format;
    if (d.width) {
        if (d.widthUnit == Millimeters) {
            format.setWidth(QTextLength(QTextLength::FixedLength, mmToPixels(d.width)));
        } else {
            format.setWidth(QTextLength(QTextLength::PercentageLength, d.width));
        }
    }
    if (d.height) {
        if (d.heightUnit == Millimeters) {
            format.setHeight(QTextLength(QTextLength::FixedLength, mmToPixels(d.height)));
        } else {
            format.setHeight(QTextLength(QTextLength::PercentageLength, d.height));
        }
    }

    format.setPadding(mmToPixels(padding()));
    format.setBorder(d.border);
    // TODO borderBrush like in AbstractTableElement
    format.setPosition(static_cast<QTextFrameFormat::Position>(d.position)); // those enums have the same value

    QTextCursor &textDocCursor = builder.cursor();

    QTextFrame *frame = textDocCursor.insertFrame(format);

    QTextCursor contentsCursor = frame->firstCursorPosition();

    ReportBuilder contentsBuilder(builder.currentDocumentData(), contentsCursor, builder.report());
    contentsBuilder.copyStateFrom(builder);

    foreach (const XReports::ElementData &ed, d.elements) {
        switch (ed.m_type) {
        case XReports::ElementData::Inline:
            contentsBuilder.addInlineElement(*ed.m_element);
            break;
        case XReports::ElementData::Block:
            contentsBuilder.addBlockElement(*ed.m_element, ed.m_align);
            break;
        case XReports::ElementData::Variable:
            contentsBuilder.addVariable(ed.m_variableType);
            break;
        case XReports::ElementData::VerticalSpacing:
            contentsBuilder.addVerticalSpacing(ed.m_value);
            break;
        }
    }

    textDocCursor.movePosition(QTextCursor::End);
}


