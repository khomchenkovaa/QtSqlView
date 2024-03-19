#ifndef XREPORTSHEADERMAP_H
#define XREPORTSHEADERMAP_H

#include "XReportsHeader.h"

namespace XReports {

class HeaderMap : public QMap<HeaderLocations, Header *>
{
public:
    HeaderMap() = default;

    ~HeaderMap() {
        for (iterator it = begin(); it != end(); ++it) {
            delete *it;
        }
    }

    HeaderMap(const HeaderMap &) = delete;

    HeaderMap &operator=(const HeaderMap &) = delete;

    void layoutWithTextWidth(qreal w) {
        for (const_iterator it = constBegin(); it != constEnd(); ++it) {
            it.value()->doc().layoutWithTextWidth(w);
        }
    }

    void updateTextValue(const QString &id, const QString &newValue) {
        for (const_iterator it = constBegin(); it != constEnd(); ++it) {
            it.value()->doc().updateTextValue(id, newValue);
        }
    }

    qreal height() const {
        qreal maxHeight = 0;
        for (const_iterator it = begin(); it != end(); ++it) {
            Header *header = it.value();
            maxHeight = qMax(maxHeight, header->doc().contentDocument().size().height());
        }
        return maxHeight;
    }

    Header *headerForPage(int pageNumber, int pageCount) const {
        Header *firstPageHeader = nullptr;
        Header *lastPageHeader = nullptr;
        Header *evenPagesHeader = nullptr;
        Header *oddPagesHeader = nullptr;
        for (const_iterator it = begin(); it != end(); ++it) {
            const HeaderLocations loc = it.key();
            Header *const h = it.value();
            if (loc & XReports::FirstPage)
                firstPageHeader = h;
            if (loc & XReports::LastPage)
                lastPageHeader = h;
            if (loc & XReports::EvenPages)
                evenPagesHeader = h;
            if (loc & XReports::OddPages)
                oddPagesHeader = h;
        }
        if (pageNumber == 1 && firstPageHeader)
            return firstPageHeader;
        if (pageNumber == pageCount && lastPageHeader)
            return lastPageHeader;
        if (pageNumber & 1) // odd
            return oddPagesHeader;
        else // even
            return evenPagesHeader;
    }

    HeaderLocations headerLocation(Header *header) const {
        for (const_iterator it = begin(); it != end(); ++it) {
            const HeaderLocations loc = it.key();
            Header *const h = it.value();
            if (h == header) {
                return loc;
            }
        }
        HeaderLocations loc;
        return loc;
    }
};

} // namespace XReports

#endif // XREPORTSHEADERMAP_H
