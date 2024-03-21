#ifndef XREPORTS_ENUMS_H
#define XREPORTS_ENUMS_H

#include <QTextFormat>

namespace XReports {

//! Type of variable
enum VariableType
{
    PageNumber, ///< Page number
    PageCount, ///< Page count
    TextDate, ///< Current date in text format like "Thu Aug 18 2005", with translated
    ///  names but unlocalized order of names and numbers
    ISODate, ///< Current date in ISO 8601 format like "2005-08-18"
    LocaleDate, ///< Current date in locale-dependent format, deprecated in favour of SystemLocaleShortDate or SystemLocaleLongDate.
    TextTime, ///< Current time in text format like "13:42:59"
    ISOTime, ///< Current time in ISO 8601 format like "13:42:59"
    LocaleTime, ///< Current time in locale-dependent format

    SystemLocaleShortDate, ///< Current date formatted according to the system locale, short format, see Qt::SystemLocaleShortDate
    SystemLocaleLongDate, ///< Current date formatted according to the system locale, long format, see Qt::SystemLocaleLongDate
    DefaultLocaleShortDate, ///< Current date formatted according to the application locale, short format, see Qt::DefaultLocaleShortDate
    DefaultLocaleLongDate, ///< Current date formatted according to the application locale, long format, see Qt::DefaultLocaleLongDate

    NVariableTypes ///< @internal
};

//! Units used when setting a size.
enum Unit
{
    Millimeters, ///< Millimeters (the default)
    Percent ///< Percentage of the text width, i.e. the page width minus margins
};

enum VariableProperty {
    VariableTypeProperty   = QTextFormat::UserProperty + 246,
    VariableLengthProperty = QTextFormat::UserProperty + 247,
    HeaderColumnsProperty  = QTextFormat::UserProperty + 248,
    ResizableImageProperty = QTextFormat::UserProperty + 5984
};

} // namespace XReports

#endif /* XREPORTS_ENUMS_H */
