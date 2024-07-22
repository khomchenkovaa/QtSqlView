#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include "dbschemamodel.h"
#include "dblistmodel.h"

#include <QMainWindow>
#include <QClipboard>
#include <QItemSelection>
#include <QTextDocument>

#include <QSqlTableModel>
#include <QSqlQuery>

#ifdef Q_WS_WIN
#include <windows.h>
#include <shlobj.h>
#include <intshcut.h>
#endif

QT_BEGIN_NAMESPACE
class QTextCursor;
QT_END_NAMESPACE

namespace Ui {
class MainWindow;
}

class SimpleReportWidget;

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow();
    ~MainWindow();

    virtual void closeEvent(QCloseEvent *);

private slots:
    // *** Menu Actions ***
    void addConnection();
    void editConnection();
    void removeConnection();
    void showAboutBox();
    void visitWebsite();

    // *** Triggers of the DbList TreeView 
    void changeCurrentTable(const QModelIndex &index);
    void showTreeDbListContextMenu(const QPoint &position);

    // *** Data Table Tab ***
    void showDataTableContextMenu(const QPoint &position);
    void on_action_AddRow_triggered();
    void on_addRowButton_clicked();
    void on_action_DelRow_triggered();
    void on_delRowButton_clicked();
    void on_copyDataButton_clicked();
    void on_toCsvDataButton_clicked();
    void on_action_RefreshData_triggered();
    void on_refreshDataButton_clicked();
    void on_action_SaveData_triggered();
    void on_saveDataButton_clicked();
    void on_action_RevertData_triggered();
    void on_revertDataButton_clicked();
    void sortDataTable(int logicalIndex);

    // *** Query Tab ***
    void on_goQueryButton_clicked();
    void on_copyQueryDataButton_clicked();
    void on_toScvButton_clicked();
    void on_clearQueryButton_clicked();
    void on_loadQueryButton_clicked();
    void on_saveQueryButton_clicked();

    void setTableHeaders();

private:
    void setupUI();
    void setupActions();
    QVariantMap setBindValues(const QStringList &params, DbConnection *dbc);
    void exportToCsv(QAbstractTableModel *model);

private: // static
    static void saveToClipboard(QSqlQuery query, const QItemSelection &sellist, QClipboard::Mode mode);
    static bool launch(const QUrl &url, const QString &client);

public:
    DbListModel	    dblist;
    QSqlTableModel *datatablemodel;
    int			    datatablemodel_lastsort;
    DbSchemaModel   schemamodel;
    QSqlQueryModel  userquerymodel;
    QVariantMap     bindTypes;
    QVariantMap     bindRef;

    SimpleReportWidget *simpleReportTab;

private:
    Ui::MainWindow *ui;
};

#endif // MAINWINDOW_H
