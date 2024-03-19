#include "XReportsTextElement.h"
#include "XReportsReportBuilder.h"

void XReports::TextElement::build(ReportBuilder &builder) const {
    QTextCursor &cursor = builder.cursor();
    const int charPosition = cursor.position();
    QTextCharFormat charFormat = cursor.charFormat();
    XReports::cleanupVariableProperties(charFormat);
    if (d.fontSet) {
#if QT_VERSION >= QT_VERSION_CHECK(5, 3, 0)
        charFormat.setFont(d.font, QTextCharFormat::FontPropertiesSpecifiedOnly);
#else
        charFormat.setFont(d.font);
#endif
    }
    if (d.boldIsSet)
        charFormat.setFontWeight(d.bold ? QFont::Bold : QFont::Normal);
    if (d.italicIsSet)
        charFormat.setFontItalic(d.italic);
    if (d.underlineIsSet)
        charFormat.setFontUnderline(d.underline);
    if (d.strikeoutIsSet)
        charFormat.setFontStrikeOut(d.strikeout);
    if (d.pointSize > 0)
        charFormat.setFontPointSize(d.pointSize);
    if (!d.fontFamily.isEmpty())
        charFormat.setFontFamily(d.fontFamily);
    if (d.foreground.isValid())
        charFormat.setForeground(d.foreground);
    else
        charFormat.clearForeground();
    if (background().style() != Qt::NoBrush)
        charFormat.setBackground(background());
    else
        charFormat.clearBackground();
    cursor.setCharFormat(charFormat);
    cursor.insertText(d.string);
    if (!d.id.isEmpty())
        builder.currentDocumentData().setTextValueMarker(charPosition, d.id, d.string.length(), false);
}
