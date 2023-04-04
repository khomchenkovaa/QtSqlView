#ifndef DBSCHEMAMODEL_H
#define DBSCHEMAMODEL_H

#include <QAbstractTableModel>

#include <QSqlIndex>

class DbTable;

class DbSchemaModel : public QAbstractTableModel
{
    Q_OBJECT

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
    QSqlRecord  m_Record;  ///< the record of which the schema is displayed
    QString     m_Driver;  ///< driver name
    QStringList m_Index;  ///< index fields

    QStringList m_Header;
};

#endif // DBSCHEMAMODEL_H
