#ifndef XREPORTS_ELEMENTDATA_H
#define XREPORTS_ELEMENTDATA_H

#include "XReportsElement.h"
#include "XReportsEnums.h"

namespace XReports {

/**
 * @internal
 * Storage for Cell and Frame contents
 */
struct ElementData
{
    enum Type {
        Inline,
        Block,
        Variable,
        VerticalSpacing
    };

    // inline
    ElementData(Element *elem)
        : m_element(elem)
        , m_type(Inline)
    {} // m_align not used

    // block
    ElementData(Element *elem, Qt::AlignmentFlag a)
        : m_element(elem)
        , m_type(Block)
        , m_align(a)
    {}

    // inline variable
    ElementData(XReports::VariableType variable)
        : m_element(nullptr)
        , m_type(Variable)
        , m_variableType(variable)
    {}

    // vertical spacing
    ElementData(Type type, qreal value)
        : m_element(nullptr)
        , m_type(type)
        , m_value(value)
    {}

    // copy ctor
    ElementData(const ElementData &other) {
        operator=(other);
    }

    ElementData &operator=(const ElementData &other) {
        m_element = other.m_element ? other.m_element->clone() : nullptr;
        m_type = other.m_type;
        switch (m_type) {
        case Block:
            m_align = other.m_align;
            break;
        case Inline:
            break;
        case Variable:
            m_variableType = other.m_variableType;
            break;
        case VerticalSpacing:
            m_value = other.m_value;
            break;
        }
        return *this;
    }

    ~ElementData() {
        delete m_element;
    }

    Element *m_element;
    Type m_type : 3;
    union {
        XReports::VariableType m_variableType;
        Qt::AlignmentFlag m_align;
        qreal m_value;
    };
};

} // namespace XReports

#endif /* XREPORTS_ELEMENTDATA_H */
