#include "dblistmodel.h"

#include <QIcon>

/******************************************************************/

DbListModel::DbListModel(QObject *parent)
    : QAbstractItemModel(parent)
{
    m_Header << tr("Connections");
}

/******************************************************************/

DbListModel::~DbListModel()
{
    clear();
}

/******************************************************************/

QVariant DbListModel::headerData(int section, Qt::Orientation orientation, int role) const
{
    if (orientation == Qt::Horizontal && role == Qt::DisplayRole) {
        return m_Header.at(section);
    }

    if (orientation == Qt::Vertical && role == Qt::DisplayRole) {
        return QString("Row %1").arg(section);
    }

    return QVariant();
}

/******************************************************************/

QModelIndex DbListModel::index(int row, int column, const QModelIndex &parent) const
{
    if (!parent.isValid()) {
        if (row < list.size())
        return createIndex(row, column, list.at(row));
    } else {
        QObject *obj = static_cast<QObject*>(parent.internalPointer());

        if (DbConnection *dbc = qobject_cast<DbConnection*>(obj)) {
            if (dbc->db.isOpen()) {
                if (row < dbc->tablelist.size())
                    return createIndex(row, column, dbc->tablelist.at(row));
            } else if (dbc->connecterror.isValid()) {
                return createIndex(row, column, (void*)&(dbc->connecterror));
            }
        }
    }
    return QModelIndex();
}

/******************************************************************/

QModelIndex DbListModel::parent(const QModelIndex &index) const
{
    if (!index.isValid())
        return QModelIndex();

    QObject *obj = static_cast<QObject*>(index.internalPointer());

    if (qobject_cast<DbConnection*>(obj)) {
        // table entries have the root as parent
        return QModelIndex();
    }
    if (DbTable *dbt = qobject_cast<DbTable*>(obj)) {
        return createIndex(list.indexOf(dbt->dbconn), 0, dbt->dbconn);
    }
    if (DbError *err = qobject_cast<DbError*>(obj)) {
        return createIndex(list.indexOf(err->dbconn), 0, err->dbconn);
    }
    return QModelIndex();
}

/******************************************************************/

int DbListModel::rowCount(const QModelIndex &parent) const
{
    if (!parent.isValid()) {
        return list.size();
    } else {
        QObject *obj = static_cast<QObject*>(parent.internalPointer());
        DbConnection *dbc = qobject_cast<DbConnection*>(obj);
        if (dbc) {
            return dbc->numChildren();
        }
    }

    return 0;
}

/******************************************************************/

int DbListModel::columnCount(const QModelIndex &parent) const
{
    Q_UNUSED(parent)
    return m_Header.size();
}

/******************************************************************/

bool DbListModel::hasChildren(const QModelIndex &parent) const
{
    if (!parent.isValid())
        return true;

    QObject *obj = static_cast<QObject*>(parent.internalPointer());
    DbConnection *dbc = qobject_cast<DbConnection*>(obj);
    return dbc;
}

/******************************************************************/

QVariant DbListModel::data(const QModelIndex &index, int role) const
{
    if (!index.isValid())
        return QVariant();

    QObject *obj = (QObject*)index.internalPointer();

    if (const DbConnection *dbc = qobject_cast<const DbConnection*>(obj)) {
        if (role == Qt::DisplayRole) {
            if (dbc->dbparam.label.isEmpty()) {
                return "<no label>";
            }
            return dbc->dbparam.label;
        } else if (role == Qt::DecorationRole) {
            static QIcon dbicon(":/img/database.png");
            return dbicon;
        }
    } else if (const DbTable *dbt = qobject_cast<const DbTable*>(obj)) {
        if (role == Qt::DisplayRole) {
            return dbt->tablename;
        } else if (role == Qt::DecorationRole) {
            static QIcon tableicon(":/img/table.png");
            static QIcon tablesysicon(":/img/tablesys.png");
            static QIcon viewicon(":/img/view.png");

            if (dbt->tabletype == 0)
                return tableicon;
            else if (dbt->tabletype == 1)
                return viewicon;
            else if (dbt->tabletype == 2)
                return tablesysicon;
        }
    } else if (const DbError *err = qobject_cast<const DbError*>(obj)) {
        if (role == Qt::DisplayRole) {
            if (index.row() == 0)
                return err->driverText();
            else if (index.row() == 1)
                return err->databaseText();
        } else if (role == Qt::DecorationRole) {
            static QIcon erroricon(":/img/error.png");
            return erroricon;
        }
    }

    return QVariant();
}

/******************************************************************/

void DbListModel::clear()
{
    if (list.isEmpty()) return;

    for (DbConnection *dbc : list) {
        dbc->disconnect(this);
    }

    beginResetModel();
    qDeleteAll(list);
    list.clear();
    endResetModel();
}

/******************************************************************/

void DbListModel::addDbConnection(const DbParameter &dbp)
{
    beginInsertRows(QModelIndex(), list.size(), list.size());
    list << new DbConnection(dbp);
    endInsertRows();
}

/******************************************************************/

void DbListModel::editDbConnection(int num, DbParameter &dbp)
{
    if (0 > num || num >= list.size()) return;

    // close old connection and change parameters
    list[num]->disconnect(this);
    list[num]->dbparam = dbp;

    emit dataChanged(createIndex(num, 0, list[num]),
                     createIndex(num, 0, list[num]));
}

