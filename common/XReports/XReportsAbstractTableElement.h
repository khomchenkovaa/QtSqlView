#ifndef XREPORTS_ABSTRACTTABLEELEMENT_H
#define XREPORTS_ABSTRACTTABLEELEMENT_H

#include "XReportsElement.h"
#include "XReportsEnums.h"
#include "XReportsUtils.h"

namespace XReports {

/**
 * Base class for TableElement and AutoTableElement.
 */
class AbstractTableElement : public Element
{
    struct ColumnConstraint
    {
        qreal width;
        Unit unit;
    };

    struct AbstractTableElementPrivate
    {
        qreal  border      = 1.0;
        QBrush borderBrush = QBrush(Qt::darkGray /*like Qt does*/);
        qreal  padding     = 0.5;
        qreal  width       = 0;
        Unit   unit        = XReports::Millimeters;
        QFont  defaultFont;
        bool   fontSpecified = false;
        std::vector<XReports::AbstractTableElement::ColumnConstraint> constraints;
    };

public:
    /**
     * Specifies the width of the border in pixels.
     * The default border has a width of 1.
     * Set it to 0 for a table without borders.
     */
    void setBorder(qreal border) {
        d.border = border;
    }

    //! Returns the width of the table border
    qreal border() const {
        return d.border;
    }

    //! Specifies the color (brush) of the border.
    void setBorderBrush(const QBrush &brush) {
        d.borderBrush = brush;
    }

    //! Returns the color (brush) of the table border
    QBrush borderBrush() const {
        return d.borderBrush;
    }

    /**
     * Specifies the size of the padding in millimeters.
     * Padding is the distance between the contents of the cell and the cell border,
     * all around the contents (above, below, on the left and on the right).
     * The default padding has a size of 0.5 mm.
     */
    void setPadding(qreal padding) {
        d.padding = padding;
    }

    //! Returns the width of the cell's internal padding in millimeters.
    qreal padding() const {
        return d.padding;
    }

    /**
     * Sets the width of the table, in millimeters or as a percentage of the page width.
     * For instance, for a table that should stretch to the full width of the page, call setWidth(100, KDReports::Percent).
     * This is only available in WordProcessing mode.
     */
    void setWidth(qreal width, Unit unit = Millimeters) {
        d.width = width;
        d.unit = unit;
    }

    /**
     * \return the width of the table, in millimeters or as a percentage of the page width.
     * \see unit
     */
    qreal width() const {
        return d.width;
    }

    //! \return the width unit of the table.
    Unit unit() const {
        return d.unit;
    }

    /**
     * Sets the default font used for text in this table.
     * In WordProcessing mode, per-cell fonts can override this.
     */
    void setDefaultFont(const QFont &font) {
        d.defaultFont = font;
        d.fontSpecified = true;
    }

    /**
     * \return the default font set by setDefaultFont(), if any
     * \param isSet set to true if a default font was set, false otherwise
     */
    QFont defaultFont(bool *isSet) const {
        *isSet = d.fontSpecified;
        return d.defaultFont;
    }

    /**
     * Sets constraints on the column widths (fixed or proportional)
     * \param constraints vector of constraints
     */
    void setColumnConstraints(const std::vector<ColumnConstraint> &constraints) {
        d.constraints = constraints;
    }

protected:
    //! Protected constructor, for derived classes only.
    AbstractTableElement() : Element() {}

    /**
     * Copies a table element.
     * The model and settings (and cell elements, when using addElement) are copied over.
     */
    AbstractTableElement(const AbstractTableElement &other) : Element(), d(other.d) {}

    //! Copies the table and settings from another table element.
    AbstractTableElement &operator=(const AbstractTableElement &other) {
        if (&other == this)
            return *this;
        Element::operator=(other);
        d = other.d;
        return *this;
    }

    //! Destructor. Deletes internal data.
    ~AbstractTableElement() override {}

    /**
     * @internal for derived classes only.
     */
    void fillTableFormat(QTextTableFormat &tableFormat, QTextCursor &textDocCursor) const {
        if (d.width) {
            if (d.unit == Millimeters) {
                tableFormat.setWidth(QTextLength(QTextLength::FixedLength, mmToPixels(d.width)));
            } else {
                tableFormat.setWidth(QTextLength(QTextLength::PercentageLength, d.width));
            }
        }

        if (!d.constraints.empty()) {
            QVector<QTextLength> constraints;
            constraints.reserve(d.constraints.size());
            for (const auto &c : qAsConst(d.constraints)) {
                QTextLength length;
                if (c.unit == Millimeters) {
                    length = QTextLength(QTextLength::FixedLength, mmToPixels(c.width));
                } else {
                    length = QTextLength(QTextLength::PercentageLength, c.width);
                }
                constraints.append(length);
            }
            tableFormat.setColumnWidthConstraints(constraints);
        }

        tableFormat.setBorder(border());
        tableFormat.setBorderBrush(borderBrush());
        tableFormat.setCellPadding(mmToPixels(padding()));
        tableFormat.setCellSpacing(0); // HTML-like table borders look so old century
        if (d.fontSpecified) {
            QTextCharFormat charFormat = textDocCursor.charFormat();
            charFormat.setFont(d.defaultFont);
            textDocCursor.setCharFormat(charFormat);
        }
    }

private:
    AbstractTableElementPrivate d;
};

} // namespace XReports

#endif /* XREPORTS_ABSTRACTTABLEELEMENT_H */
