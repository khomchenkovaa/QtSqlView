#ifndef XREPORTS_HLINETEXTOBJECT_H
#define XREPORTS_HLINETEXTOBJECT_H

#include <QTextObjectInterface>

QT_BEGIN_NAMESPACE
class QTextDocument;
class QTextFormat;
class QPainter;
class QRectF;
QT_END_NAMESPACE

namespace XReports {

class HLineTextObject : public QObject, public QTextObjectInterface
{
    Q_OBJECT
    Q_INTERFACES(QTextObjectInterface)

public:
    using QObject::QObject;

    enum { // QTextCharFormat objectType
        HLineTextFormat = QTextFormat::UserObject + 1
    };
    enum { // QTextCharFormat property
        Color = 1,
        Thickness = 2,
        Margin = 3
    };

    /**
     * Registers the HLineTextObject (a QTextObjectInterface)
     * handler with a QTextDocument in order to enable the
     * painting of HLineTextObjects
     */
    static void registerHLineObjectHandler(QTextDocument *doc);

    QSizeF intrinsicSize(QTextDocument *doc, int posInDocument, const QTextFormat &format) override;

    void drawObject(QPainter *painter, const QRectF &rect, QTextDocument *doc, int posInDocument, const QTextFormat &format) override;
};

} // namespace XReports

#endif /* XREPORTS_HLINETEXTOBJECT_H */
