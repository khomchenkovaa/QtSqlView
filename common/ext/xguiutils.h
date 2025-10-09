#ifndef XGUIUTILS_H
#define XGUIUTILS_H

#include "xutils.h"

#include <QTextEdit>
#include <QStandardPaths>
#include <QFileDialog>
#include <QImageReader>
#include <QImageWriter>

#include <QComboBox>
#include <QMessageBox>

#include <QMimeDatabase>
#include <QMimeType>

#include <QDebug>

namespace Utils {

/******************************************************************/

inline void initializeImageFileDialog(QFileDialog &dialog, QFileDialog::AcceptMode acceptMode)
{
    static bool firstDialog = true;

    if (firstDialog) {
        firstDialog = false;
        const QStringList picturesLocations = QStandardPaths::standardLocations(QStandardPaths::PicturesLocation);
        dialog.setDirectory(picturesLocations.isEmpty() ? QDir::currentPath() : picturesLocations.last());
    }

    QStringList mimeTypeFilters;
    const QByteArrayList supportedMimeTypes = acceptMode == QFileDialog::AcceptOpen
        ? QImageReader::supportedMimeTypes() : QImageWriter::supportedMimeTypes();
    for (const QByteArray &mimeTypeName : supportedMimeTypes) {
        mimeTypeFilters.append(mimeTypeName);
    }
    mimeTypeFilters.sort();
    dialog.setMimeTypeFilters(mimeTypeFilters);
    dialog.selectMimeTypeFilter("image/jpeg");
    dialog.setAcceptMode(acceptMode);
    if (acceptMode == QFileDialog::AcceptSave)
        dialog.setDefaultSuffix("jpg");
}

/******************************************************************/

inline QIcon iconForMimeType(const QString &mime, const QIcon &fallback = QIcon())
{
    QMimeDatabase db;
    QMimeType t = db.mimeTypeForName(mime);
    if (QIcon::hasThemeIcon(t.iconName())) {
        return QIcon::fromTheme(t.iconName());
    }
    if (QIcon::hasThemeIcon(t.genericIconName())) {
        return QIcon::fromTheme(t.genericIconName());
    }
    return fallback;
}

/******************************************************************/

inline QIcon iconForFile(const QString &fileName, const QIcon &fallback = QIcon())
{
    QMimeDatabase db;
    QMimeType t = db.mimeTypeForFile(fileName, QMimeDatabase::MatchExtension);
    if (QIcon::hasThemeIcon(t.iconName())) {
        return QIcon::fromTheme(t.iconName());
    }
    if (QIcon::hasThemeIcon(t.genericIconName())) {
        return QIcon::fromTheme(t.genericIconName());
    }
    return fallback;
}

/******************************************************************/

inline void findInCmb(QComboBox *cmb, QVariant data)
{
    int idx = cmb->findData(data);
    cmb->setCurrentIndex(idx);
}

/******************************************************************/

inline void fillCmb(QComboBox *cmb, const QStringList& cmbList)
{
    const QSignalBlocker blocker(cmb);
    cmb->clear();
    cmb->addItems(cmbList);
}

/******************************************************************/

inline void fillCmb(QComboBox *cmb, const CmbList& cmbList)
{
    const QSignalBlocker blocker(cmb);
    cmb->clear();
    for (const auto &item : cmbList) {
        cmb->addItem(item.first, item.second);
    }
    cmb->setCurrentIndex(-1);
}

/******************************************************************/

inline void fillCmb(QComboBox *cmb, const CmbExtList& cmbList)
{
    const QSignalBlocker blocker(cmb);
    cmb->clear();
    for (const auto &item : cmbList) {
        if (item.second.isEmpty()) {
            cmb->addItem(item.first);
        } else {
            QVariant itemId = item.second.first();
            cmb->addItem(item.first, itemId);
            int idx = cmb->findData(itemId);
            if (idx != -1) {
                for (int i=0; i<item.second.size(); ++i) {
                    cmb->setItemData(idx, item.second.at(i), Qt::UserRole + i);
                }
            }
        }
    }
    cmb->setCurrentIndex(-1);
}

/******************************************************************/

inline void notImplemented(const QString &sender)
{
    QMessageBox::warning(Q_NULLPTR,
                         "Not implemented",
                         QString("Action '%1' is not implemented yet").arg(sender));
}

/******************************************************************/

class QMessageBoxResize: public QMessageBox
{
public:
    QMessageBoxResize(QWidget *parent = Q_NULLPTR)
        : QMessageBox(parent)
    {
        setMouseTracking(true);
        setSizeGripEnabled(true);
    }
private:
    virtual bool event(QEvent *e) {
        bool res = QMessageBox::event(e);
        switch (e->type()) {
        case QEvent::MouseMove:
        case QEvent::MouseButtonPress:
            setMaximumSize(QWIDGETSIZE_MAX, QWIDGETSIZE_MAX);
            if (QWidget *textEdit = findChild<QTextEdit *>()) {
                textEdit->setMaximumHeight(QWIDGETSIZE_MAX);
            }
        default: break;
        }
        return res;
    }
};

/******************************************************************/

} // namespace Utils

#endif // XGUIUTILS_H
