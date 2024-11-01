#include "dbschemamodel.h"

#include "dbconnection.h"

#include <QSqlField>

#include <QDebug>

/******************************************************************/

enum {
    NameColumn,
    TypeColumn,
    LengthColumn,
    NullableColumn,
    ModifiersColumn,
    DefaultColumn
};

/******************************************************************/

DbSchemaModel::DbSchemaModel(QObject *parent)
    : QAbstractTableModel(parent)
{
    d.header << tr("Column name")
             << tr("Data Type")
             << tr("Length")
             << tr("Nullable")
             << tr("Modifiers")
             << tr("Default");
}

/******************************************************************/

QVariant DbSchemaModel::headerData(int section, Qt::Orientation orientation, int role) const
{
    if (orientation == Qt::Horizontal && role == Qt::DisplayRole) {
        return d.header.at(section);
    }
    return QVariant();
}

/******************************************************************/

int DbSchemaModel::rowCount(const QModelIndex &parent) const
{
    if (parent.isValid())
        return 0;

    return d.sqlRecord.count();
}

/******************************************************************/

int DbSchemaModel::columnCount(const QModelIndex &parent) const
{
    if (parent.isValid())
        return 0;

    return d.header.size();
}

/******************************************************************/

QVariant DbSchemaModel::data(const QModelIndex &index, int role) const
{
    if (!index.isValid())
        return QVariant();


    switch (role) {
    case Qt::DisplayRole:
        return dataValue(index.row(), index.column());
    }

    return QVariant();
}

/******************************************************************/

void DbSchemaModel::setRecord(const QString &driver, const QSqlRecord &rec, const QSqlIndex &idx)
{
    beginResetModel();
    d.driver = driver;
    d.sqlRecord  = rec;
    d.index.clear();
    for (int i=0; i < idx.count(); ++i) {
        d.index << idx.field(i).name();
    }
    endResetModel();
}

/******************************************************************/

QVariant DbSchemaModel::dataValue(int idx, int column) const
{
    if (idx >= d.sqlRecord.count())
        return QVariant();

    QSqlField field = d.sqlRecord.field(idx);

    switch (column) {
    case NameColumn:
        return field.name();
    case TypeColumn:
        return DbTypes::getName(d.driver, field.typeID());
    case LengthColumn: {
        int length = field.length();
        int precision = field.precision();
        if (length > 0) {
            if (precision > 0) {
                return QString("(%1:%2)").arg(length).arg(precision);
            } else {
                return QString("(%1)").arg(length);
            }
        }
        break;
    }
    case NullableColumn: {
        switch (field.requiredStatus()) {
        case QSqlField::Required :
            return "No";
        case QSqlField::Optional :
            return "Yes";
        case QSqlField::Unknown :
            return "Unknown";
        }
        break;
    }
    case ModifiersColumn: {
        QStringList mods;
        if (d.index.contains(field.name())) {
            mods << "PRIMARY KEY";
        }
        if (field.isAutoValue()) {
            mods << "AUTO_INCREMENT";
        }
        return mods.join(" ");
    }
    case DefaultColumn:
        return field.defaultValue();
    }

    return QVariant();
}

/******************************************************************/
