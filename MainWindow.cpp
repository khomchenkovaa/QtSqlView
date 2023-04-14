#include "MainWindow.h"
#include "ui_MainWindow.h"

#include "sqlhighlighter.h"
#include "xcsvmodel.h"

#include "ConnectionDlg.h"
#include "QueryParamDlg.h"
#include "TableHeadersDlg.h"
#include "printtemplatedlg.h"

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

#include <QDebug>

/******************************************************************/

MainWindow::MainWindow()
    : QMainWindow()
    , datatablemodel(NULL)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    statusBar()->hide();

    dblist.loadFromSettings();

    ui->treeDbList->setModel(&dblist);

    ui->treeDbList->setContextMenuPolicy(Qt::CustomContextMenu);
    connect(ui->treeDbList, SIGNAL(customContextMenuRequested(const QPoint&)),
            this, SLOT(show_treeDbList_contextMenu(const QPoint &)));

    ui->dataTable->setContextMenuPolicy(Qt::CustomContextMenu);
    connect(ui->dataTable, SIGNAL(customContextMenuRequested(const QPoint&)),
            this, SLOT(show_dataTable_contextMenu(const QPoint &)));

    connect(ui->dataTable->horizontalHeader(), SIGNAL(sectionDoubleClicked(int)),
            this, SLOT(slot_dataTable_horizontalHeader_sectionDoubleClicked(int)));

    ui->schemaTable->setModel(&schemamodel);
    ui->schemaTable->verticalHeader()->hide();

    ui->queryTable->hide();
    ui->queryTable->setModel(&userquerymodel);

    // configure query editor
    QFont font("Courier", 10);
    font.setFixedPitch(true);
    ui->editQuery->setFont(font);

    new SQLHighlighter(ui->editQuery->document());
}

/******************************************************************/

MainWindow::~MainWindow()
{
    delete ui;
}

/******************************************************************/

void MainWindow::closeEvent(QCloseEvent *)
{
    dblist.saveToSettings();
}

/******************************************************************/

void MainWindow::on_action_AddConnection_triggered()
{
    ConnectionDlg wc(this);
    if (wc.exec() == QDialog::Accepted) {
        dblist.addDbConnection( wc.dbp );
    }
}

/******************************************************************/

void MainWindow::on_action_EditConnection_triggered()
{
    QModelIndex selected = ui->treeDbList->currentIndex();

    int selectednum = dblist.getDbConnectionNum(selected);

    if (selectednum < 0) {
        QMessageBox::critical(this, "QtSqlView",
                              "No database connection selected. Click on one of the entries in the database list.");
    } else {
        ConnectionDlg wc(this, dblist.list[selectednum]->dbparam);
        if (wc.exec() == QDialog::Accepted) {
            dblist.editDbConnection(selectednum, wc.dbp);
        }
    }
}

/******************************************************************/

void MainWindow::on_action_RemoveConnection_triggered()
{
    QModelIndex selected = ui->treeDbList->currentIndex();

    int selectednum = dblist.getDbConnectionNum(selected);

    if (selectednum < 0) {
        QMessageBox::critical(this, "QtSqlView",
                              "No database connection selected. Click on one of the entries in the database list.");
    } else {
        dblist.delDbConnection( selectednum );
    }
}

/******************************************************************/

void MainWindow::on_action_RefreshTablelist_triggered()
{
    dblist.refresh();
}

/******************************************************************/

void MainWindow::on_action_Exit_triggered()
{
    close();
}

/******************************************************************/

void MainWindow::on_action_About_triggered()
{
    QMessageBox::about(this, "About QtSqlView",
                       "QtSqlView is a simple SQL database browser\n"
                       "built on Qt's components.\n\n");
}

/******************************************************************/

void MainWindow::on_action_VisitWebsite_triggered()
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

void MainWindow::on_action_AboutQt_triggered()
{
    QMessageBox::aboutQt(this, "About Qt");
}

/******************************************************************/

void MainWindow::on_treeDbList_clicked(const QModelIndex &index)
{
    DbTable* dbt = dblist.getDbTable(index);

    if (!dbt) return;

    if (datatablemodel) {
        delete datatablemodel;
    }

    datatablemodel = new QSqlTableModel(this, dbt->dbconn->db);
    ui->dataTable->setModel(datatablemodel);

    datatablemodel->setTable(dbt->tablename);
    datatablemodel->setEditStrategy(QSqlTableModel::OnManualSubmit);

    datatablemodel->select();
    datatablemodel_lastsort = -1;

    ui->dataTable->resizeColumnsToContents();
    ui->dataTable->resizeRowsToContents();

    schemamodel.setRecord(dbt, datatablemodel->record());
}

/******************************************************************/

void MainWindow::on_treeDbList_expanded(const QModelIndex &index)
{
    dblist.expanding(index);
}

/******************************************************************/

void MainWindow::on_treeDbList_collapsed(const QModelIndex &index)
{
    dblist.collapsed(index);
}

