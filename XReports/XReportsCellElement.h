#ifndef XREPORTS_CELLELEMENT_H
#define XREPORTS_CELLELEMENT_H

#include "XReportsElement.h"
#include "XReportsEnums.h"
#include "XReportsElementData.h"
#include "XReportsReportBuilder.h"

#include <QMap>
#include <QPair>

namespace XReports {

/**
 * This class presents a cell in a table.
 * To add an element to a cell, first get hold of the
 * right cell using Table::cell(), then add elements to it.
 */
class CellElement final : public Element
{
    struct CellElementPrivate
    {
        QList<ElementData> elements;
        int columnSpan = 1;
        int rowSpan    = 1;
    };

public:
    /**
     * Set the number of columns that this cell will span.
     * The default is 1.
     */
    void setColumnSpan(int columnSpan) {
        d.columnSpan = columnSpan;
    }

    //! Returns the number of columns that this cell will span.
    int columnSpan() const {
        return d.columnSpan;
    }

    /**
     * Set the number of rows that this cell will span.
     * The default is 1.
     */
    void setRowSpan(int rowSpan) {
        d.rowSpan = rowSpan;
    }

    //! Returns the number of rows that this cell will span.
    int rowSpan() const {
        return d.rowSpan;
    }

    //! Adds an element to the cell, next to the previous element, in the same paragraph.
    void addInlineElement(const Element &element) {
        d.elements.append(XReports::ElementData(element.clone()));
    }

    /**
     * Adds an element to the cell, creating a new paragraph for it.
     * You can specify the alignment of that paragraph.
     */
    void addElement(const Element &element, Qt::AlignmentFlag horizontalAlignment = Qt::AlignLeft) {
        d.elements.append(XReports::ElementData(element.clone(), horizontalAlignment));
    }

    /**
     * Adds an variable in the text of the current paragraph.
     */
    void addVariable(VariableType variable) {
        d.elements.append(XReports::ElementData(variable));
    }

    /**
     * Adds vertical spacing between paragraphs.
     * Make sure to call addElement after that, not addInlineElement.
     * \param space the space in millimeters
     */
    void addVerticalSpacing(qreal space) {
        d.elements.append(XReports::ElementData(XReports::ElementData::VerticalSpacing, space));
    }

    /**
     * @internal
     * @reimp
     */
    void build(ReportBuilder &builder) const override
    {
        foreach (const XReports::ElementData &ed, d.elements) {
            switch (ed.m_type) {
            case XReports::ElementData::Inline:
                builder.addInlineElement(*ed.m_element);
                break;
            case XReports::ElementData::Block:
                builder.addBlockElement(*ed.m_element, ed.m_align);
                break;
            case XReports::ElementData::Variable:
                builder.addVariable(ed.m_variableType);
                break;
            case XReports::ElementData::VerticalSpacing:
                builder.addVerticalSpacing(ed.m_value);
                break;
            }
        }
    }

    /**
     * @internal - not supported
     */
    Element *clone() const override {
        return nullptr;
    }

    /**
     * @internal - do not call (public for QMap)
     */
    ~CellElement() override {}

    /**
     * @internal - do not call (public for QMap)
     */
    CellElement(const CellElement &other) : Element(other), d(other.d) {}

    /**
     * @internal - do not call (public for QMap)
     */
    CellElement &operator=(const CellElement &other) {
        if (&other == this) return *this;
        Element::operator=(other);
        d = other.d;
        return *this;
    }

private:
    friend class TableElement;
    friend class QMap<QPair<int, int>, CellElement>;

    CellElement() : Element() {}

private:
    CellElementPrivate d;
};

}

#endif /* XREPORTS_CELLELEMENT_H */
