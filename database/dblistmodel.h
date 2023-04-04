#ifndef DBLISTMODEL_H
#define DBLISTMODEL_H

#include "dbconnection.h"

#include <QAbstractItemModel>

class DbConnection;

class DbListModel : public QAbstractItemModel
{
    Q_OBJECT

public:
    explicit DbListModel(QObject *parent = nullptr);
    ~DbListModel();

    // Header:
    QVariant headerData(int section, Qt::Orientation orientation, int role = Qt::DisplayRole) const override;

    // Basic functionality:
    QModelIndex index(int row, int column,
                      const QModelIndex &parent = QModelIndex()) const override;
    QModelIndex parent(const QModelIndex &index) const override;

    int rowCount(const QModelIndex &parent = QModelIndex()) const override;
    int columnCount(const QModelIndex &parent = QModelIndex()) const override;
    bool hasChildren(const QModelIndex& parent = QModelIndex()) const override;

    QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const override;

    /// correctly delete all dbconnections
    void clear();

    /// add a new dbconnection
    void addDbConnection(const DbParameter &dbp);

    /// change a dbconnection
    void editDbConnection(int num, DbParameter &dbp);

    /// remove a dbconnection
    void delDbConnection(int num);

    /// save all database parameters to settings
    void saveToSettings();

    /// load all database parameters from settings
    void loadFromSettings();

    int getDbConnectionNum(const QModelIndex &index) const;

    DbConnection *getDbConnection(const QModelIndex &index) const;

    DbTable *getDbTable(const QModelIndex &index) const;

    void expanding(const QModelIndex &index);

    void collapsed(const QModelIndex &index);

    void refresh();

    void tablelist_clear(class DbConnection &dbc);

    void tablelist_load(class DbConnection &dbc);

    void tablelist_seterror(class DbConnection &dbc, QSqlError e);

public:
    QList<DbConnection*> list; ///< the primary list of data connections

private:
    QStringList m_Header;
};

#endif // DBLISTMODEL_H
