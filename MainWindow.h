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

    struct MainWindowPrivate {
        DbListModel	    dblist;
        QSqlTableModel *datatablemodel = Q_NULLPTR;
        int			    datatablemodel_lastsort = -1;
        DbSchemaModel   schemamodel;
        QSqlQueryModel  userquerymodel;
        QVariantMap     bindTypes;
        QVariantMap     bindRef;
    };

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
    void addTableRow();
    void delTableRow();
    void copyTableData();
    void exportTableToCsv();
    void refreshTableData();
    void saveTableData();
    void revertTableData();
    void sortDataTable(int logicalIndex);

    // *** Query Tab ***
    void runQuery();
    void copyQueryResult();
    void exportQueryToCsv();
    void clearQueryResult();
    void loadQueryFromFile();
    void saveQueryToFile();

    void setTableHeaders();

private:
    void setupUI();
    void setupIcons();
    void setupActions();
    QVariantMap setBindValues(const QStringList &params, DbConnection *dbc);
    void exportToCsv(QAbstractTableModel *model);

private: // static
    static void saveToClipboard(QSqlQuery query, const QItemSelection &sellist, QClipboard::Mode mode);
    static bool launch(const QUrl &url, const QString &client);

private:
    Ui::MainWindow *ui;
    SimpleReportWidget *simpleReportTab;
    MainWindowPrivate d;
};

#endif // MAINWINDOW_H
