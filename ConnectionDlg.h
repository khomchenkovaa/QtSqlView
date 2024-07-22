#ifndef CONNECTIONDLG_H
#define CONNECTIONDLG_H

#include "dbconnection.h"

#include <QDialog>

namespace Ui {
class ConnectionDlg;
}

class ConnectionDlg : public QDialog
{
    Q_OBJECT

public:
    explicit ConnectionDlg(QWidget *parent = nullptr, const DbParameter &dbParameter = DbParameter());
    ~ConnectionDlg();

private slots:
    void updatePasswordStatus();
    void testConnection();
    void chooseSQLiteDbFile();

private:
    void setupConnections();
    void fetchDbParameter();
    void updateFields();

private:
    static QString getDescription(const QString &drv);

public:
    DbParameter	dbp;

private:
    Ui::ConnectionDlg *ui;
};

#endif // CONNECTIONDLG_H
