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
    explicit ConnectionDlg(QWidget *parent = nullptr, PDbParam dbParameter = PDbParam::create());
    ~ConnectionDlg();

private slots:
    void updatePasswordStatus();
    void testConnection();
    void chooseSqlDbFile();

private:
    void setupConnections();
    void fetchDbParameter();
    void updateFields(const QString &drv);

private:
    static QString getDriverDescription(const QString &drv);

public:
    PDbParam dbp;

private:
    Ui::ConnectionDlg *ui;
};

#endif // CONNECTIONDLG_H
