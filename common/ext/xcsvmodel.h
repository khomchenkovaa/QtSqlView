#ifndef XCSVMODEL_H
#define XCSVMODEL_H

#include <QAbstractTableModel>
#include <QVariant>
#include <QIODevice>
#include <QChar>
#include <QString>
#include <QStringList>
#include <QModelIndex>

class QTextCodec;

class XCsvModel : public QAbstractTableModel
{
    Q_OBJECT

public:
    enum QuoteOption {
        NoQuotes          = 0,
        SingleQuote       = 1,
        DoubleQuote       = 2,
        BothQuotes        = 3,
        NoEscape          = 0,
        TwoQuoteEscape    = 4,
        BackslashEscape   = 8,
        AlwaysQuoteOutput = 16,
        DefaultQuoteMode  = BothQuotes | BackslashEscape | AlwaysQuoteOutput
    };
    Q_DECLARE_FLAGS(QuoteMode, QuoteOption)

    XCsvModel(QObject *parent = Q_NULLPTR);
    explicit XCsvModel(QIODevice *file, QObject *parent = 0, bool withHeader = false, QChar separator = ',');
    explicit XCsvModel(const QString filename, QObject *parent = 0, bool withHeader = false, QChar separator = ',');
    ~XCsvModel();

    int rowCount(const QModelIndex& parent = QModelIndex()) const;
    int columnCount(const QModelIndex& parent = QModelIndex()) const;
    QVariant data(const QModelIndex& index, int role = Qt::DisplayRole) const;
    bool setData(const QModelIndex& index, const QVariant& data, int role = Qt::EditRole);
    QVariant headerData(int section, Qt::Orientation orientation, int role = Qt::DisplayRole) const;
    bool setHeaderData(int section, Qt::Orientation orientation, const QVariant& value, int role = Qt::DisplayRole);
    void setHeaderData(const QStringList& data);
    QString text(int row, int column) const;
    void setText(int row, int column, const QString& value);
    QString headerText(int column) const;
    void setHeaderText(int column, const QString& value);

    bool insertRows(int row, int count, const QModelIndex& parent = QModelIndex());
    bool removeRows(int row, int count, const QModelIndex& parent = QModelIndex());
    bool insertColumns(int col, int count, const QModelIndex& parent = QModelIndex());
    bool removeColumns(int col, int count, const QModelIndex& parent = QModelIndex());

    void setSource(QIODevice *file, bool withHeader = false, QChar separator = ',', QTextCodec* codec = Q_NULLPTR);
    void setSource(const QString& filename, bool withHeader = false, QChar separator = ',', QTextCodec* codec = Q_NULLPTR);

    void toCSV(QIODevice *file, bool withHeader = false, QChar separator = ',', QTextCodec* codec = Q_NULLPTR) const;
    void toCSV(const QString& filename, bool withHeader = false, QChar separator = ',', QTextCodec* codec = Q_NULLPTR) const;

    QString toHTML(bool withHeader = false, int numEmpty = 0) const;

    QuoteMode quoteMode() const;
    void setQuoteMode(QuoteMode mode);

    Qt::ItemFlags flags(const QModelIndex& index) const;

    void importFromModel(QAbstractItemModel *model);

private:
    QList<QStringList> m_CsvData;
    QStringList        m_Header;
    int                m_MaxColumn;
    QuoteMode          m_QuoteMode;
};

Q_DECLARE_OPERATORS_FOR_FLAGS(XCsvModel::QuoteMode)

#endif // XCSVMODEL_H