/******************************************************************/

void MainWindow::show_treeDbList_contextMenu(const QPoint &position)
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

void MainWindow::show_dataTable_contextMenu(const QPoint &position)
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

void MainWindow::on_action_AddRow_triggered()
{
    if (!datatablemodel) return;

    QModelIndex index = ui->dataTable->currentIndex();

    int row = index.row() == -1 ? 0 : index.row();
    datatablemodel->insertRow(row);

    index = datatablemodel->index(row, 0);
    ui->dataTable->setCurrentIndex(index);
    ui->dataTable->edit(index);
}

/******************************************************************/

void MainWindow::on_addRowButton_clicked()
{
    on_action_AddRow_triggered();
}

/******************************************************************/

void MainWindow::on_action_DelRow_triggered()
{
    if (!datatablemodel) return;

    QModelIndexList indexlist = ui->dataTable->selectionModel()->selectedIndexes();

    for (int i = 0; i < indexlist.count(); ++i) {
        datatablemodel->removeRow(indexlist.at(i).row());
    }
}

/******************************************************************/

void MainWindow::on_delRowButton_clicked()
{
    on_action_DelRow_triggered();
}

/******************************************************************/

void MainWindow::on_copyDataButton_clicked()
{
    if (!datatablemodel) return;

    QItemSelectionModel *selmodel = ui->dataTable->selectionModel();
    saveToClipboard(datatablemodel->query(), selmodel->selection(), QClipboard::Clipboard);
}

/******************************************************************/

void MainWindow::on_toCsvDataButton_clicked()
{
    exportToCsv(datatablemodel);
}

/******************************************************************/

void MainWindow::on_action_RefreshData_triggered()
{
    if (!datatablemodel) return;
    datatablemodel->select();
}

/******************************************************************/

void MainWindow::on_refreshDataButton_clicked()
{
    on_action_RefreshData_triggered();
}

/******************************************************************/

void MainWindow::on_action_SaveData_triggered()
{
    if (!datatablemodel) return;
    datatablemodel->submitAll();
}

/******************************************************************/

void MainWindow::on_saveDataButton_clicked()
{
    on_action_SaveData_triggered();
}

/******************************************************************/

void MainWindow::on_action_RevertData_triggered()
{
    if (!datatablemodel) return;
    datatablemodel->revertAll();
}

/******************************************************************/

void MainWindow::on_revertDataButton_clicked()
{
    on_action_RevertData_triggered();
}

/******************************************************************/

void MainWindow::slot_dataTable_horizontalHeader_sectionDoubleClicked(int logicalIndex)
{
    if (!datatablemodel) return;

    Qt::SortOrder order = Qt::AscendingOrder;

    if (logicalIndex == datatablemodel_lastsort) {
        order = Qt::DescendingOrder;
        datatablemodel_lastsort = -1;
    } else {
        datatablemodel_lastsort = logicalIndex;
    }

    datatablemodel->sort(logicalIndex, order);
}

/******************************************************************/

void MainWindow::on_goQueryButton_clicked()
{
    // clear all
    userquerymodel.clear();
    ui->queryResultText->clear();

    // check connections
    DbConnection *dbc = dblist.getDbConnection( ui->treeDbList->currentIndex() );

    if (!dbc) {
        ui->queryTable->hide();
        ui->queryResultText->show();
        ui->queryResultText->setPlainText("No database connection selected.\nAdd and activate a connection in the left tree view.");
        return;
    }

    if (!dbc->db.isOpen()) {
        QSqlError ce = dbc->connect(&dblist);
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
    QStringList params = findBindings(sqlText);
    QVariantMap bindings = setBindValues(params);

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
            userquerymodel.setQuery(query);
            ui->queryResultText->hide();
            ui->queryTable->show();
            ui->queryTable->resizeColumnsToContents();
            ui->queryTable->resizeRowsToContents();
        } else {
            ui->queryTable->hide();
            ui->queryResultText->show();

            ui->queryResultText->setPlainText(QString("%1 rows affected.")
                                          .arg( query.numRowsAffected() ));
        }
    } else {
        ui->queryTable->hide();
        ui->queryResultText->show();
        ui->queryResultText->setPlainText(QString("%1\n%2")
                                      .arg(query.lastError().driverText(),
                                           query.lastError().databaseText()));
    }
}

/******************************************************************/

void MainWindow::on_copyQueryDataButton_clicked()
{
    QItemSelectionModel *selmodel = ui->queryTable->selectionModel();
    saveToClipboard(userquerymodel.query(), selmodel->selection(), QClipboard::Clipboard);
}

/******************************************************************/

void MainWindow::on_toScvQueryButton_clicked()
{
    exportToCsv(&userquerymodel);
}

/******************************************************************/

