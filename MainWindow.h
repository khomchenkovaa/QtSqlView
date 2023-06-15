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
class KdReportWidget;

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow();
    ~MainWindow();

    virtual void closeEvent(QCloseEvent *);

private slots:
    // *** Menu Actions ***
    void on_action_AddConnection_triggered();
    void on_action_EditConnection_triggered();
    void on_action_RemoveConnection_triggered();
    void on_action_RefreshTablelist_triggered();
    void on_action_Exit_triggered();
    void on_action_About_triggered();
    void on_action_VisitWebsite_triggered();
    void on_action_AboutQt_triggered();

    // *** Triggers of the DbList TreeView 
    void on_treeDbList_clicked(const QModelIndex &index);
    void on_treeDbList_expanded(const QModelIndex &index);
    void on_treeDbList_collapsed(const QModelIndex &index);
    void show_treeDbList_contextMenu(const QPoint &position);

    // *** Data Table Tab ***
    void show_dataTable_contextMenu(const QPoint &position);
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
    void slot_dataTable_horizontalHeader_sectionDoubleClicked(int logicalIndex);

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

private: // static
    static void saveToClipboard(QSqlQuery query, const QItemSelection &sellist, QClipboard::Mode mode);
    static bool launch(const QUrl &url, const QString &client);
    static QStringList findBindings(const QString &sql);
    static QVariantMap setBindValues(const QStringList &params, DbConnection *dbc);

public:
    DbListModel	    dblist;
    QSqlTableModel *datatablemodel;
    int			    datatablemodel_lastsort;
    DbSchemaModel   schemamodel;
    QSqlQueryModel  userquerymodel;

    SimpleReportWidget *simpleReportTab;
    KdReportWidget     *kdReportTab;

private:
    Ui::MainWindow *ui;
};

#endif // MAINWINDOW_H
