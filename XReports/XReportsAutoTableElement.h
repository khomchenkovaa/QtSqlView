#ifndef XREPORTS_AUTOTABLEELEMENT_H
#define XREPORTS_AUTOTABLEELEMENT_H

#include "XReportsAbstractTableElement.h"

#include <QSize>

QT_BEGIN_NAMESPACE
class QTextDocument;
class QTextTableCell;
class QAbstractItemModel;
QT_END_NAMESPACE

namespace XReports {

/**
 * The XReports::AutoTableElement class represents a table in the report,
 * whose data is provided by a QAbstractItemModel.
 * A header row is added if the QAbstractItemModel has horizontal header data,
 * and a header column is added if the QAbstractItemModel has vertical header data.
 * The header row is repeated on every page if the table is broken across page
 * boundaries.
 */
class AutoTableElement : public AbstractTableElement
{
    struct AutoTableElementPrivate
    {
        QAbstractItemModel *m_tableModel = nullptr;
        QString m_modelKey;
        bool m_verticalHeaderVisible = true;
        bool m_horizontalHeaderVisible = true;
        QBrush m_headerBackground = QColor(218, 218, 218);
        QSize m_iconSize = QSize(32, 32);
    };

public:
    enum Role
    {
        DecorationAlignmentRole = 0x2D535FB1, ///< This model role allows to specify whether the icon should go before the text (Qt::AlignLeft) or after the text (Qt::AlignRight).
        NonBreakableLinesRole   = 0x2D535FB2 ///< This model role allows to specify that line-breaking is not allowed in this table cell. \since 1.7.
    };

    /**
     * Creates a table element from the given table model.
     * Note that the argument isn't const because XReports will call fetchMore() on it,
     * if canFetchMore() returns true.
     */
    explicit AutoTableElement(QAbstractItemModel *tableModel) : AbstractTableElement() {
        d.m_tableModel = tableModel;
    }

    //! Destructor. Deletes internal data.
    ~AutoTableElement() override {}

    /**
     * Copies a table element.
     * The model and settings are copied over.
     */
    AutoTableElement(const AutoTableElement &other)
        : AbstractTableElement(other)
        , d(other.d)
    { }

    //! Copies the model and settings from another table element.
    AutoTableElement &operator=(const AutoTableElement &other) {
        if (&other == this)
            return *this;
        AbstractTableElement::operator=(other);
        d = other.d;
        return *this;
    }

    //! \return the model associated with this element
    QAbstractItemModel *tableModel() const {
        return d.m_tableModel;
    }

    //! set the model associated with this element
    void setTableModel(QAbstractItemModel *tableModel) {
        d.m_tableModel = tableModel;
    }

    /**
     * Sets whether to show a vertical header (showing header data from the model,
     * or row numbers by default).
     * This is true by default, call setVerticalHeaderVisible(false) to hide the vertical header.
     */
    void setVerticalHeaderVisible(bool visible) {
        d.m_verticalHeaderVisible = visible;
    }

    /**
     * Sets whether to show a horizontal header, showing header data from the model.
     * This is true by default, call setHorizontalHeaderVisible(false) to hide the horizontal header.
     */
    void setHorizontalHeaderVisible(bool visible) {
        d.m_horizontalHeaderVisible = visible;
    }

    //! \return true if the vertical header will be visible
    bool isVerticalHeaderVisible() const {
        return d.m_verticalHeaderVisible;
    }

    /**
     * \return true if the horizontal header will be visible
     */
    bool isHorizontalHeaderVisible() const {
        return d.m_horizontalHeaderVisible;
    }

    /**
     * Sets the background color of the headers
     * The default color is gray.
     * Call setHeaderBackground(QBrush()) to disable the background color
     * and have transparent headers instead.
     */
    void setHeaderBackground(const QBrush &brush) {
        d.m_headerBackground = brush;
    }

    //! \return the background color of the headers
    QBrush headerBackground() const {
        return d.m_headerBackground;
    }

    /**
     * Sets the size of the decoration icons, in pixels.
     * This is used when setting the DecorationRole to a QIcon in the model.
     * Note that the model can also set it to a QPixmap or QImage, in which case
     * the size in the report will simply be the size of the pixmap or image.
     */
    void setIconSize(QSize iconSize) {
        d.m_iconSize = iconSize;
    }

    //! \return the size passed to setIconSize
    QSize iconSize() const {
        return d.m_iconSize;
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
        return new AutoTableElement(*this);
    }

private:
    void fillCellFromHeaderData(int section, Qt::Orientation orientation, QTextTableCell &cell, QTextDocument &textDoc, QTextTable *textTable, ReportBuilder &builder) const;
    QSize fillTableCell(int row, int column, QTextTableCell &cell, QTextDocument &textDoc, QTextTable *textTable, ReportBuilder &builder) const;

private:
    AutoTableElementPrivate d;
};

}

#endif /* XREPORTS_AUTOTABLEELEMENT_H */
