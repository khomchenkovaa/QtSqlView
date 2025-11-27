#include "dbconnection.h"

#include "dblistmodel.h"

#include <QInputDialog>

/******************************************************************/

QSqlError DbConnection::connect(DbListModel *dblist)
{
    assert(!dbuuid.isNull());

    if (!QSqlDatabase::isDriverAvailable(dbparam->driver())) {
        QSqlError e = QSqlError("Could not connect to database",
                    QString("Database driver %1 is not available.").arg(dbparam->driver()),
                    QSqlError::ConnectionError);
        dblist->tablelist_seterror(*this, e);
        return e;
    }

    if (QSqlDatabase::contains(dbuuid.toString())) {
        db = QSqlDatabase::database(dbuuid.toString());
    } else {
        db = QSqlDatabase::addDatabase(dbparam->driver(), dbuuid.toString());
    }

    if (!dbparam->hostname().isEmpty()) db.setHostName(dbparam->hostname());
    if (dbparam->port() > 0) db.setPort(dbparam->port());
    db.setDatabaseName(dbparam->database());
    if (!dbparam->username().isEmpty()) db.setUserName(dbparam->username());
    if (!dbparam->options().isEmpty()) db.setConnectOptions(dbparam->options());

    if (dbparam->connAskPassword) {
        bool ok;
        QString passwd = QInputDialog::getText(NULL, "QtSqlView Password Prompt",
                               QString("Enter password for '%1':").arg(dbparam->connLabel),
                               QLineEdit::Password, QString(), &ok);

        if (!ok) {
            QSqlError e = QSqlError("Could not connect to database",
                        "Password prompt failed.",
                        QSqlError::ConnectionError);
            dblist->tablelist_seterror(*this, e);
            return e;
        }
        db.setPassword(passwd);
    } else {
        if (!dbparam->password().isEmpty()) db.setPassword(dbparam->password());
    }

    if (!db.open()) {
        QSqlError e = db.lastError();
        db = QSqlDatabase();
        QSqlDatabase::removeDatabase(dbuuid.toString());
        dblist->tablelist_seterror(*this, e);
        return e;
    }

    if (!dblist->tablelist_load(*this)) {
        QSqlError e = QSqlError("Could not load tables",
                    "Load table list failed.",
                    QSqlError::ConnectionError);
        dblist->tablelist_seterror(*this, e);
        return e;
    }

    return QSqlError();
}

/******************************************************************/

void DbConnection::disconnect(DbListModel *dblist)
{
    if (db.isOpen()) {
        db.close();
        db = QSqlDatabase();
        QSqlDatabase::removeDatabase(dbuuid.toString());
    }
    dblist->tablelist_clear(*this);
}

/******************************************************************/