void MainWindow::on_setHeadersButton_clicked()
{
    QSqlRecord rec = userquerymodel.record();
    if (rec.isEmpty()) {
        return;
    }
    QStringList fields;
    for (int i=0; i < rec.count(); ++i) {
        fields << rec.field(i).name();
    }
    QStringList headers;
    for (int i=0; i < userquerymodel.columnCount(); ++i) {
        headers << userquerymodel.headerData(i, Qt::Horizontal).toString();
    }
    TableHeadersDlg dlg(fields);
    dlg.setHeaders(headers);
    if (dlg.exec() == QDialog::Accepted) {
        headers = dlg.headers();
        for (int i=0; i < headers.size(); ++i) {
            userquerymodel.setHeaderData(i, Qt::Horizontal, headers.at(i));
        }
    }
}

/******************************************************************/

void MainWindow::on_printButton_clicked()
{
    PrintTemplateDlg dlg;
    if (dlg.exec() == QDialog::Rejected) {
        return;
    }
    const QString title = dlg.title();
    const QString header = dlg.header();
    const QString footer = dlg.footer();

    QPrinter printer(QPrinter::HighResolution);
    printer.setFullPage( true );
    QPrintPreviewDialog preview(&printer, this);
    preview.setWindowFlags ( Qt::Window );
    connect(&preview, &QPrintPreviewDialog::paintRequested,
            this, [this, title, header, footer](QPrinter *printer){
        QScopedPointer<QTextDocument> document(new QTextDocument());
        QTextCursor cursor(document.data());
        if (!title.isEmpty()) {
            cursor.insertHtml("<h1 align=\"center\">" + title + "</h1>");
            cursor.insertBlock();
        }
        if (!header.isEmpty()) {
            cursor.insertHtml(header);
            cursor.insertBlock();
        }
        printTable(&cursor, &userquerymodel);
        if (!footer.isEmpty()) {
            cursor.movePosition(QTextCursor::End, QTextCursor::MoveAnchor);
            cursor.insertBlock();
            cursor.insertHtml(footer);
            cursor.insertBlock();
        }
        document->print(printer);
    });
    preview.exec();
}

/******************************************************************/

void MainWindow::on_clearQueryButton_clicked()
{
    ui->editQuery->clear();
}

/******************************************************************/

void MainWindow::on_loadQueryButton_clicked()
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

void MainWindow::on_saveQueryButton_clicked()
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

void MainWindow::saveToClipboard(QSqlQuery query, const QItemSelection &sellist, QClipboard::Mode mode)
{
    if (!query.isSelect() || !query.isActive()) {
        qDebug() << "bad query";
        return;
    }

    QString seltext;

    for (auto selrange : sellist) {
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
    return (QProcess::startDetached(client + " " + url.toEncoded()));
}

/******************************************************************/

void MainWindow::exportToCsv(QAbstractItemModel *model)
{
    QString filters("CSV files (*.csv);;All files (*.*)");
    QString defaultFilter("CSV files (*.csv)");
    QString fileName = QFileDialog::getSaveFileName(Q_NULLPTR, tr("Export to CSV-file"),
                                                    QCoreApplication::applicationDirPath(),
                                                    filters, &defaultFilter);
    if (fileName.isEmpty()) return;

    XCsvModel csvModel;
    csvModel.importFromModel(model);
    csvModel.toCSV(fileName, true);
}

/******************************************************************/

QStringList MainWindow::findBindings(const QString &sql)
{
    QStringList result;
    QRegularExpression re(":[\\d\\w]+");
    QRegularExpressionMatchIterator i = re.globalMatch(sql);
    while (i.hasNext()) {
        QRegularExpressionMatch match = i.next();
        QString word = match.captured(0);
        if (!result.contains(word)) {
            result << word;
        }
    }
    return result;
}

/******************************************************************/

QVariantMap MainWindow::setBindValues(const QStringList &params)
{
    if (params.isEmpty()) {
        return QVariantMap();
    }

    QueryParamDlg dlg(params);
    if (dlg.exec()) {
        return dlg.bindings();
    }

    return QVariantMap();
}

/******************************************************************/

void MainWindow::printTable(QTextCursor *cursor, QAbstractItemModel *model) {
    cursor->clearSelection();
    cursor->movePosition(QTextCursor::End, QTextCursor::KeepAnchor);

    const int rows = model->rowCount();
    const int cols = model->columnCount();

    QTextTable* table = cursor->insertTable(rows+1, cols);
    // print header
    for (int j=0; j<cols; ++j) {
        auto pos = table->cellAt(0, j).firstCursorPosition();
        pos.insertHtml("<p align=\"center\"><b>" +
                       model->headerData(j, Qt::Horizontal).toString() +
                       "</b>");
    }
    // print data
    for (int i=0; i<rows; ++i) {
        for (int j=0; j<cols; ++j) {
            auto pos = table->cellAt(i+1, j).firstCursorPosition();
            QModelIndex idx = model->index(i, j);
            pos.insertHtml(model->data(idx).toString());
        }
    }
}

/******************************************************************/
