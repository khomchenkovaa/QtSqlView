#ifndef DBSCHEMAMODEL_H
#define DBSCHEMAMODEL_H

#include <QAbstractTableModel>

#include <QSqlIndex>

class DbTable;

class DbSchemaModel : public QAbstractTableModel
{
    Q_OBJECT

    struct DbSchemaModelPrivate {
        QSqlRecord  sqlRecord;  ///< the record of which the schema is displayed
        QString     driver;     ///< driver name
        QStringList index;      ///< index fields
        QStringList header;
    };

public:
    explicit DbSchemaModel(QObject *parent = nullptr);

    // Header:
    QVariant headerData(int section, Qt::Orientation orientation, int role = Qt::DisplayRole) const override;

    // Basic functionality:
    int rowCount(const QModelIndex &parent = QModelIndex()) const override;
    int columnCount(const QModelIndex &parent = QModelIndex()) const override;

    QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const override;

    void setRecord(DbTable *dbt, QSqlRecord r);

private:
    QVariant dataValue(int idx, int column) const;
    void updateIndexFields(const QSqlIndex &pindex);

private:
    DbSchemaModelPrivate d;
};

#endif // DBSCHEMAMODEL_H
