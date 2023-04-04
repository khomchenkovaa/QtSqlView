#include "dbschemamodel.h"

#include "dbconnection.h"

#include <QSqlField>

#include <QDebug>

/******************************************************************/

enum {
    ConnectionsColumn,
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
    m_Header << tr("Column name")
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
        return m_Header.at(section);
    }
    return QVariant();
}

/******************************************************************/

int DbSchemaModel::rowCount(const QModelIndex &parent) const
{
    if (parent.isValid())
        return 0;

    return m_Record.count();
}

/******************************************************************/

int DbSchemaModel::columnCount(const QModelIndex &parent) const
{
    if (parent.isValid())
        return 0;

    return m_Header.size();
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

void DbSchemaModel::setRecord(DbTable *dbt, QSqlRecord r)
{
    beginResetModel();
    m_Driver = dbt->dbconn->db.driverName();
    m_Record  = r;
    updateIndexFields(dbt->dbconn->db.primaryIndex(dbt->tablename));
    endResetModel();
}

/******************************************************************/

QVariant DbSchemaModel::dataValue(int idx, int column) const
{
    if (idx >= m_Record.count())
        return QVariant();

    QSqlField field = m_Record.field(idx);

    switch (column) {
    case ConnectionsColumn:
        return field.name();
    case TypeColumn:
        return DbTypes::getName(m_Driver, field.typeID());
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
        if (m_Index.contains(field.name())) {
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

void DbSchemaModel::updateIndexFields(const QSqlIndex &pindex)
{
    m_Index.clear();
    for (int i=0; i < pindex.count(); ++i) {
        m_Index << pindex.field(i).name();
    }
}

/******************************************************************/
