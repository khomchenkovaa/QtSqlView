#include "xcombobox.h"

#include <QEvent>
#include <QMouseEvent>
#include <QAbstractItemView>
#include <QLineEdit>
#include <QAction>
#include <QTreeView>
#include <QStandardItemModel>

/******************************************************************/

XComboBox::XComboBox(QWidget *parent)
    : QComboBox(parent)
    , skipNextHide(false)
{
    setupTreeView();
}

/******************************************************************/
/**
 * @brief XComboBox::eventFilter filter tree manipulations
 * @param object
 * @param event
 * @return
 */
bool XComboBox::eventFilter(QObject *object, QEvent *event)
{
    if (event->type() == QEvent::MouseButtonPress) {
        if (object == QComboBox::view()->viewport()) {
            QMouseEvent *mouseEvent = static_cast<QMouseEvent*>(event);
            QModelIndex index = QComboBox::view()->indexAt(mouseEvent->pos());
            if (!QComboBox::view()->visualRect(index).contains(mouseEvent->pos())) {
                skipNextHide = true;
            }
        }
    }
    return false;
}

/******************************************************************/
/**
 * @brief XComboBox::hidePopup prevent hide popup in case of tree manipulation
 */
void XComboBox::hidePopup()
{
    if (skipNextHide) {
        skipNextHide = false;
    } else {
        QComboBox::hidePopup();
    }
}

/******************************************************************/

void XComboBox::setPlaceholderText(const QString &text)
{
    QLineEdit *edit = lineEdit();
    if (edit) {
        edit->setPlaceholderText(text);
    } else {
#if QT_VERSION >= QT_VERSION_CHECK(5, 15, 0)
        QComboBox::setPlaceholderText(text);
#endif
    }
}

/******************************************************************/

QModelIndex XComboBox::findIndexFromData(QVariant data, int column, int role, Qt::MatchFlags flags) const
{
    QModelIndex startIndex = QComboBox::model()->index(0,column);
    QModelIndexList indexList = QComboBox::model()->match(startIndex, role, data, 1, flags);
    QModelIndex index;
    if (!indexList.isEmpty()) {
        index = indexList.first();
    }
    return index;
}

/******************************************************************/

QModelIndex XComboBox::currentModelIndex() const
{
    return view()->currentIndex();
}

/******************************************************************/

QVariant XComboBox::currentModelData() const
{
    QModelIndex index = currentModelIndex();
    return modelDataByIdx(index);
}

/******************************************************************/
/**
 * @brief XComboBox::currentModelData
 * Reimplement this if model is not QStandardItemModel
 * @param index
 * @return data as QVariant
 */
QVariant XComboBox::modelDataByIdx(const QModelIndex& index) const
{
    QStandardItemModel *model = qobject_cast<QStandardItemModel*>(QComboBox::model());
    if (model) {
        QStandardItem *item = model->itemFromIndex(index);
        if (item) {
            return item->data();
        }
    }
    return QVariant();
}

/******************************************************************/

void XComboBox::setCurrentModelIndex(const QModelIndex &index)
{
    QVariant data;
    if (index.isValid()) {
        QComboBox::setRootModelIndex(index.parent());
        QComboBox::setCurrentIndex(index.row());
        QComboBox::setRootModelIndex(QModelIndex());
        data = modelDataByIdx(index);
    } else {
        QComboBox::setCurrentIndex(-1);
    }
    QComboBox::view()->setCurrentIndex(index);
    expandToRoot(index);

    emit currentDataChanged(data);
}

/******************************************************************/

void XComboBox::setNotSelected()
{
    setCurrentModelIndex(QModelIndex());
}

/******************************************************************/

void XComboBox::enableItemsRecoursive(const QModelIndex& index, bool enabled)
{
    QStandardItemModel *model = qobject_cast<QStandardItemModel*>(QComboBox::model());
    if (model) {
        QStandardItem *item = model->itemFromIndex(index);
        if (item) {
            item->setEnabled(enabled);
            if (item->hasChildren()) {
                for (int i=0; i < item->rowCount(); ++i) {
                    enableItemsRecoursive(model->index(i, index.column(), index), enabled);
                }
            }
        }
    }
}

/******************************************************************/

void XComboBox::expand(const QModelIndex &index)
{
    QTreeView *itemView = qobject_cast<QTreeView *>(view());
    if (itemView) {
        itemView->expand(index);
    }
}

/******************************************************************/

void XComboBox::expandToRoot(const QModelIndex &index)
{
    if (!index.isValid()) return;
    QModelIndex parent = index.parent();
    while (parent.isValid()) {
        expand(parent);
        parent = parent.parent();
    }
}

/******************************************************************/

void XComboBox::keyPressEvent(QKeyEvent *event)
{
    if (QComboBox::currentIndex() != -1) {
        if (event->key() == Qt::Key_Backspace) {
            event->accept();
            setNotSelected();
            return;
        }
    }
    QComboBox::keyPressEvent(event);
}

/******************************************************************/
/**
 * @brief XComboBox::setupTreeView setup TreeView as ItemView
 */
void XComboBox::setupTreeView()
{
    QTreeView *itemView = new QTreeView(this);
    itemView->setObjectName("TreeView");
    itemView->setHeaderHidden(true);
    itemView->setAlternatingRowColors(true);
    QComboBox::setView(itemView);
    itemView->viewport()->installEventFilter(this);

    QComboBox::setEditable(true);
    QLineEdit *edit = lineEdit();
    edit->setReadOnly(true);

    QAction *clearAction = edit->addAction(style()->standardIcon(QStyle::SP_LineEditClearButton), QLineEdit::TrailingPosition);
    clearAction->setToolTip(tr("Clear"));

    QObject::connect(edit, &QLineEdit::textChanged, this, [clearAction](const QString &text) {
        clearAction->setDisabled(text.isEmpty());
    });

    QObject::connect(this, QOverload<int>::of(&QComboBox::currentIndexChanged), this, [clearAction](int idx) {
        clearAction->setDisabled(idx == -1);
    });

    QObject::connect(clearAction, &QAction::triggered, this, &XComboBox::setNotSelected);

    QObject::connect(this, &QComboBox::currentTextChanged, [this](){
        QVariant value = currentModelData();
        emit currentDataChanged(value);
    });

}

/******************************************************************/
