#ifndef DBCONNECTION_H
#define DBCONNECTION_H

#include "dbtypes.h"

#include <QSettings>

#include <QSqlDatabase>
#include <QSqlError>

#include <QUuid>

#define S_LABEL         "label"
#define S_HOSTNAME      "hostname"
#define S_PORT          "port"
#define S_DRIVER        "driver"
#define S_USERNAME      "username"
#define S_PASSWORD      "password"
#define S_ASKPASSWORD   "askpassword"
#define S_DATABASE      "database"
#define S_SHOWSYSTABLES "showsystables"

class DbTable;
class DbConnection;
class DbListModel;

typedef QList<DbTable*> tablelist_t;

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
        settings.setValue(S_LABEL, label);
        settings.setValue(S_HOSTNAME, hostname);
        settings.setValue(S_PORT, port);
        settings.setValue(S_DRIVER, driver);
        settings.setValue(S_USERNAME, username);
        settings.setValue(S_PASSWORD, password);
        settings.setValue(S_ASKPASSWORD, askpassword);
        settings.setValue(S_DATABASE, database);
        settings.setValue(S_SHOWSYSTABLES, showsystables);
    }

    void loadFromSettings(QSettings &settings) {
        label         = settings.value(S_LABEL).toString();
        hostname      = settings.value(S_HOSTNAME).toString();
        port          = settings.value(S_PORT, 0).toUInt();
        driver        = settings.value(S_DRIVER).toString();
        username      = settings.value(S_USERNAME).toString();
        password      = settings.value(S_PASSWORD).toString();
        askpassword   = settings.value(S_ASKPASSWORD, 0).toUInt();
        database      = settings.value(S_DATABASE).toString();
        showsystables = settings.value(S_SHOWSYSTABLES, 0).toUInt();
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
    class DbConnection *dbconn;

    DbError(class DbConnection *_dbconn)
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
    DbParameter		dbparam;

    /// because qt requires us to use addDatabase with a unique id, we use the
    /// only random function aviablable in qt itself here.
    QUuid		dbuuid;

    QSqlDatabase	db;

    DbError		connecterror;

    tablelist_t tablelist;

    DbConnection(const DbParameter &params)
	: QObject(), dbparam(params),
	  dbuuid( QUuid::createUuid() ),
	  connecterror(this)
    {
    }

    QSqlError connect(DbListModel *dblist);

    void disconnect(DbListModel *dblist);

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
