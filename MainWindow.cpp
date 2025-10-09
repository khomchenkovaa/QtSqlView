#include "MainWindow.h"
#include "ui_MainWindow.h"

#include "sqlhighlighter.h"

#include "simplereportwidget.h"
#include "ConnectionDlg.h"
#include "QueryParamDlg.h"
#include "TableHeadersDlg.h"

#include <QMessageBox>
#include <QFileDialog>
#include <QTextStream>
#include <QProcess>

#include <QSqlRecord>
#include <QSqlField>

#include <QTextTable>
#include <QTextDocument>

#include <QtPrintSupport/QPrinter>
#include <QtPrintSupport/QPrintDialog>
#include <QtPrintSupport/QPrintPreviewDialog>

#include "report.h"

#include <QDebug>

enum {
    DataTab,
    SchemaTab,
    QueryTab,
    SimpleReportTab
};

/******************************************************************/

MainWindow::MainWindow()
    : QMainWindow()
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    setupUI();
    setupIcons();
    setupActions();
}

/******************************************************************/

MainWindow::~MainWindow()
{
    delete ui;
}

/******************************************************************/

void MainWindow::closeEvent(QCloseEvent *)
{
    d.dblist.saveAll();
}

/******************************************************************/

void MainWindow::addConnection()
{
    ConnectionDlg wc(this);
    if (wc.exec() == QDialog::Accepted) {
        d.dblist.addDbConnection( wc.dbp );
    }
}

/******************************************************************/

void MainWindow::editConnection()
{
    QModelIndex selected = ui->treeDbList->currentIndex();

    int selectednum = d.dblist.getDbConnectionNum(selected);

    if (selectednum < 0) {
        QMessageBox::critical(this, "QtSqlView",
                              "No database connection selected. Click on one of the entries in the database list.");
    } else {
        ConnectionDlg wc(this, d.dblist.getDbConnection(selectednum)->dbparam);
        if (wc.exec() == QDialog::Accepted) {
            d.dblist.editDbConnection(selectednum, wc.dbp);
        }
    }
}

/******************************************************************/

void MainWindow::removeConnection()
{
    QModelIndex selected = ui->treeDbList->currentIndex();

    int selectednum = d.dblist.getDbConnectionNum(selected);

    if (selectednum < 0) {
        QMessageBox::critical(this, "QtSqlView",
                              "No database connection selected. Click on one of the entries in the database list.");
    } else {
        d.dblist.delDbConnection( selectednum );
    }
}

/******************************************************************/

void MainWindow::showAboutBox()
{
    QMessageBox::about(this, "About QtSqlView",
                       "QtSqlView is a simple SQL database browser\n"
                       "built on Qt's components.\n\n");
}

/******************************************************************/

void MainWindow::visitWebsite()
{
    QUrl url("https://github.com/khomchenkovaa/QtSqlView");

#ifdef Q_WS_WIN
    QT_WA({ ShellExecute(0, 0, (TCHAR*)QString(url.toEncoded()).utf16(), 0, 0, SW_SHOWNORMAL); },
    { ShellExecuteA(0, 0, QString(url.toEncoded()).toLocal8Bit(), 0, 0, SW_SHOWNORMAL); });
#else
    if (launch(url, "firefox"))
        return;
    if (launch(url, "mozilla"))
        return;
    if (launch(url, "netscape"))
        return;
    if (launch(url, "opera"))
        return;
#endif
}

/******************************************************************/

void MainWindow::changeCurrentTable(const QModelIndex &index)
{
    const QSignalBlocker blocker(ui->treeDbList);

    DbTable* dbt = d.dblist.getDbTable(index);

    if (!dbt) return;

    if (d.datatablemodel) {
        d.datatablemodel->deleteLater();
    }

    d.datatablemodel = new QSqlTableModel(this, dbt->dbconn->db);
    d.datatablemodel->setTable(dbt->tablename);

    ui->dataTable->setModel(d.datatablemodel);
    d.schemamodel.setRecord(dbt->dbconn->db.driverName(), d.datatablemodel->record(), d.datatablemodel->primaryKey());

    d.datatablemodel->setEditStrategy(QSqlTableModel::OnManualSubmit);
    d.datatablemodel->select();
    d.datatablemodel_lastsort = -1;

    ui->dataTable->resizeColumnsToContents();
    ui->dataTable->resizeRowsToContents();
}

