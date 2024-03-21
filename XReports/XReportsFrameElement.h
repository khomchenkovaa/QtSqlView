/****************************************************************************
**
** This file is part of the KD Reports library.
**
** SPDX-FileCopyrightText: 2007 Klarälvdalens Datakonsult AB, a KDAB Group company <info@kdab.com>
**
** SPDX-License-Identifier: MIT
**
****************************************************************************/

#ifndef XREPORTS_FRAME_H
#define XREPORTS_FRAME_H

#include "XReportsElement.h"
#include "XReportsElementData.h"
#include "XReportsEnums.h"

#include <QMap>
#include <QPair>
#include <QtCore/Qt>

#include <memory>

namespace XReports {
class ReportBuilder;

/**
 * This class represents a frame, with other elements inside.
 * The frame can be visible or not.
 */
class FrameElement : public Element
{
    enum Position
    {
        InFlow, ///< The frame is part of the flow of the paragraph (default)
        FloatLeft, ///< The frame floats on the left side of the text
        FloatRight, ///< The frame floats on the right side of the text
    };

    struct FrameElementPrivate
    {
        QList<ElementData> elements;
        qreal    width      = 0;
        qreal    height     = 0;
        qreal    padding    = 0.5;
        qreal    border     = 1.0;
        Unit     widthUnit  = XReports::Millimeters;
        Unit     heightUnit = XReports::Millimeters;
        Position position   = FrameElement::InFlow;
    };

public:
    //! Creates a frame.
    explicit FrameElement() : Element() {}

    //! Destroys this frame.
    ~FrameElement() override {}

    //! Copies a frame.
    FrameElement(const FrameElement &other) : Element(other), d(other.d) {}

    //! Copies the text and settings from another frame.
    FrameElement &operator=(const FrameElement &other) {
        if (&other == this) return *this;
        Element::operator=(other);
        d = other.d;
        return *this;
    }

    //! Set the frame width in millimeters or in percent of the page width.
    void setWidth(qreal width, Unit unit = Millimeters) {
        d.width = width;
        d.widthUnit = unit;
    }

    //! Set the frame height in millimeters or in percent of the page height.
    void setHeight(qreal height, Unit unit = Millimeters) {
        d.height = height;
        d.heightUnit = unit;
    }

    /**
     * Specifies the size of the padding in millimeters.
     * Padding is the distance between the contents of the frame and the frame border,
     * all around the contents (above, below, on the left and on the right).
     * The default padding has a size of 0.5 mm.
     */
    void setPadding(qreal padding) {
        d.padding = padding;
    }

    //! Returns the width of the frame's internal padding in millimeters.
    qreal padding() const {
        return d.padding;
    }

    /**
     * Specifies the width of the border in pixels.
     * The default border has a width of 1.
     * Set it to 0 for a frame without borders.
     */
    void setBorder(qreal border) {
        d.border = border;
    }

    //! Returns the width of the frame
    qreal border() const {
        return d.border;
    }

    //! Specifies the position of the frame in the document
    void setPosition(Position pos) {
        d.position = pos;
    }

    //! Returns the position of the frame in the document
    Position position() const {
        return d.position;
    }

    //! Adds an element to the frame, next to the previous element, in the same paragraph.
    void addInlineElement(const Element &element) {
        d.elements.append(ElementData(element.clone()));
    }

    /**
     * Adds an element to the frame, creating a new paragraph for it.
     * You can specify the alignment of that paragraph.
     */
    void addElement(const Element &element, Qt::AlignmentFlag horizontalAlignment = Qt::AlignLeft) {
        d.elements.append(ElementData(element.clone(), horizontalAlignment));
    }

    /**
     * Adds an variable in the text of the current paragraph.
     */
    void addVariable(VariableType variable) {
        d.elements.append(XReports::ElementData(variable));
    }

    /**
     * @internal
     * @reimp
     */
    void build(ReportBuilder &builder) const override;

    /**
     * @internal
     * @reimp
     */
    Element *clone() const override {
        return new FrameElement(*this);
    }

private:
    FrameElementPrivate d;
};

}

#endif /* XREPORTS_FRAME_H */
