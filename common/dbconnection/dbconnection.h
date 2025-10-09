#ifndef DBCONNECTION_H
#define DBCONNECTION_H

#include <QSettings>

#include <QSqlDatabase>
#include <QSqlQuery>
#include <QSqlError>

#include <QUuid>

#include <QDebug>

class DbTable;
class DbConnection;
class DbListModel;

typedef QList<DbTable*> tablelist_t;

/******************************************************************/

struct DbParameterSettings {
    const QString LABEL         = "label";
    const QString HOSTNAME      = "hostname";
    const QString PORT          = "port";
    const QString DRIVER        = "driver";
    const QString USERNAME      = "username";
    const QString PASSWORD      = "password";
    const QString ASKPASSWORD   = "askpassword";
    const QString DATABASE      = "database";
    const QString SHOWSYSTABLES = "showsystables";
};

/******************************************************************/

struct DbParameter
{
    QString	label;
    QString	hostname;
    int		port;
    QString	driver;
    QString	username;
    QString	password;
    int		askpassword;
    QString	database;
    int		showsystables;

    void saveToSettings(QSettings &settings) const {
        const DbParameterSettings S;
        settings.setValue(S.LABEL, label);
        settings.setValue(S.HOSTNAME, hostname);
        settings.setValue(S.PORT, port);
        settings.setValue(S.DRIVER, driver);
        settings.setValue(S.USERNAME, username);
        settings.setValue(S.PASSWORD, password);
        settings.setValue(S.ASKPASSWORD, askpassword);
        settings.setValue(S.DATABASE, database);
        settings.setValue(S.SHOWSYSTABLES, showsystables);
    }

    void loadFromSettings(QSettings &settings) {
        const DbParameterSettings S;
        label         = settings.value(S.LABEL).toString();
        hostname      = settings.value(S.HOSTNAME).toString();
        port          = settings.value(S.PORT, 0).toUInt();
        driver        = settings.value(S.DRIVER).toString();
        username      = settings.value(S.USERNAME).toString();
        password      = settings.value(S.PASSWORD).toString();
        askpassword   = settings.value(S.ASKPASSWORD, 0).toUInt();
        database      = settings.value(S.DATABASE).toString();
        showsystables = settings.value(S.SHOWSYSTABLES, 0).toUInt();
    }
};

/******************************************************************/

class DbTable : public QObject
{
    Q_OBJECT

public:
    enum TableType {
        UserTable,
        View,
        SystemTable
    };

    /// link to parent connection object
    DbConnection *dbconn;

    /// guess
    QString	tablename;

    /// 0 = user table, 1 = view, 2 = system table
    TableType tabletype;

    inline DbTable(class DbConnection *dbc, const QString &tab, TableType _tabletype)
	: QObject(), dbconn(dbc), tablename(tab), tabletype(_tabletype)
    {
    }
};

/******************************************************************/

class DbError : public QObject, public QSqlError
{
    Q_OBJECT

public:
    /// link to parent connection object
    DbConnection *dbconn;

    DbError(DbConnection *_dbconn)
	: QObject(), QSqlError(), dbconn(_dbconn)
    {
    }

    DbError(const QSqlError &e)
	: QObject(), QSqlError(e)
    {
    }

    DbError& operator=(const QSqlError &e) {
        *(QSqlError*)this = e;
        return *this;
    }
};

/******************************************************************/

class DbConnection : public QObject
{
    Q_OBJECT

public:
    /// parameters of the connection
    DbParameter	dbparam;

    /// because qt requires us to use addDatabase with a unique id, we use the
    /// only random function aviablable in qt itself here.
    QUuid dbuuid;

    QSqlDatabase db;

    DbError connecterror;

    tablelist_t tablelist;

    DbConnection(const DbParameter &params)
	: QObject(), dbparam(params),
	  dbuuid( QUuid::createUuid() ),
	  connecterror(this)
    {
    }

    QSqlError connect(DbListModel *dblist);

    void disconnect(DbListModel *dblist);

    QStringList tables(QSql::TableType type = QSql::Tables) const {
        return db.tables(type);
    }

    inline int numChildren() const {
        if (db.isOpen())
            return tablelist.size();
        else if (connecterror.isValid())
            return 2;
        else
            return 0;
    }
};

/******************************************************************/

#endif // DBCONNECTION_H
