#ifndef XUTILS_H
#define XUTILS_H

#include <QDateTime>
#include <QFile>
#include <QMetaObject>
#include <QMetaClassInfo>

#include <QPointer>
#include <QCryptographicHash>

#include <QDebug>

typedef QPair<QString, QVariant> CmbItem;
typedef QList<CmbItem>           CmbList;

typedef QPair<QString, QVariantList> CmbExtItem;
typedef QList<CmbExtItem>            CmbExtList;

namespace Utils {

/******************************************************************/

inline QString fromResource(const QString& fileName)
{
    QFile file(fileName);

    if (!file.open(QIODevice::ReadOnly | QIODevice::Text)) {
        qDebug() << file.errorString();
        return QString();
    }

    QString result = file.readAll();
    file.close();

    return result;
}

/******************************************************************/
/**
 * @brief gets the root node of the object's hierarchy
 * @param item the member of the object's hierarchy
 * @return root of the hierarchy
 */
inline QObject *root(QObject *item) {
    while (item->parent()) {
        item = item->parent();
    }
    return item;
}

/******************************************************************/

inline const char *metaInfo(const QMetaObject *meta, const char *name) {
    int idx = meta->indexOfClassInfo(name);
    if (idx == -1) {
        qDebug() << "Can't find " << name << " metainfo for class " << meta->className();
        return "";
    }
    return meta->classInfo(idx).value();
}

/******************************************************************/

inline QMetaEnum metaEnum(const QMetaObject *meta, const char *enumName) {
    const int idx = meta->indexOfEnumerator(enumName);
    return meta->enumerator(idx);
}

/******************************************************************/

inline const char *metaEnumKey(const QMetaObject *meta, const char *enumName, int key) {
    return metaEnum(meta, enumName).key(key);
}

/******************************************************************/

inline int metaEnumKeyToValue(const QMetaObject *meta, const char *enumName, const QString& key) {
    return metaEnum(meta, enumName).keyToValue(key.toLocal8Bit().data());
}

/******************************************************************/

inline void DestructorMsg(const QString& value)
{
    static const QString DESTRUCTOR_MSG =
        QStringLiteral("Running the %1 destructor.");

    qDebug() << DESTRUCTOR_MSG.arg(value);
}

/******************************************************************/

inline void DestructorMsg(const QPointer<QObject> object)
{
    DestructorMsg(object->metaObject()->className());
}

/******************************************************************/

inline QByteArray md5(const QString &text)
{
    return QCryptographicHash::hash(text.toUtf8(), QCryptographicHash::Md5).toHex();
}

/******************************************************************/
//! convert msec to format '[N days] HH:MM:SS'
inline QString duration(qint64 msec)
{
    QDateTime dur = QDateTime::fromMSecsSinceEpoch(msec, Qt::UTC);
    qint64   days = QDateTime().daysTo(dur);
    QString hours = dur.toString("hh:mm:ss");
    if (days > 0) {
        return QString("%1 days %2").arg(days).arg(hours);
    }
    return hours;
}

/******************************************************************/
//! Print file size in human redabale format
inline QString sizeHuman(double dataSize)
{
    static const QStringList list =
        QStringList() << "KB" << "MB" << "GB" << "TB";

    QStringListIterator i(list);
    QString unit("B");

    while(dataSize >= 1024.0 && i.hasNext()) {
        unit = i.next();
        dataSize /= 1024.0;
    }
    return QString().setNum(dataSize,'f',2)+" "+unit;
}

/******************************************************************/
//! Ecapes string according to C language standards: https://en.wikipedia.org/wiki/Escape_sequences_in_C
inline QString escape(const QString &str)
{
    QString result;
    for (int i = 0; i < str.size(); ++i) {
        switch(str.at(i).unicode()) {
        case QChar::Null:           result.append("\\0");  break;
        case 0x0007:                result.append("\\a");  break; // Alert (Beep, Bell)
        case 0x0008:                result.append("\\b");  break; // Backspace
        case QChar::Tabulation:     result.append("\\t");  break; // Horizontal Tab
        case QChar::LineFeed:       result.append("\\n");  break; // Newline
        case 0x000b:                result.append("\\v");  break; // Vertical Tab
        case 0x000c:                result.append("\\f");  break; // Formfeed Page Break
        case QChar::CarriageReturn: result.append("\\r");  break;
        case 0x001b:                result.append("\\e");  break; // Escape character
        case 0x0022:                result.append("\\\""); break; // Double quotation mark
        case 0x0027:                result.append("\\'");  break; // Apostrophe or single quotation mark
//        case 0x003f:                result.append("\\?");  break; // Question mark (used to avoid trigraphs)
        case 0x005c:                result.append("\\\\"); break; // Backslash
        default: result.append(str.at(i));
        }
    }
    return result;
}

/******************************************************************/

inline bool decodeFromVariant(const QVariant &value, bool defValue = false)
{
    if (value.toString().compare("Yes", Qt::CaseInsensitive) == 0) {
        return true;
    }
    if (value.toString().compare("No", Qt::CaseInsensitive) == 0) {
        return false;
    }
    if (value.canConvert(QVariant::Bool)) {
        return value.toBool();
    }
    return defValue;
}

/******************************************************************/

inline QString boolToString(bool value)
{
    return value? "Yes" : "No";
}

/******************************************************************/

inline QString varFloatToStr(const QVariant &val)
{
    QString result;
    if (val.isValid()) {
        result = QString::number(val.toDouble());
    }
    return result;
}

/******************************************************************/

inline QVariant strToVarReal(const QString &val)
{
    QVariant result;
    bool ok;
    qreal realVal = val.toDouble(&ok);
    if (ok) {
        result = realVal;
    }
    return result;
}

/******************************************************************/


} // namespace Utils

#endif // XUTILS_H
