#ifndef REPORTTABLEELEMENT_H
#define REPORTTABLEELEMENT_H

#include "reportelement.h"
#include "reportutils.h"
#include "xcsvmodel.h"

namespace Report {

/**
 * The Report::TableElement class represents table in the report.
 * This is one way to insert table into the report (the other way is with HtmlElement).
 */
class TableElement : public Element
{
public:
    /**
     * Creates an HTML report element.
     */
    explicit TableElement(QAbstractItemModel *model = nullptr, bool withHeaders = true) : Element() {
        m_Model       = model;
        b_WithHeaders = withHeaders;
    }

    /**
     * Sets the model in this element.
     */
    void setModel(QAbstractItemModel *model = nullptr, bool withHeaders = true) {
        m_Model       = model;
        b_WithHeaders = withHeaders;
    }

    /**
     * \return the model in this element.
     */
    QAbstractItemModel *model() const {
        return m_Model;
    }

    /**
     * @internal
     * @reimp
     */
    void build(QTextCursor *cursor) const override {
        Report::printTable(cursor, m_Model, b_WithHeaders);
    }

    bool toCsvFile(const QString& fileName) const {
        if (fileName.isEmpty() || m_Model == Q_NULLPTR) return false;

        XCsvModel csvModel;
        csvModel.importFromModel(m_Model);
        csvModel.toCSV(fileName, b_WithHeaders);
        return true;
    }

private:
    QAbstractItemModel *m_Model;
    bool b_WithHeaders;

};

} // namespace Report

#endif // REPORTTABLEELEMENT_H
