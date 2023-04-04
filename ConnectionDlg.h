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

    void fetchDbParameter();
    void updatePasswordStatus();
    void updateFields();

public slots:
    void on_okButton_clicked();
    void on_testButton_clicked();
    void on_checkAskPassword_clicked();
    void on_comboType_currentIndexChanged(int index);
    void on_buttonSelectFile_clicked();

private:
    static QString getDescription(QString drv);

public:
    DbParameter	dbp;

private:
    Ui::ConnectionDlg *ui;
};

#endif // CONNECTIONDLG_H
