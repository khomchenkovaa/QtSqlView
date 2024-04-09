#ifndef XREPORTS_IMAGEELEMENT_H
#define XREPORTS_IMAGEELEMENT_H

#include "XReportsElement.h"
#include "XReportsEnums.h"

#include <QPixmap>
#include <QImage>

namespace XReports {

/**
 * The XReports::ImageElement class represents an image in the report.
 */
class ImageElement : public Element
{
    struct ImageElementPrivate
    {
        QVariant pixmap; // pixmap or image, can't use QPixmap directly in threads
        QSize    pixmapSize; // = m_pixmap.size()

        // size in final document, chosen by the user:
        qreal   width = 0;
        qreal   height = 0;
        bool    fitToPage = false;
        Unit    unit = XReports::Millimeters;
    };

public:

    /**
     * Creates a image element from the given pixmap.
     * This constructor can only be used in the GUI thread.
     */
    explicit ImageElement(const QPixmap &pixmap) : Element() {
        setPixmap(pixmap);
    }

    /**
     * Creates a image element from the given image.
     * This constructor can be used from secondary threads as well.
     */
    explicit ImageElement(const QImage &image) : Element() {
        setImage(image);
    }

    //! Copies a image element.
    ImageElement(const ImageElement &other) : Element(other), d(other.d) {}

    //! Copies the data from another image element.
    ImageElement &operator=(const ImageElement &other) {
        if (&other == this) return *this;
        Element::operator=(other);
        d = other.d;
        return *this;
    }

    //! Destroys this image element.
    ~ImageElement() override {}

    /**
     * Sets the pixmap for this image element.
     * This replaces the pixmap or image set previously.
     */
    void setPixmap(const QPixmap &pixmap) {
        // the call to toImage() is a workaround for a bug in QTextOdfWriter
        // https://codereview.qt-project.org/c/qt/qtbase/+/369642
        d.pixmap     = QVariant::fromValue(pixmap.toImage());
        d.pixmapSize = pixmap.size();
    }

    //! \return the pixmap.
    QPixmap pixmap() const {
        return d.pixmap.value<QPixmap>();
    }

    /**
     * Sets a new image for this image element.
     * This replaces the pixmap or image set previously.
     */
    void setImage(const QImage &image) {
        d.pixmap     = QVariant::fromValue(image);
        d.pixmapSize = image.size();
    }

    //! \return the image.
    QImage image() const {
        return d.pixmap.value<QImage>();
    }

    /**
     * Sets the width of the image.
     * The height of the image is calculated so that the aspect ratio is preserved.
     *
     * The default size, if %setWidth is not called, is the pixmap's size in pixels,
     * which might lead to a different size depending on the resolution of the
     * output device but avoids any resizing.
     * On the other hand, setWidth makes sure that the layout of the document
     * is always the same by resizing the pixmap if necessary.
     *
     * For an image that should take half of the width of the page, call setWidth(50, XReports::Percent).
     *
     * Note that images inside table cells still have their width relative to the page, not to the cell.
     * For instance setWidth(10, XReports::Percent) means that the image width will be 10% of the page width,
     * not 10% of the containing cell's width.
     */
    void setWidth(qreal width, Unit unit = Unit::Millimeters) {
        d.width = width;
        d.unit = unit;
        d.height = 0; // mutually exclusive!
        d.fitToPage = false; // "
    }

    //! \return the width of the image.
    qreal width() const {
        return d.width;
    }

    /**
     * Sets the height of the image.
     * The width of the image is calculated so that the aspect ratio is preserved.
     * This is mutually exclusive with setWidth().
     */
    void setHeight(qreal height, Unit unit = Unit::Millimeters) {
        d.height = height;
        d.unit = unit;
        d.width = 0; // mutually exclusive!
        d.fitToPage = false; // "
    }

    //! \return the height of the image.
    qreal height() const {
        return d.height;
    }

    //! \return the unit for the image size (percent or millimeters).
    Unit unit() const {
        return d.unit;
    }

    //! Set the unit for the image size (percent or millimeters).
    void setUnit(Unit unit) {
        d.unit = unit;
    }

    /**
     * Requests that the image be scaled so that it fills the page as much as
     * possible, while still preserving aspect ratio.
     * This is mutually exclusive with setHeight/setWidth.
     */
    void setFitToPage() {
        d.width = 0; // mutually exclusive!
        d.height = 0; // mutually exclusive!
        d.fitToPage = true;
    }

    //! \return if the image be scaled to fills the page.
    bool fitToPage() const {
        return d.fitToPage;
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
        return new ImageElement(*this);
    }

private:
    ImageElementPrivate d;
};

}

#endif /* XREPORTS_IMAGEELEMENT_H */
