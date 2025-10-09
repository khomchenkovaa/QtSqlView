/**
 * \class XCsvModel
 * \brief The XCsvModel class provides a QAbstractTableModel for CSV Files
 */
#include "xcsvmodel.h"
#include <QFile>
#include <QTextStream>
#include <QUrl>
#include <QDebug>

/******************************************************************/
/**
 * Creates an empty XCsvModel with parent \a parent.
 */
XCsvModel::XCsvModel(QObject *parent)
    : QAbstractTableModel(parent)
    , m_MaxColumn(0)
    , m_QuoteMode(XCsvModel::DefaultQuoteMode)
{
}

/******************************************************************/
/**
 * Creates a XCsvModel with the parent \a parent and content loaded from \a file.
 *
 * See \a setSource for information on the \a withHeader and \a separator properties, or
 * if you need control over the quoting method or codec used to parse the file.
 *
 * \sa setSource
 */
XCsvModel::XCsvModel(QIODevice *file, QObject *parent, bool withHeader, QChar separator)
    : QAbstractTableModel(parent)
    , m_MaxColumn(0)
    , m_QuoteMode(XCsvModel::DefaultQuoteMode)
{
    setSource(file, withHeader, separator);
}

/******************************************************************/
/**
 * \overload
 *
 * Creates a XCsvModel with the parent \a parent and content loaded from \a file.
 *
 * See \a setSource for information on the \a withHeader and \a separator properties, or
 * if you need control over the quoting method or codec used to parse the file.
 *
 * \sa setSource
 */
XCsvModel::XCsvModel(const QString filename, QObject *parent, bool withHeader, QChar separator)
    : QAbstractTableModel(parent)
    , m_MaxColumn(0)
    , m_QuoteMode(XCsvModel::DefaultQuoteMode)
{
    QFile src(filename);
    setSource(&src, withHeader, separator);
}

/******************************************************************/

XCsvModel::~XCsvModel()
{}

/******************************************************************/
/**
 * \reimp
 */
int XCsvModel::rowCount(const QModelIndex& parent) const
{
    if (parent.isValid()) return 0;
    return m_CsvData.count();
}

/******************************************************************/
/**
 * \reimp
 */
int XCsvModel::columnCount(const QModelIndex& parent) const
{
    if (parent.isValid()) return 0;
    return m_MaxColumn;
}

/******************************************************************/
/**
 * \reimp
 */
QVariant XCsvModel::data(const QModelIndex& index, int role) const
{
    if(index.parent().isValid()) {
        return QVariant();
    }
    if(role == Qt::DisplayRole || role == Qt::EditRole || role == Qt::UserRole) {
        if(index.row() < 0 || index.column() < 0 || index.row() >= rowCount()) {
            return QVariant();
        }
        const QStringList& row = m_CsvData[index.row()];
        if(index.column() >= row.length()) {
            return QVariant();
        }
        return row[index.column()];
    }
    return QVariant();
}

/******************************************************************/
/**
 * \reimp
 */
QVariant XCsvModel::headerData(int section, Qt::Orientation orientation, int role) const
{
    if(section < m_Header.count() && orientation == Qt::Horizontal && (role == Qt::DisplayRole || role == Qt::EditRole || role == Qt::UserRole)) {
        return m_Header[section];
    }
    return QAbstractTableModel::headerData(section, orientation, role);
}

/******************************************************************/
/**
 * \overload
 *
 * Reads in a CSV file from the provided \a file using \a codec.
 */
void XCsvModel::setSource(const QString& filename, bool withHeader, QChar separator, QTextCodec* codec)
{
    QFile src(filename);
    setSource(&src, withHeader, separator, codec);
}

/******************************************************************/
/**
 * Reads in a CSV file from the provided \a file using \a codec.
 *
 * The value of \a separator will be used to delimit fields, subject to the specified \a quoteMode.
 * If \a withHeader is set to true, the first line of the file will be used to populate the model's
 * horizontal header.
 *
 * \sa quoteMode
 */
