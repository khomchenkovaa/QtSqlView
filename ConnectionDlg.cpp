#include "ConnectionDlg.h"
#include "ui_ConnectionDlg.h"

#include "dblistmodel.h"

#include <QFileDialog>

#include <QMessageBox>

/******************************************************************/

ConnectionDlg::ConnectionDlg(QWidget *parent, const DbParameter &dbParameter)
    : QDialog(parent)
    , dbp(dbParameter)
    , ui(new Ui::ConnectionDlg)
{
    ui->setupUi(this);
    setupConnections();

    const auto drivers = QSqlDatabase::drivers();
    for (const auto &drv : drivers) {
        const QString desc = getDescription(drv);
        if (desc.isNull()) {
            ui->comboType->addItem(drv, drv);
        } else {
            ui->comboType->addItem(QString("%1 (%2)").arg(desc, drv), drv);
        }
    }

    if (!dbp.label.isEmpty()) {
        ui->editLabel->setText(dbp.label);
        ui->comboType->setCurrentIndex( ui->comboType->findData(dbp.driver) );
        ui->editHostname->setText(dbp.hostname);
        ui->spinPort->setValue(dbp.port);
        ui->editUsername->setText(dbp.username);
        ui->editPassword->setText(dbp.password);
        ui->checkAskPassword->setChecked(dbp.askpassword);
        ui->editDatabase->setText(dbp.database);
        ui->checkSysTables->setChecked(dbp.showsystables);
    }

    updatePasswordStatus();
}

/******************************************************************/

ConnectionDlg::~ConnectionDlg()
{
    delete ui;
}

/******************************************************************/

void ConnectionDlg::updatePasswordStatus()
{
    if (!ui->checkAskPassword->isEnabled()) return;
    ui->editPassword->setEnabled( !ui->checkAskPassword->isChecked() );
}

/******************************************************************/

void ConnectionDlg::testConnection()
{
    fetchDbParameter();

    DbConnection conn(dbp);
    DbListModel dblist;

    QSqlError ce = conn.connect(&dblist);

    if (ce.isValid()) {
        QMessageBox::critical(this, "Testing Connection",
                              QString("Connection failed:\n%1\n%2")
                              .arg(ce.driverText(), ce.databaseText()));
    } else {
        QMessageBox::information(this, "Testing Connection",
                                 QString("Connection established successfully."));
    }
}

/******************************************************************/

void ConnectionDlg::chooseSQLiteDbFile()
{
    QString filename = QFileDialog::getOpenFileName(this, "Choose a SQLite database file",
                                                    QString(),
                                                    "SQLite databases (*.db);;All Files (*.*)");
    if (filename.isEmpty()) return;
    ui->editDatabase->setText(filename);
}

/******************************************************************/

void ConnectionDlg::setupConnections()
{
    connect(ui->okButton, &QAbstractButton::clicked, this, [this](){
        fetchDbParameter();
        accept();
    });
    connect(ui->testButton, &QAbstractButton::clicked,
            this, &ConnectionDlg::testConnection);
    connect(ui->checkAskPassword, &QAbstractButton::clicked,
            this, &ConnectionDlg::updatePasswordStatus);
    connect(ui->comboType, QOverload<int>::of(&QComboBox::currentIndexChanged), this, [this](int index){
        dbp.driver = ui->comboType->itemData(index).toString();
        updateFields();
    });
    connect(ui->buttonSelectFile, &QAbstractButton::clicked,
            this, &ConnectionDlg::chooseSQLiteDbFile);
}

/******************************************************************/

void ConnectionDlg::fetchDbParameter()
{
    dbp.label         = ui->editLabel->text();
    dbp.driver        = ui->comboType->itemData( ui->comboType->currentIndex() ).toString();
    dbp.hostname      = ui->editHostname->text();
    dbp.port          = ui->spinPort->value();
    dbp.username      = ui->editUsername->text();
    dbp.password      = ui->editPassword->text();
    dbp.askpassword   = ui->checkAskPassword->isChecked();
    dbp.database      = ui->editDatabase->text();
    dbp.showsystables = ui->checkSysTables->isChecked();

    if (dbp.askpassword) {
        dbp.password.clear();
    }
}

/******************************************************************/

void ConnectionDlg::updateFields()
{
    const QString drv = dbp.driver;

    if (drv == "QSQLITE" || drv == "QSQLITE2") {
        ui->editHostname->setEnabled(false);
        ui->spinPort->setEnabled(false);
        ui->editUsername->setEnabled(false);
        ui->editPassword->setEnabled(false);
        ui->checkAskPassword->setEnabled(false);
        ui->labelDatabase->setText("Filename");
        ui->buttonSelectFile->setEnabled(true);
    } else if (drv == "QODBC" || drv == "QODBC3") {
        ui->editHostname->setEnabled(false);
        ui->spinPort->setEnabled(false);
        ui->editUsername->setEnabled(false);
        ui->editPassword->setEnabled(false);
        ui->checkAskPassword->setEnabled(false);
        ui->labelDatabase->setText("Connect");
        ui->buttonSelectFile->setEnabled(false);
    } else {
        ui->editHostname->setEnabled(true);
        ui->spinPort->setEnabled(true);
        ui->editUsername->setEnabled(true);
        ui->checkAskPassword->setEnabled(true);
        ui->labelDatabase->setText("Database");
        ui->buttonSelectFile->setEnabled(false);
        updatePasswordStatus();
    }
}

/******************************************************************/

QString ConnectionDlg::getDescription(const QString &drv)
{
    if (0) return "";
    else if (drv == "QDB2")		return "IBM DB2";
    else if (drv == "QIBASE")	return "Borland InterBase";
    else if (drv == "QOCI")		return "Oracle Call Interface";
    else if (drv == "QODBC")	return "ODBC";
    else if (drv == "QODBC3")	return "ODBC";
    else if (drv == "QTDS")		return "Sybase Adaptive Server";
    else if (drv == "QMYSQL")	return "MySQL 4.x";
    else if (drv == "QMYSQL3")	return "MySQL 3.x";
    else if (drv == "QPSQL")	return "PostgreSQL 8.x";
    else if (drv == "QPSQL7")	return "PostgreSQL 7.x";
    else if (drv == "QSQLITE")	return "SQLite 3.x";
    else if (drv == "QSQLITE2")	return "SQLite 2.x";
    else return QString();
}

/******************************************************************/