/******************************************************************/

void MainWindow::showTreeDbListContextMenu(const QPoint &position)
{
    QMenu contextmenu(this);

    QModelIndex index = ui->treeDbList->indexAt(position);
    if (index.isValid()) {
        contextmenu.addAction(ui->action_EditConnection);
        contextmenu.addAction(ui->action_RemoveConnection);
        contextmenu.addSeparator();
    }

    contextmenu.addAction(ui->action_AddConnection);
    contextmenu.addAction(ui->action_RefreshTablelist);

    contextmenu.exec(ui->treeDbList->mapToGlobal(position));
}

/******************************************************************/

void MainWindow::showDataTableContextMenu(const QPoint &position)
{
    QMenu contextmenu(this);

    contextmenu.addAction(ui->action_RevertData);
    contextmenu.addAction(ui->action_RefreshData);
    contextmenu.addSeparator();
    contextmenu.addAction(ui->action_AddRow);
    contextmenu.addAction(ui->action_DelRow);

    contextmenu.exec(ui->dataTable->mapToGlobal(position));
}

/******************************************************************/

void MainWindow::addTableRow()
{
    if (!d.datatablemodel) return;

    QModelIndex index = ui->dataTable->currentIndex();

    int row = index.row() == -1 ? 0 : index.row();
    d.datatablemodel->insertRow(row);

    index = d.datatablemodel->index(row, 0);
    ui->dataTable->setCurrentIndex(index);
    ui->dataTable->edit(index);
}

/******************************************************************/

void MainWindow::delTableRow()
{
    if (!d.datatablemodel) return;

    QModelIndexList indexlist = ui->dataTable->selectionModel()->selectedIndexes();

    for (int i = 0; i < indexlist.count(); ++i) {
        d.datatablemodel->removeRow(indexlist.at(i).row());
    }
}

/******************************************************************/

void MainWindow::copyTableData()
{
    if (!d.datatablemodel) return;

    QItemSelectionModel *selmodel = ui->dataTable->selectionModel();
    saveToClipboard(d.datatablemodel->query(), selmodel->selection(), QClipboard::Clipboard);
}

/******************************************************************/

void MainWindow::exportTableToCsv()
{
    if (!d.datatablemodel) return;
    exportToCsv(d.datatablemodel);
}

/******************************************************************/

void MainWindow::refreshTableData()
{
    if (!d.datatablemodel) return;
    d.datatablemodel->select();
}

/******************************************************************/

void MainWindow::saveTableData()
{
    if (!d.datatablemodel) return;
    d.datatablemodel->submitAll();
}

/******************************************************************/

void MainWindow::revertTableData()
{
    if (!d.datatablemodel) return;
    d.datatablemodel->revertAll();
}

/******************************************************************/

void MainWindow::sortDataTable(int logicalIndex)
{
    if (!d.datatablemodel) return;

    Qt::SortOrder order = Qt::AscendingOrder;

    if (logicalIndex == d.datatablemodel_lastsort) {
        order = Qt::DescendingOrder;
        d.datatablemodel_lastsort = -1;
    } else {
        d.datatablemodel_lastsort = logicalIndex;
    }

    d.datatablemodel->sort(logicalIndex, order);
}

/******************************************************************/