void XCsvModel::setSource(QIODevice *file, bool withHeader, QChar separator, QTextCodec* codec)
{
    bool headerSet = !withHeader;
    if(!file->isOpen()) {
        file->open(QIODevice::ReadOnly);
    }
    m_MaxColumn = withHeader ? 0 : m_Header.size();
    m_CsvData.clear();
    QStringList row;
    QString field;
    QChar quote;
    QChar ch, buffer(0);
    bool readCR = false;
    QTextStream stream(file);
    if(codec) {
        stream.setCodec(codec);
    } else {
        stream.setAutoDetectUnicode(true);
    }
    while(!stream.atEnd()) {
        if(buffer != QChar(0)) {
            ch = buffer; 
            buffer = QChar(0);
        } else {
            stream >> ch;
        }
        if(ch == '\n' && readCR) {
            continue;            
        } else if(ch == '\r') {
            readCR = true;
        } else {
            readCR = false;
        }
        if(ch != separator && (ch.category() == QChar::Separator_Line || ch.category() == QChar::Separator_Paragraph || ch.category() == QChar::Other_Control)) {
            row << field;
            field.clear();
            if(!row.isEmpty()) {
                if(!headerSet) {
                    m_Header = row;
                    headerSet = true;
                } else {
                    m_CsvData.append(row);
                }
                if(row.length() > m_MaxColumn) {
                    m_MaxColumn = row.length();
                }
            }
            row.clear();
        } else if((m_QuoteMode & DoubleQuote && ch == '"') || (m_QuoteMode & SingleQuote && ch == '\'')) {
            quote = ch;
            do {
                stream >> ch;
                if(ch == '\\' && m_QuoteMode & BackslashEscape) {
                    stream >> ch;
                } else if(ch == quote) {
                    if(m_QuoteMode & TwoQuoteEscape) {
                        stream >> buffer;
                        if(buffer == quote) {
                            buffer = QChar(0);
                            field.append(ch);
                            continue;
                        }
                    }
                    break;
                }
                field.append(ch);
            } while(!stream.atEnd());
        } else if(ch == separator) {
            row << field;
            field.clear();
        } else {
            field.append(ch);
        }
    }
    if(!field.isEmpty()) {
        row << field;
    }
    if(!row.isEmpty()) {
        if(!headerSet) {
            m_Header = row;
        } else {
            m_CsvData.append(row);
        }
    }
    file->close();
}

/******************************************************************/
/**
 * Sets the horizontal headers of the model to the values provided in \a data.
 */
void XCsvModel::setHeaderData(const QStringList& data)
{
    m_Header = data;
    emit headerDataChanged(Qt::Horizontal, 0, data.count());
}

/******************************************************************/
/**
 * \reimp
 */
bool XCsvModel::setHeaderData(int section, Qt::Orientation orientation, const QVariant& value, int role)
{
    if(orientation != Qt::Horizontal) return false;                   // We don't support the vertical header
    if(role != Qt::DisplayRole && role != Qt::EditRole) return false; // We don't support any other roles
    if(section < 0) return false;                                     // Bogus input
    while(section > m_Header.size()) {
        m_Header << QString();
    }
    m_Header[section] = value.toString();
    emit headerDataChanged(Qt::Horizontal, section, section);
    return true;
}

/******************************************************************/
/**
 * \reimp
 */
bool XCsvModel::setData(const QModelIndex& index, const QVariant& data, int role)
{
    if (index.parent().isValid()) return false;

    if(role == Qt::DisplayRole || role == Qt::EditRole || role == Qt::UserRole) {
        if(index.row() >= rowCount() || index.column() >= columnCount() || index.row() < 0 || index.column() < 0) {
            return false;
        }
        QStringList& row = m_CsvData[index.row()];
        while(row.length() <= index.column()) {
            row << QString();
        }
        row[index.column()] = data.toString();
        emit dataChanged(index, index);
        return true;
    }
    return false;
}

/******************************************************************/
/**
 * \reimp
 */
bool XCsvModel::insertRows(int row, int count, const QModelIndex& parent)
{
    if (parent.isValid() || row < 0) return false;
    beginInsertRows(parent, row, row + count);

    if(row >= rowCount()) {
        for(int i = 0; i < count; i++) m_CsvData << QStringList();
    } else {
        for(int i = 0; i < count; i++) m_CsvData.insert(row, QStringList());
    }
    endInsertRows();
    return true;
}

/******************************************************************/
/**
 * \reimp
 */
bool XCsvModel::removeRows(int row, int count, const QModelIndex& parent)
{
    if (parent.isValid() || row < 0 || row >= rowCount()) {
        return false;
    }
    if (row + count >= rowCount()) {
        count = rowCount() - row;
    }
    beginRemoveRows(parent, row, row + count);
    for (int i = 0; i < count; i++) {
        m_CsvData.removeAt(row);
    }
    endRemoveRows();
    return true;
}

