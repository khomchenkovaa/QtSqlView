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

    virtual bool toCsvFile(const QString& fileName) const;
    virtual void toTextCursor(QTextCursor *cursor) const;
    virtual QSharedPointer<QTextDocument> toTextDocument() const;
    virtual bool toPrinter() const;
    virtual int  toPreviewDialog() const;
    virtual bool toPdfFile(const QString& fileName) const;
    virtual bool toHtmlFile(const QString& fileName) const;

protected:
    QAbstractTableModel *m_Model;
    bool b_WithHeaders;

};

#endif // LISTREPORT_H
