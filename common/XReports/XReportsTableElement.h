#ifndef XREPORTS_TABLEELEMENT_H
#define XREPORTS_TABLEELEMENT_H

#include "XReportsAbstractTableElement.h"
#include "XReportsCellElement.h"

namespace XReports {

/**
 * The XReports::TableElement class represents a table in the report.
 * Use cell() to provide the contents for each cell of the table.
 *
 * Use AutoTableElement instead if you have the data in a QAbstractItemModel.
 */
class TableElement : public AbstractTableElement
{
    class CellContentMap : public QMap<QPair<int /*row*/, int /*column*/>, CellElement>
    {
    public:
        CellContentMap() {}
        void getSize(int &rows, int &columns) const
        {
            rows = 0;
            columns = 0;
            for (const_iterator it = begin(); it != end(); ++it) {
                rows = qMax(rows, it.key().first + 1);
                columns = qMax(columns, it.key().second + 1);
            }
        }
    };

    struct TableElementPrivate
    {
        CellContentMap m_cellContentMap;
        int m_headerRowCount = 0;
        int m_headerColumnCount = 0;
    };

public:
    /**
     * Creates an empty table.
     * Text and other content can be added to the table cells using
     * the cell() method.
     * This must be done before the table is added to the report.
     */
    TableElement() : AbstractTableElement() {}

    //! Destructor. Deletes internal data.
    ~TableElement() override {}

    //! Copies a table element.
    TableElement(const TableElement &other) : AbstractTableElement(other), d(other.d) {}

    //! Copies the settings and cell elements from another table element.
    TableElement &operator=(const TableElement &other) {
        if (&other == this) return *this;
        AbstractTableElement::operator=(other);
        d = other.d;
        return *this;
    }

    /**
     * Declares the first @p count rows of the table as table header.
     * The table header rows get repeated when a table is broken across a page boundary.
     * The default number of header rows is 0.
     */
    void setHeaderRowCount(int count) {
        d.m_headerRowCount = count;
    }

    //! \return table header rows count.
    int headerRowCount() const {
        return d.m_headerRowCount;
    }

    /**
     * Declares the first @p count columns of the table as table header.
     * The table header columns get repeated when a table is broken
     * into multiple pages horizontally (see Report::scaleTo).
     * The default number of header rows is 0.
     */
    void setHeaderColumnCount(int count) {
        d.m_headerColumnCount = count;
    }

    //! \return table header columns count.
    int headerColumnCount() const {
        return d.m_headerColumnCount;
    }

    /**
     * Returns the reference to a cell in the table.
     * @param row number, starting from 0
     * @param column number, starting from 0
     */
    CellElement &cell(int row, int column) {
        const QPair<int, int> coord = qMakePair(row, column);
        return d.m_cellContentMap[coord]; // find or create
    }

    /**
     * @internal
     * @reimp
     */
    void build(ReportBuilder &) const override;

    /**
     * @internal
     * @reimp
     */
    Element *clone() const override {
        // never used at the moment
        return new TableElement(*this);
    }

private:
    TableElementPrivate d;
};

} // namespace XReports

#endif /* XREPORTS_TABLEELEMENT_H */