/******************************************************************/
/**
 * \reimp
 */
bool XCsvModel::insertColumns(int col, int count, const QModelIndex& parent)
{
    if (parent.isValid() || col < 0) {
        return false;
    }
    beginInsertColumns(parent, col, col + count - 1);
    for(int i = 0; i < rowCount(); i++) {
        QStringList& row = m_CsvData[i];
        while(col >= row.length()) {
            row.append(QString());
        }
        for(int j = 0; j < count; j++) {
            row.insert(col, QString());
        }
    }
    for(int i = 0; i < count ;i++) {
        m_Header.insert(col, QString());
    }
    m_MaxColumn += count;
    endInsertColumns();
    return true;
}

/******************************************************************/
/**
 * \reimp
 */
bool XCsvModel::removeColumns(int col, int count, const QModelIndex& parent)
{
    if (parent.isValid() || col < 0 || col >= columnCount()) {
        return false;
    }
    if (col + count >= columnCount()) {
        count = columnCount() - col;
    }
    beginRemoveColumns(parent, col, col + count);
    for(int i = 0; i < rowCount(); i++) {
        for(int j = 0; j < count; j++) {
            m_CsvData[i].removeAt(col);
        }
    }
    for(int i = 0; i < count; i++) {
        m_Header.removeAt(col);
    }
    endRemoveColumns();
    return true;
}

/******************************************************************/

static QString xAddCsvQuotes(XCsvModel::QuoteMode mode, QString field)
{
    bool addDoubleQuotes = ((mode & XCsvModel::DoubleQuote) && field.contains('"'));
    bool addSingleQuotes = ((mode & XCsvModel::SingleQuote) && field.contains('\''));
    bool quoteField = (mode & XCsvModel::AlwaysQuoteOutput) || addDoubleQuotes || addSingleQuotes;
    if(quoteField && !addDoubleQuotes && !addSingleQuotes) {
        if(mode & XCsvModel::DoubleQuote) {
            addDoubleQuotes = true;
        } else if(mode & XCsvModel::SingleQuote) {
            addSingleQuotes = true;
        }
    } 
    if(mode & XCsvModel::BackslashEscape) {
        if(addDoubleQuotes) {
            return '"' + field.replace("\\", "\\\\").replace("\"", "\\\"") + '"';
        }
        if(addSingleQuotes) {
            return '\'' + field.replace("\\", "\\\\").replace("'", "\\'") + '\'';
        }
    } else {
        if(addDoubleQuotes) {
            return '"' + field.replace("\"", "\"\"") + '"';
        }
        if(addSingleQuotes) {
            return '\'' + field.replace("'", "''") + '\'';
        }
    }
    return field;
}

/******************************************************************/
/*!
  Outputs the content of the model as a CSV file to the device \a dest using \a codec.

  Fields in the output file will be separated by \a separator. Set \a withHeader to true
  to output a row of headers at the top of the file.
 */ 
void XCsvModel::toCSV(QIODevice* dest, bool withHeader, QChar separator, QTextCodec* codec) const
{
    int row, col, rows, cols;
    rows = rowCount();
    cols = columnCount();
    QString data;
    if(!dest->isOpen()) {
        dest->open(QIODevice::WriteOnly | QIODevice::Truncate);
    }
    QTextStream stream(dest);
    if(codec) {
        stream.setCodec(codec);
    }
    if(withHeader) {
        data = "";
        for(col = 0; col < cols; ++col) {
            if(col > 0) data += separator;
            data += xAddCsvQuotes(m_QuoteMode, m_Header.at(col).simplified());
        }
        stream << data << Qt::endl;
    }
    for(row = 0; row < rows; ++row) {
        const QStringList& rowData = m_CsvData[row];
        data = "";
        for(col = 0; col < cols; ++col) {
            if(col > 0) {
                data += separator;
            }
            if(col < rowData.length()) {
                data += xAddCsvQuotes(m_QuoteMode, rowData.at(col).simplified());
            } else {
                data += xAddCsvQuotes(m_QuoteMode, QString());
            }
        }
        stream << data << Qt::endl;
    }
    stream << Qt::flush;
    dest->close();
}

/******************************************************************/
/*!
  \overload

  Outputs the content of the model as a CSV file to the file specified by \a filename using \a codec.

  Fields in the output file will be separated by \a separator. Set \a withHeader to true
  to output a row of headers at the top of the file.
 */ 
