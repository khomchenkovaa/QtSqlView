#ifndef LISTREPORT_H
#define LISTREPORT_H

#include <QObject>
#include <QTextDocument>

QT_BEGIN_NAMESPACE
class QTextCursor;
class QAbstractTableModel;
QT_END_NAMESPACE

class ListReport : public QObject
{
    Q_OBJECT
public:
    explicit ListReport(QObject *parent = nullptr);
    void setModel(QAbstractTableModel *model);
    void setWithHeaders(bool withHeaders);

    virtual void toCsv(const QString& fileName) const;
    virtual void printTo(QTextCursor *cursor) const;
    virtual QSharedPointer<QTextDocument> toDocument() const;
    virtual void toPreview() const;
    virtual void toPdf(const QString& fileName) const;

public:
    static void exportToCsv(QAbstractTableModel *model);
    static void printTable(QTextCursor *cursor, QAbstractTableModel *model);

protected:
    QAbstractTableModel *m_Model;
    bool b_WithHeaders;

};

#endif // LISTREPORT_H
