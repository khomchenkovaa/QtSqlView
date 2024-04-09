#ifndef XREPORTS_HLINEELEMENT_H
#define XREPORTS_HLINEELEMENT_H

#include "XReportsElement.h"

#include <QString>

namespace XReports {

//! The KDReports::HLineElement class represents an horizontal line in the report.
class HLineElement : public Element
{
    struct HLineElementPrivate
    {
        QColor  color     = Qt::gray;
        qreal   thickness = 2;
        int     margin    = 6;
    };

public:
    //! Creates a hline element.
    HLineElement() : Element() { }

    //! Destroys this hline element.
    ~HLineElement() override {}

    //! Copies a hline element.
    HLineElement(const HLineElement &other) : Element(other), d(other.d) {}

    //! Copies the hline and settings from another hline element.
    HLineElement &operator=(const HLineElement &other) {
        if (&other == this) return *this;
        Element::operator=(other);
        d = other.d;
        return *this;
    }

    //! Set the line color.
    void setColor(const QColor &color) {
        d.color = color;
    }

    //! Returns the hline color
    QColor color() const {
        return d.color;
    }

    //! Sets the line thickness
    void setThickness(qreal t) {
        d.thickness = t;
    }

    //! Returns the hline thickness
    qreal thickness() const {
        return d.thickness;
    }

    //! Sets the margin (empty space) used above and below the line, in mm.
    void setMargin(int m) {
        d.margin = m; // in mm
    }

    //! Returns the margin (empty space) used above and below the line, in mm.
    int margin() const {
        return d.margin;
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
        return new HLineElement(*this);
    }

private:
    HLineElementPrivate d;
};

} // namespace XReports

#endif /* XREPORTS_HLINELEMENT_H */
