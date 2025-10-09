#ifndef XREPORTSUTILS_H
#define XREPORTSUTILS_H

#include "XReportsEnums.h"

#include <QDate>
#include <QLocale>
#include <QTextCursor>
#include <QTextDocument>
#include <QTextCharFormat>

#include <QDebug>

QT_BEGIN_NAMESPACE
Q_GUI_EXPORT extern int qt_defaultDpi(); // This is what QTextDocument uses...
QT_END_NAMESPACE

namespace XReports {

/**
 * mmToPixels converts a distance in millimeters, to pixels, for drawing in the watermarkFunction.
 * @param mm distance in millimeters
 * @return distance in pixels
 */
inline qreal mmToPixels(qreal mm) {
    return mm * 0.039370147 // mm to inch
        * qt_defaultDpi(); // inch to pixels
}

/**
 * @internal
 */
inline qreal pixelsToPointsMultiplier(double resolution) {
    // See Q_GUI_EXPORT qreal qt_pixelMultiplier(int resolution) in Qt
    return resolution <= 0 ? 1.0 : 72.0 / resolution;
}

/**
 * @internal
 */
inline void setVariableMarker(QTextDocument &textDoc, int pos, XReports::VariableType variableType, int valueLength) {
    QTextCursor c(&textDoc);
    c.setPosition(pos);
    c.movePosition(QTextCursor::NextCharacter, QTextCursor::KeepAnchor);
    QTextCharFormat charFormat = c.charFormat();
    charFormat.setProperty(ResizableImageProperty, QVariant());
    charFormat.setProperty(VariableTypeProperty, variableType);
    charFormat.setProperty(VariableLengthProperty, valueLength);
    c.setCharFormat(charFormat);
}

/**
 * @internal
 */
inline QString variableValue(int pageNumber, int pageCount, VariableType type) {
    switch (type) {
    case PageNumber:
        return QString::number(pageNumber + 1);
    case PageCount:
        return QString::number(pageCount);
    case TextDate:
        return QDate::currentDate().toString(Qt::TextDate);
    case ISODate:
        return QDate::currentDate().toString(Qt::ISODate);
    case SystemLocaleShortDate:
        return QLocale::system().toString(QDate::currentDate(), QLocale::ShortFormat);
    case SystemLocaleLongDate:
        return QLocale::system().toString(QDate::currentDate(), QLocale::LongFormat);
    case LocaleDate:
    case DefaultLocaleShortDate:
        return QLocale().toString(QDate::currentDate(), QLocale::ShortFormat);
    case DefaultLocaleLongDate:
        return QLocale().toString(QDate::currentDate(), QLocale::LongFormat);
    case TextTime:
        return QTime::currentTime().toString(Qt::TextDate);
    case ISOTime:
        return QTime::currentTime().toString(Qt::ISODate);
    case LocaleTime:
        return QLocale().toString(QTime::currentTime(), QLocale::ShortFormat);
    default:
        qWarning() << "Program error, variable" << type << "not implemented";
    }
    return QString();
}

/**
 * @internal
 */
inline void cleanupVariableProperties(QTextCharFormat &charFormat) {
    charFormat.setProperty(ResizableImageProperty, QVariant());
    charFormat.setProperty(VariableTypeProperty, QVariant());
    charFormat.setProperty(VariableLengthProperty, QVariant());
}

/**
 * @internal
 */
inline QTextCharFormat::VerticalAlignment toVerticalAlignment(Qt::Alignment alignment)
{
    switch (alignment & Qt::AlignVertical_Mask) {
    case Qt::AlignTop:
        return QTextCharFormat::AlignTop;
    case Qt::AlignBottom:
        return QTextCharFormat::AlignBottom;
    case Qt::AlignVCenter:
        return QTextCharFormat::AlignMiddle;
    case Qt::AlignBaseline:
        return QTextCharFormat::AlignBaseline;
    }
    return QTextCharFormat::AlignNormal;
}

/**
 * @internal
 */
inline QString displayText(const QVariant &value)
{
    QLocale locale; // in QStyledItemDelegate this is configurable, it comes from QWidget::locale()...
    QString text;
    switch (value.userType()) {
    case QMetaType::Float:
    case QVariant::Double:
        text = locale.toString(value.toReal());
        break;
    case QVariant::Int:
    case QVariant::LongLong:
        text = locale.toString(value.toLongLong());
        break;
    case QVariant::UInt:
    case QVariant::ULongLong:
        text = locale.toString(value.toULongLong());
        break;
    case QVariant::Date:
        text = locale.toString(value.toDate(), QLocale::ShortFormat);
        break;
    case QVariant::Time:
        text = locale.toString(value.toTime(), QLocale::ShortFormat);
        break;
    case QVariant::DateTime:
        text = locale.toString(value.toDateTime().date(), QLocale::ShortFormat);
        text += QLatin1Char(' ');
        text += locale.toString(value.toDateTime().time(), QLocale::ShortFormat);
        break;
    default:
        text = value.toString();
        break;
    }
    return text;
}

}

#endif // XREPORTSUTILS_H
