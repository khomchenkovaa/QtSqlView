#ifndef REPORTELEMENT_H
#define REPORTELEMENT_H

#include <QtGlobal>
#include <QTextCursor>

namespace Report {

class ReportBuilder;

/**
 * The Report::Element class is the base for all elements.
 *
 * An element defines what is to be added to the report. Calling
 * Report::addElement leads to new content in the report.
 */
class Element
{
public:
    /**
     * @internal
     */
    virtual void build(QTextCursor *cursor) const = 0;

protected:
    /**
     * Constructor - used by subclasses.
     */
    Element() {}
};

}

#endif // REPORTELEMENT_H