void MainWindow::runQuery()
{
    // clear all
    d.userquerymodel.clear();
    ui->queryResultText->clear();

    // check connections
    DbConnection *dbc = d.dblist.getDbConnection( ui->treeDbList->currentIndex() );

    if (!dbc) {
        ui->queryTable->hide();
        ui->queryResultText->show();
        ui->queryResultText->setPlainText("No database connection selected.\nAdd and activate a connection in the left tree view.");
        return;
    }

    if (!dbc->db.isOpen()) {
        QSqlError ce = dbc->connect(&d.dblist);
        if (ce.isValid()) {
            ui->queryTable->hide();
            ui->queryResultText->show();
            ui->queryResultText->setPlainText(QString("%1\n%2")
                                          .arg(ce.driverText(), ce.databaseText()));
            return;
        }
    }

    // prepare for bindings
    QString sqlText = ui->editQuery->toPlainText();
    QStringList params = Report::findBindings(sqlText);
    QVariantMap bindings = setBindValues(params, dbc);

    // initialize new sql query object   
    QSqlQuery query(dbc->db);
    query.prepare(sqlText);
    QMapIterator<QString, QVariant> i(bindings);
    while (i.hasNext()) {
        i.next();
        query.bindValue(i.key(), i.value());
    }
    if (query.exec()) {
        if (query.isSelect()) {
            d.userquerymodel.setQuery(query);
            ui->queryResultText->hide();
            ui->queryTable->show();
            ui->queryTable->resizeColumnsToContents();
            ui->queryTable->resizeRowsToContents();
            ui->tabWidget->setTabEnabled(SimpleReportTab, true);
            if (simpleReportTab) {
                simpleReportTab->updateView();
            }
        } else {
            ui->queryTable->hide();
            ui->tabWidget->setTabEnabled(SimpleReportTab, false);
            ui->queryResultText->show();

            ui->queryResultText->setPlainText(QString("%1 rows affected.")
                                          .arg( query.numRowsAffected() ));
        }
    } else {
        ui->queryTable->hide();
        ui->tabWidget->setTabEnabled(SimpleReportTab, false);
        ui->queryResultText->show();
        ui->queryResultText->setPlainText(QString("%1\n%2")
                                      .arg(query.lastError().driverText(),
                                           query.lastError().databaseText()));
    }
}

/******************************************************************/

void MainWindow::copyQueryResult()
{
    auto selmodel = ui->queryTable->selectionModel();
    saveToClipboard(d.userquerymodel.query(), selmodel->selection(), QClipboard::Clipboard);
}

/******************************************************************/

void MainWindow::exportQueryToCsv()
{
    if (ui->queryTable->isHidden()) return;
    exportToCsv(&d.userquerymodel);
}

/******************************************************************/

void MainWindow::clearQueryResult()
{
    ui->editQuery->clear();
}

/******************************************************************/

void MainWindow::loadQueryFromFile()
{
    QString filename = QFileDialog::getOpenFileName(this, "Choose a SQL text file",
                                                    QString(),
                                                    "SQL text files (*.sql *.txt);;All Files (*.*)");

    if (filename.isEmpty()) return;

    QFile file(filename);
    if (!file.open(QIODevice::ReadOnly | QIODevice::Text)) {
        QMessageBox::critical(this, "QtSqlView",
                              "Could not load sql query text file");
        return;
    }

    ui->editQuery->setPlainText( file.readAll() );
}

/******************************************************************/

void MainWindow::saveQueryToFile()
{
    QString filename = QFileDialog::getSaveFileName(this, "Choose a SQL text file",
                                                    QString(),
                                                    "SQL text files (*.sql *.txt);;All Files (*.*)");

    if (filename.isEmpty()) return;

    QFile file(filename);
    if (!file.open(QIODevice::WriteOnly | QIODevice::Text)) {
        QMessageBox::critical(this, "QtSqlView",
                              "Could not save sql query text file");
        return;
    }

    QTextStream out(&file);
    out << ui->editQuery->toPlainText();
}

/******************************************************************/

void MainWindow::setTableHeaders()
{
    QSqlRecord rec = d.userquerymodel.record();
    if (rec.isEmpty()) {
        return;
    }
    QStringList fields;
    for (int i=0; i < rec.count(); ++i) {
        fields << rec.field(i).name();
    }
    QStringList headers;
    for (int i=0; i < d.userquerymodel.columnCount(); ++i) {
        headers << d.userquerymodel.headerData(i, Qt::Horizontal).toString();
    }
    TableHeadersDlg dlg(fields);
    dlg.setHeaders(headers);
    if (dlg.exec() == QDialog::Accepted) {
        headers = dlg.headers();
        for (int i=0; i < headers.size(); ++i) {
            d.userquerymodel.setHeaderData(i, Qt::Horizontal, headers.at(i));
        }
    }
}

/******************************************************************/