void XCsvModel::toCSV(const QString& filename, bool withHeader, QChar separator, QTextCodec* codec) const
{
    QFile dest(filename);
    toCSV(&dest, withHeader, separator, codec);
}

/******************************************************************/
//! Outputs the content of the model as a HTML table.
QString XCsvModel::toHTML(bool withHeader, int numEmpty) const
{
    int rows = rowCount();
    int cols = columnCount();
    QString result;
    QTextStream stream(&result);
    stream << "<table border=1 cellspacing=0 cellpadding=0 width=\"100%\">" << Qt::endl;
    if(withHeader) {
        stream << "<tr>" << Qt::endl;
        for(int col = 0; col < cols; ++col) {
//            auto htmlEncoded = QUrl::toPercentEncoding(m_Header.at(col).simplified());
            stream << "<td valign=top>" << "<p align=center>" << m_Header.at(col).simplified() << "</p>" << "</td>" << Qt::endl;
        }
        stream << "</tr>" << Qt::endl;
    }
    if (!rows && numEmpty) {
        for (int row = 0; row < numEmpty; ++row) {
            stream << "<tr>" << Qt::endl;
            for(int col = 0; col < cols; ++col) {
                stream << "<td>" << "&nbsp;" << "</td>" << Qt::endl;
            }
            stream << "</tr>" << Qt::endl;
        }
    }
    for(int row = 0; row < rows; ++row) {
        const QStringList& rowData = m_CsvData[row];
        stream << "<tr>" << Qt::endl;
        for(int col = 0; col < cols; ++col) {
            if(col < rowData.length()) {
//                auto htmlEncoded = QUrl::toPercentEncoding(rowData.at(col).simplified());
                stream << "<td valign=top>" << "<p align=center>" << rowData.at(col).simplified() << "</p>" << "</td>" << Qt::endl;
            } else {
                stream << "<td>" << "&nbsp;" << "</td>" << Qt::endl;
            }
        }
        stream << "</tr>" << Qt::endl;
    }
    stream << "</table>" << Qt::endl;
    stream << Qt::flush;
    return result;
}

/******************************************************************/
/*!
    \reimp
 */
Qt::ItemFlags XCsvModel::flags(const QModelIndex& index) const
{
    return Qt::ItemIsEditable | QAbstractTableModel::flags(index);
}

/******************************************************************/

void XCsvModel::importFromModel(QAbstractItemModel *model)
{
    beginResetModel();
    m_CsvData.clear();
    m_Header.clear();
    m_MaxColumn = model->columnCount();

    QStringList header;
    for(int i=0; i<m_MaxColumn; ++i) {
        header << model->headerData(i, Qt::Horizontal).toString();
    }
    setHeaderData(header);

    for(int row=0; row<model->rowCount(); ++row) {
        QStringList rowData;
        for(int col=0; col<m_MaxColumn; ++col) {
            rowData << model->data(model->index(row, col)).toString();
        }
        m_CsvData << rowData;
    }

    endResetModel();
}

/******************************************************************/
/*!
 * Returns the current quoting mode.
 * \sa setQuoteMode
 */
XCsvModel::QuoteMode XCsvModel::quoteMode() const
{
    return m_QuoteMode;
}

/******************************************************************/
/*!
 * Sets the current quoting mode. The default quoting mode is BothQuotes | BackslashEscape.
 *
 * The quoting mode determines what kinds of quoting is used for reading and writing CSV files.
 * \sa quoteMode
 * \sa QuoteOption
 */
void XCsvModel::setQuoteMode(QuoteMode mode)
{
    m_QuoteMode = mode;
}

/******************************************************************/
/*!
  Sets the content of the cell at row \a row and column \a column to \a value.
  
  \sa text
  */
void XCsvModel::setText(int row, int column, const QString& value)
{
    setData(index(row, column), value);
}

/******************************************************************/
/*!
  Fetches the content of the cell at row \a row and column \a column.
  
  \sa setText
  */
QString XCsvModel::text(int row, int column) const
{
    return data(index(row, column)).toString();
}

/******************************************************************/
/*!
  Sets the content of the header for column \a column to \a value.
  
  \sa headerText
  */
void XCsvModel::setHeaderText(int column, const QString& value)
{
    setHeaderData(column, Qt::Horizontal, value);
}

/******************************************************************/
/*!
  Fetches the content of the cell at row \a row and column \a column.
  
  \sa setText
  */
QString XCsvModel::headerText(int column) const
{
    return headerData(column, Qt::Horizontal).toString();
}

/******************************************************************/
