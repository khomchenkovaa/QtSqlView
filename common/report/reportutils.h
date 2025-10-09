#ifndef REPORTUTILS_H
#define REPORTUTILS_H

#include "reporttypes.h"

#include <QObject>
#include <QLabel>
#include <QComboBox>
#include <QLineEdit>
#include <QSpinBox>
#include <QDateEdit>

#include <QTextCursor>
#include <QTextTable>
#include <QAbstractItemModel>

#include <QSqlQuery>
#include <QSqlError>

namespace Report {

/******************************************************************/

inline void printTable(QTextCursor *cursor, QAbstractItemModel *model, bool withHeaders=true) {
    if (model == Q_NULLPTR) return;

    cursor->clearSelection();
    cursor->movePosition(QTextCursor::End, QTextCursor::KeepAnchor);

    const int rows = model->rowCount();
    const int cols = model->columnCount();
    const int rowsTotal = withHeaders ? rows+1 : rows;

    QTextTable* table = cursor->insertTable(rowsTotal, cols);
    // print header
    if (withHeaders) {
        for (int j=0; j<cols; ++j) {
            auto pos = table->cellAt(0, j).firstCursorPosition();
            pos.insertHtml("<p align=\"center\"><b>" +
                           model->headerData(j, Qt::Horizontal).toString() +
                           "</b>");
        }
    }
    // print data
    for (int i=0; i<rows; ++i) {
        for (int j=0; j<cols; ++j) {
            const int curRow = withHeaders ? i+1 : i;
            auto pos = table->cellAt(curRow, j).firstCursorPosition();
            QModelIndex idx = model->index(i, j);
            pos.insertHtml(model->data(idx).toString());
        }
    }
}

/******************************************************************/

inline QWidget *createParamEditor(int type, const QString& objectName, QWidget *parent)
{
    QWidget *result = Q_NULLPTR;
    QSizePolicy sizePolicy(QSizePolicy::Maximum, QSizePolicy::Fixed);
    sizePolicy.setHorizontalStretch(0);
    sizePolicy.setVerticalStretch(0);

    switch (type) {
    case Report::ParamType::Null:
        result = new QLabel("NULL", parent);
        break;
    case Report::ParamType::String:
        result = new QLineEdit(parent);
        break;
    case Report::ParamType::Integer:
        result = new QSpinBox(parent);
        qobject_cast<QSpinBox*>(result)->setMaximum(std::numeric_limits<int>::max());
        sizePolicy.setHeightForWidth(result->sizePolicy().hasHeightForWidth());
        result->setSizePolicy(sizePolicy);
        break;
    case Report::ParamType::Real:
        result = new QDoubleSpinBox(parent);
        qobject_cast<QDoubleSpinBox*>(result)->setMaximum(std::numeric_limits<double>::max());
        sizePolicy.setHeightForWidth(result->sizePolicy().hasHeightForWidth());
        result->setSizePolicy(sizePolicy);
        break;
    case Report::ParamType::Date:
        result = new QDateEdit(QDate::currentDate(), parent);
        sizePolicy.setHeightForWidth(result->sizePolicy().hasHeightForWidth());
        result->setSizePolicy(sizePolicy);
        break;
    case Report::ParamType::DateTime:
        result = new QDateTimeEdit(QDateTime::currentDateTime(), parent);
        sizePolicy.setHeightForWidth(result->sizePolicy().hasHeightForWidth());
        result->setSizePolicy(sizePolicy);
        break;
    case Report::ParamType::Ref:
        result = new QComboBox(parent);
        sizePolicy.setHeightForWidth(result->sizePolicy().hasHeightForWidth());
        result->setSizePolicy(sizePolicy);
        break;
    default:
        result = new QWidget(parent);
        break;
    }
    if (!objectName.isEmpty()) {
        result->setObjectName(objectName);
    }
    return result;
}

/******************************************************************/

inline bool fillSqlRef(QComboBox *cmb, const QString &sql, QSqlDatabase db = QSqlDatabase(), QString *err = Q_NULLPTR)
{
    if (!cmb || sql.isEmpty()) return false;

    const QSignalBlocker blocker(cmb);
    cmb->clear();

    QSqlQuery query(sql, db);
    if (query.isSelect()) {
        while (query.next()) {
            cmb->addItem(query.value(1).toString(), query.value(0));
        }
        return true;
    }

    if (query.lastError().type() != QSqlError::NoError && err) {
        err->append(query.lastError().text());
    }
    return false;
}

/******************************************************************/

inline QVariant paramValue(QWidget *editor) {
    if (!editor) return QVariant();

    QComboBox *refCombo = qobject_cast<QComboBox*>(editor);
    if (refCombo) {
        return refCombo->currentData();
    }

    QLineEdit *stringEdit = qobject_cast<QLineEdit*>(editor);
    if (stringEdit) {
        return stringEdit->text();
    }

    QSpinBox *intEdit = qobject_cast<QSpinBox*>(editor);
    if (intEdit) {
        return intEdit->value();
    }

    QDoubleSpinBox *realEdit = qobject_cast<QDoubleSpinBox*>(editor);
    if (realEdit) {
        return realEdit->value();
    }

    QDateEdit *dateEdit = qobject_cast<QDateEdit*>(editor);
    if (dateEdit) {
        return dateEdit->date();
    }

    QDateTimeEdit *dateTimeEdit = qobject_cast<QDateTimeEdit*>(editor);
    if (dateTimeEdit) {
        return dateTimeEdit->dateTime();
    }

    return QVariant();
}

/******************************************************************/

inline void setParamValue(QWidget *editor, QVariant value)
{
    if (!editor) return;

    QComboBox *refCombo = qobject_cast<QComboBox*>(editor);
    if (refCombo) {
        int idx = refCombo->findData(value);
        refCombo->setCurrentIndex(idx);
    }

    QLineEdit *stringEdit = qobject_cast<QLineEdit*>(editor);
    if (stringEdit) {
        stringEdit->setText(value.toString());
    }

    QSpinBox *intEdit = qobject_cast<QSpinBox*>(editor);
    if (intEdit) {
        intEdit->setValue(value.toInt());
    }

    QDoubleSpinBox *realEdit = qobject_cast<QDoubleSpinBox*>(editor);
    if (realEdit) {
        realEdit->setValue(value.toDouble());
    }

    QDateEdit *dateEdit = qobject_cast<QDateEdit*>(editor);
    if (dateEdit) {
        dateEdit->setDate(value.toDate());
    }

    QDateTimeEdit *dateTimeEdit = qobject_cast<QDateTimeEdit*>(editor);
    if (dateTimeEdit) {
        dateTimeEdit->setDateTime(value.toDateTime());
    }
}

/******************************************************************/

} // namespace Report

#endif // REPORTUTILS_H