/******************************************************************/

void DbListModel::delDbConnection(int num)
{
    if (0 > num || num >= list.size()) return;

    beginRemoveRows(QModelIndex(), num, num);

    list[num]->disconnect(this);
    delete list[num];

    list.removeAt(num);

    endRemoveRows();
}

/******************************************************************/

void DbListModel::saveToSettings()
{
    QSettings settings;

    settings.beginWriteArray("connections");
    int i = 0;
    for(auto dbc : list) {
        settings.setArrayIndex(i++);
        dbc->dbparam.saveToSettings(settings);
    }
    settings.endArray();
}

/******************************************************************/

void DbListModel::loadFromSettings()
{
    QSettings settings;

    clear();

    int connnum = settings.beginReadArray("connections");
    for (int i = 0; i < connnum; ++i) {
        settings.setArrayIndex(i);

        DbParameter dbp;
        dbp.loadFromSettings(settings);
        addDbConnection(dbp);
    }
    settings.endArray();
}

/******************************************************************/

int DbListModel::getDbConnectionNum(const QModelIndex &index) const
{
    if (!index.isValid()) return -1;

    QObject *obj = (QObject*)index.internalPointer();

    if (DbConnection *dbc = qobject_cast<DbConnection*>(obj)) {
        return list.indexOf(dbc);
    } else if (const DbTable *dbt = qobject_cast<const DbTable*>(obj)) {
        return list.indexOf(dbt->dbconn);
    }
    return 0;
}

/******************************************************************/

DbConnection *DbListModel::getDbConnection(const QModelIndex &index) const
{
    if (!index.isValid())
        return Q_NULLPTR;

    QObject *obj = (QObject*)index.internalPointer();

    if (DbConnection *dbc = qobject_cast<DbConnection*>(obj)) {
        return dbc;
    } else if (DbTable *dbt = qobject_cast<DbTable*>(obj)) {
        return dbt->dbconn;
    }
    return Q_NULLPTR;
}

/******************************************************************/

DbTable *DbListModel::getDbTable(const QModelIndex &index) const
{
    if (!index.isValid())
        return Q_NULLPTR;

    QObject *obj = (QObject*)index.internalPointer();

    if (DbTable *dbt = qobject_cast<DbTable*>(obj)) {
        return dbt;
    }
    return Q_NULLPTR;
}

/******************************************************************/

void DbListModel::expanding(const QModelIndex &index)
{
    if (!index.isValid()) return;

    QObject *obj = (QObject*)index.internalPointer();

    if (DbConnection *dbc = qobject_cast<DbConnection*>(obj)) {
        if (!dbc->db.isOpen()) {
            dbc->connect(this);
        }
    }
}

/******************************************************************/

void DbListModel::collapsed(const QModelIndex &index)
{
    if (!index.isValid()) return;

    QObject *obj = (QObject*)index.internalPointer();

    if (DbConnection *dbc = qobject_cast<DbConnection*>(obj)) {
        if (dbc->db.isOpen()) {
            dbc->disconnect(this);
        }
    }
}

/******************************************************************/

void DbListModel::refresh()
{
    for (auto dbc : list) {
        if (dbc->db.isOpen()) {
            tablelist_clear(*dbc);
            tablelist_load(*dbc);
        }
    }
}

/******************************************************************/

void DbListModel::tablelist_clear(DbConnection &dbc)
{
    if (!dbc.tablelist.isEmpty()) {
        beginRemoveRows(createIndex(list.indexOf(&dbc), 0, &dbc), 0, dbc.tablelist.size()-1);
        qDeleteAll(dbc.tablelist);
        dbc.tablelist.clear();
        endRemoveRows();
    }
    if (dbc.connecterror.isValid()) {
        beginRemoveRows(createIndex(list.indexOf(&dbc), 0, &dbc), 0, 1);
        dbc.connecterror = QSqlError();
        endRemoveRows();
    }
}

/******************************************************************/

void DbListModel::tablelist_load(DbConnection &dbc)
{
    tablelist_clear(dbc);

    if (!dbc.db.isOpen()) return;

    QList<DbTable*> newtablelist;

    for (QString table : dbc.db.tables()) {
        newtablelist << new DbTable(&dbc, table, DbTable::UserTable);
    }

    for (QString table : dbc.db.tables(QSql::Views)) {
        newtablelist << new DbTable(&dbc, table, DbTable::View);
    }

    if (dbc.dbparam.showsystables) {
        for (QString table : dbc.db.tables(QSql::SystemTables)) {
            newtablelist << new DbTable(&dbc, table, DbTable::SystemTable);
        }
    }

    beginInsertRows(createIndex(list.indexOf(&dbc), 0, &dbc), 0, newtablelist.size()-1);
    dbc.tablelist = newtablelist;
    endInsertRows();
}

/******************************************************************/

void DbListModel::tablelist_seterror(DbConnection &dbc, QSqlError e)
{
    tablelist_clear(dbc);

    beginInsertRows(createIndex(list.indexOf(&dbc), 0, &dbc), 0, 1);

    dbc.connecterror = e;

    endInsertRows();
}

/******************************************************************/