void MainWindow::setupUI()
{
    statusBar()->hide();

    d.dblist.loadAll();

    ui->treeDbList->setModel(&d.dblist);

    ui->treeDbList->setContextMenuPolicy(Qt::CustomContextMenu);
    connect(ui->treeDbList, &QWidget::customContextMenuRequested,
            this, &MainWindow::showTreeDbListContextMenu);

    ui->dataTable->setContextMenuPolicy(Qt::CustomContextMenu);
    connect(ui->dataTable, &QWidget::customContextMenuRequested,
            this, &MainWindow::showDataTableContextMenu);

    connect(ui->dataTable->horizontalHeader(), &QHeaderView::sectionDoubleClicked,
            this, &MainWindow::sortDataTable);

    ui->schemaTable->setModel(&d.schemamodel);
    ui->schemaTable->verticalHeader()->hide();

    // configure query tab
    QFont font("Courier", 10);
    font.setFixedPitch(true);
    ui->editQuery->setFont(font);

    new SQLHighlighter(ui->editQuery->document());

    ui->queryTable->hide();
    ui->queryTable->setModel(&d.userquerymodel);

    connect(ui->setHeadersButton, &QToolButton::clicked,
            this, &MainWindow::setTableHeaders);

    // configure simple report tab
    simpleReportTab = new SimpleReportWidget(this);
    simpleReportTab->setUserQueryModel(&d.userquerymodel);
    ui->tabWidget->addTab(simpleReportTab, QIcon::fromTheme("printer"), tr("Simple Report"));
    ui->tabWidget->setTabEnabled(SimpleReportTab, false);

    connect(simpleReportTab, &SimpleReportWidget::tableHeaders,
            this, &MainWindow::setTableHeaders);
}

/******************************************************************/

void MainWindow::setupIcons()
{
    auto iconAddRow = QIcon::fromTheme("list-add", QIcon(":/img/addrow.png"));
    ui->action_AddRow->setIcon(iconAddRow);
    ui->addRowButton->setIcon(iconAddRow);

    auto iconDelRow = QIcon::fromTheme("list-remove", QIcon(":/img/delrow.png"));
    ui->action_DelRow->setIcon(iconDelRow);
    ui->delRowButton->setIcon(iconDelRow);

    auto iconCopyEdit = QIcon::fromTheme("edit-copy", QIcon(":/img/copyedit.png"));
    ui->copyDataButton->setIcon(iconCopyEdit);
    ui->copyQueryDataButton->setIcon(iconCopyEdit);

    auto iconFileSave = QIcon::fromTheme("document-save", QIcon(":/img/filesave.png"));
    ui->action_SaveData->setIcon(iconFileSave);
    ui->saveDataButton->setIcon(iconFileSave);
    ui->saveQueryButton->setIcon(iconFileSave);

    auto iconClear = QIcon::fromTheme("edit-clear", QIcon(":/img/trash.png"));
    ui->clearQueryButton->setIcon(iconClear);

    auto iconFileLoad = QIcon::fromTheme("document-open", QIcon(":/img/fileload.png"));
    ui->loadQueryButton->setIcon(iconFileLoad);

    auto iconRefresh = QIcon::fromTheme("view-refresh", QIcon(":/img/refresh.png"));
    ui->action_RefreshData->setIcon(iconRefresh);
    ui->action_RefreshTablelist->setIcon(iconRefresh);
    ui->refreshDataButton->setIcon(iconRefresh);

    // document-revert
    // x-office-spreadsheet
    // insert-text
}

/******************************************************************/

