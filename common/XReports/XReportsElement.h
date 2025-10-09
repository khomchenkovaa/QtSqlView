#ifndef XREPORTS_ELEMENT_H
#define XREPORTS_ELEMENT_H

#include <QBrush>

namespace XReports {

class ReportBuilder;

/**
 * The XReports::Element class is the base for all elements.
 *
 * An element defines what is to be added to the report. Calling
 * Report::addElement or Report::addInlineElement leads to new content
 * in the report. The report doesn't store the element, though.
 * For this reason, any changes to the element after insertion will have no
 * effect on the report. Set all settings in the element before calling addElement.
 */
class Element
{
    struct ElementPrivate {
        QBrush background;
    };

public:
    //! Destroys this element.
    virtual ~Element() {}

    //! Sets the background color for this element.
    void setBackground(const QBrush &brush) {
        d.background = brush;
    }

    //! Returns the background color for this element.
    QBrush background() const {
        return d.background;
    }

    //! @internal
    virtual void build(ReportBuilder &) const = 0;

    //! @internal
    virtual Element *clone() const = 0;

protected:
    //! Constructor - used by subclasses.
    Element() {}

    //! Copy constructor - used by subclasses.
    Element(const Element &other) {
        d = other.d;
    }

    //! Assignment operator - used by subclasses.
    Element &operator=(const Element &other) {
        if (&other == this) return *this;
        d = other.d;
        return *this;
    }

private:
    ElementPrivate d;
};

} // namespace XReports

#endif /* XREPORTS_ELEMENT_H */
