#ifndef DBCONNECTION_H
#define DBCONNECTION_H

#include <QSettings>

#ifdef USE_QUERY_DB

#include "t_rep_connection.h"
typedef Db::Query::PQueConnection PDbParam;

#else

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

struct DbParameter
{
    QString	connLabel;         ///< Connection's name
    QString	connHostname;      ///< Hostname
    int		connPort;          ///< Port
    QString	connDriver;        ///< Driver name
    QString	connUsername;      ///< User name
    QString	connPassword;      ///< Password
    int		connAskPassword;   ///< Ask password on connect
    QString	connDatabase;      ///< Database
    int		connShowSystables; ///< Show system tables flag

    void saveToSettings(QSettings &settings) const {
        const DbParameterSettings S;
        settings.setValue(S.LABEL, connLabel);
        settings.setValue(S.HOSTNAME, connHostname);
        settings.setValue(S.PORT, connPort);
        settings.setValue(S.DRIVER, connDriver);
        settings.setValue(S.USERNAME, connUsername);
        settings.setValue(S.PASSWORD, connPassword);
        settings.setValue(S.ASKPASSWORD, connAskPassword);
        settings.setValue(S.DATABASE, connDatabase);
        settings.setValue(S.SHOWSYSTABLES, connShowSystables);
    }

    void loadFromSettings(QSettings &settings) {
        const DbParameterSettings S;
        connLabel         = settings.value(S.LABEL).toString();
        connHostname      = settings.value(S.HOSTNAME).toString();
        connPort          = settings.value(S.PORT, 0).toUInt();
        connDriver        = settings.value(S.DRIVER).toString();
        connUsername      = settings.value(S.USERNAME).toString();
        connPassword      = settings.value(S.PASSWORD).toString();
        connAskPassword   = settings.value(S.ASKPASSWORD, 0).toUInt();
        connDatabase      = settings.value(S.DATABASE).toString();
        connShowSystables = settings.value(S.SHOWSYSTABLES, 0).toUInt();
    }

    QString driver() const {
        return connDriver;
    }

    QString hostname() const {
        return connHostname;
    }

    int port() const {
        return connPort;
    }

    QString username() const {
        return connUsername;
    }

    QString password() const {
        return connPassword;
    }

    QString database() const {
        return connDatabase;
    }
};

typedef QSharedPointer<DbParameter> PDbParam;

#endif

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
    PDbParam dbparam;

    /// because qt requires us to use addDatabase with a unique id, we use the
    /// only random function aviablable in qt itself here.
    QUuid dbuuid;

    QSqlDatabase db;

    DbError connecterror;

    tablelist_t tablelist;

    DbConnection(PDbParam params)
	: QObject(), dbparam(params),
	  dbuuid( QUuid::createUuid() ),
	  connecterror(this)
    {
    }

    DbConnection(PDbParam params, QUuid uuid)
    : QObject(), dbparam(params),
      dbuuid(uuid),
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