void MainWindow::setupActions()
{
    // *** Menu Actions ***
    connect(ui->action_AddConnection, &QAction::triggered,
            this, &MainWindow::addConnection);
    connect(ui->action_EditConnection, &QAction::triggered,
            this, &MainWindow::editConnection);
    connect(ui->action_RemoveConnection, &QAction::triggered,
            this, &MainWindow::removeConnection);
    connect(ui->action_RefreshTablelist, &QAction::triggered, this, [this](){
        d.dblist.refresh();
    });
    connect(ui->action_Exit, &QAction::triggered,
            this, &MainWindow::close);
    connect(ui->action_About, &QAction::triggered,
            this, &MainWindow::showAboutBox);
    connect(ui->action_VisitWebsite, &QAction::triggered,
            this, &MainWindow::visitWebsite);
    connect(ui->action_AboutQt, &QAction::triggered, this, [this](){
        QMessageBox::aboutQt(this, "About Qt");
    });

    // *** Triggers of the DbList TreeView
    connect(ui->treeDbList, &QAbstractItemView::clicked,
            this, &MainWindow::changeCurrentTable);
    connect(ui->treeDbList, &QTreeView::expanded,
            &d.dblist, &DbListModel::expanding);
    connect(ui->treeDbList, &QTreeView::collapsed,
            &d.dblist, &DbListModel::collapsed);

    // *** Data Table Tab ***
    connect(ui->action_AddRow, &QAction::triggered,
            this, &MainWindow::addTableRow);
    connect(ui->addRowButton, &QAbstractButton::clicked,
            this, &MainWindow::addTableRow);
    connect(ui->action_DelRow, &QAction::triggered,
            this, &MainWindow::delTableRow);
    connect(ui->delRowButton, &QAbstractButton::clicked,
            this, &MainWindow::delTableRow);
    connect(ui->copyDataButton, &QAbstractButton::clicked,
            this, &MainWindow::copyTableData);
    connect(ui->toCsvDataButton, &QAbstractButton::clicked,
            this, &MainWindow::exportTableToCsv);
    connect(ui->action_RefreshData, &QAction::triggered,
            this, &MainWindow::refreshTableData);
    connect(ui->refreshDataButton, &QAbstractButton::clicked,
            this, &MainWindow::refreshTableData);
    connect(ui->action_SaveData, &QAction::triggered,
            this, &MainWindow::saveTableData);
    connect(ui->saveDataButton, &QAbstractButton::clicked,
            this, &MainWindow::saveTableData);
    connect(ui->action_RevertData, &QAction::triggered,
            this, &MainWindow::revertTableData);
    connect(ui->revertDataButton, &QAbstractButton::clicked,
            this, &MainWindow::revertTableData);

    // *** Query Tab ***
    connect(ui->goQueryButton, &QAbstractButton::clicked,
            this, &MainWindow::runQuery);
    connect(ui->copyQueryDataButton, &QAbstractButton::clicked,
            this, &MainWindow::copyQueryResult);
    connect(ui->toScvButton, &QAbstractButton::clicked,
            this, &MainWindow::exportQueryToCsv);
    connect(ui->clearQueryButton, &QAbstractButton::clicked,
            this, &MainWindow::clearQueryResult);
    connect(ui->loadQueryButton, &QAbstractButton::clicked,
            this, &MainWindow::loadQueryFromFile);
    connect(ui->saveQueryButton, &QAbstractButton::clicked,
            this, &MainWindow::saveQueryToFile);
}

/******************************************************************/

QVariantMap MainWindow::setBindValues(const QStringList &params, DbConnection *dbc)
{
    if (params.isEmpty()) {
        return QVariantMap();
    }

    QueryParamDlg dlg(dbc);
    dlg.setBindSql(d.bindRef);
    dlg.setBindTypes(d.bindTypes);
    dlg.setupParams(params);
    if (dlg.exec()) {
        d.bindTypes = dlg.bindTypes();
        d.bindRef = dlg.bindSql();
        return dlg.bindings(params);
    }

    return QVariantMap();
}

/******************************************************************/

void MainWindow::exportToCsv(QAbstractTableModel *model)
{
    QString fileName = Report::exportToCsvDlg(this);
    if (fileName.isEmpty()) return;

    ListReport report;
    report.setModel(model);
    report.toCsvFile(fileName);
}

/******************************************************************/

void MainWindow::saveToClipboard(QSqlQuery query, const QItemSelection &sellist, QClipboard::Mode mode)
{
    if (!query.isSelect() || !query.isActive()) {
        qDebug() << "bad query";
        return;
    }

    QString seltext;

    for (const auto &selrange : sellist) {
        if (!query.seek(selrange.top())) {
            qDebug() << "Could not seek in result";
            continue;
        }

        for (int rowi = selrange.top(); rowi <= selrange.bottom(); ++rowi) {
            for(int fi = selrange.left(); fi <= selrange.right(); ++fi) {
                if (fi != selrange.left()) seltext += "\t";
                seltext += query.value(fi).toString();
            }
            seltext += "\n";
            query.next();
        }
    }

    QClipboard *clip = QApplication::clipboard();
    clip->setText(seltext, mode);
}

/******************************************************************/

bool MainWindow::launch(const QUrl &url, const QString &client)
{
    const auto args = QStringList() << url.toEncoded();
    return (QProcess::startDetached(client, args));
}

/******************************************************************/
