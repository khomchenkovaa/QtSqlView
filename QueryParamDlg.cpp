#include "QueryParamDlg.h"

#include "dbconnection.h"

#include "reportutils.h"

#include <QGridLayout>
#include <QDialogButtonBox>
#include <QVBoxLayout>

#include <QInputDialog>
#include <QMessageBox>

enum {
    NameColumn,
    TypeColumn,
    ValueColumn
};

/******************************************************************/

QueryParamDlg::QueryParamDlg(DbConnection *dbc, QWidget *parent) :
    QDialog(parent)
{
    m_Db = dbc;
    setupUI();
}

/******************************************************************/

QueryParamDlg::~QueryParamDlg()
{

}

/******************************************************************/

void QueryParamDlg::setBindSql(const QVariantMap &sqlRef)
{
    QMapIterator<QString, QVariant> i(sqlRef);
    while (i.hasNext()) {
        i.next();
        m_RefSql.insert(i.key(), i.value());
    }
}

/******************************************************************/

QVariantMap QueryParamDlg::bindSql() const
{
    QVariantMap result;
    QMapIterator<QString, QVariant> i(m_RefSql);
    while (i.hasNext()) {
        i.next();
        result.insert(i.key(), i.value());
    }
    return result;
}

/******************************************************************/

void QueryParamDlg::setBindTypes(const QVariantMap &map)
{
    QMapIterator<QString, QVariant> i(map);
    while (i.hasNext()) {
        i.next();
        m_BndTypes.insert(i.key(), i.value());
    }
}

/******************************************************************/

QVariantMap QueryParamDlg::bindTypes() const
{
    QVariantMap result;
    QMapIterator<QString, QVariant> i(m_BndTypes);
    while (i.hasNext()) {
        i.next();
        result.insert(i.key(), i.value());
    }
    return result;
}

/******************************************************************/

void QueryParamDlg::setupParams(const QStringList &params)
{
    for (QString param : params) {
        const int row = ui_Grid->rowCount();
        ui_Grid->addWidget(new QLabel(param, this), row, NameColumn);
        QComboBox *cmbType = createCmb(param, row, this);
        ui_Grid->addWidget(cmbType, row, TypeColumn);
        QWidget *editor = createValueEditor(param, this);
        ui_Grid->addWidget(editor, row, ValueColumn);
    }
}

/******************************************************************/

QVariantMap QueryParamDlg::bindings(const QStringList &params) const
{
    QVariantMap result;
    for (auto param : params) {
        QVariant value = getValue(param);
        result.insert(param, value);
    }
    return result;
}

/******************************************************************/

void QueryParamDlg::setDefaults(const QVariantMap &map)
{
    QMapIterator<QString, QVariant> i(map);
    while (i.hasNext()) {
        i.next();
        setValue(i.key(), i.value());
    }
}

/******************************************************************/

void QueryParamDlg::fillReference(QComboBox *cmb, const QString &param)
{
    QString sql = m_RefSql.value(param).toString();
    bool ok = false;
    bool done = Report::fillSqlRef(cmb, sql, m_Db->db);
    while (!done) {
        sql = QInputDialog::getMultiLineText(this, tr("Reference"), tr("SQL"), sql, &ok);
        if (!ok) break; // canceled
        if (sql.isEmpty()) {
            QMessageBox::warning(this, tr("Reference"),
                                 tr("Error in SQL\nSQL text is Empty"));
        } else {
            QString err;
            done = Report::fillSqlRef(cmb, sql,  m_Db->db, &err);
            if (done) {
                m_RefSql.insert(param, sql);
            }
            if (!err.isEmpty()) {
                QMessageBox::warning(this, tr("Reference"),
                                     tr("Error in SQL\n%1").arg(err));
            }
        }
    }
}

/******************************************************************/

void QueryParamDlg::setupUI()
{
    setWindowTitle(tr("Enter binds"));

    ui_Grid = new QGridLayout();
    ui_Grid->addWidget(new QLabel(tr("Name"), this), 0, NameColumn);
    ui_Grid->addWidget(new QLabel(tr("Type"), this), 0, TypeColumn);
    ui_Grid->addWidget(new QLabel(tr("Value"), this), 0, ValueColumn);

    QDialogButtonBox *buttonBox = new QDialogButtonBox(this);
    buttonBox->setOrientation(Qt::Horizontal);
    buttonBox->setStandardButtons(QDialogButtonBox::Cancel|QDialogButtonBox::Ok);

    QVBoxLayout *mainLayout = new QVBoxLayout(this);
    mainLayout->addLayout(ui_Grid);
    mainLayout->addWidget(buttonBox);

    QObject::connect(buttonBox, &QDialogButtonBox::accepted,
                     this, &QDialog::accept);
    QObject::connect(buttonBox, &QDialogButtonBox::rejected,
                     this, &QDialog::reject);

}

/******************************************************************/

QComboBox *QueryParamDlg::createCmb(const QString &param, int row, QWidget *parent)
{
    static QStringList items = QStringList()
            << tr("Null")
            << tr("String")
            << tr("Integer")
            << tr("Real")
            << tr("Date")
            << tr("Date and Time")
            << tr("Reference");
    auto typeSelected = m_BndTypes.value(param, Report::ParamType::String).toInt();

    QComboBox *result = new QComboBox(parent);
    result->setObjectName(comboName(param));
    result->addItems(items);   
    result->setCurrentIndex(typeSelected);

    connect(result, QOverload<int>::of(&QComboBox::currentIndexChanged), [this, param, row](int type){
        m_BndTypes.insert(param, type);
        updateValueEditor(param, row);
    });

    return result;
}

/******************************************************************/

void QueryParamDlg::updateValueEditor(const QString &param, int row)
{
    QString objName = editorName(param);
    QWidget *editor = findChild<QWidget *>(objName);
    QWidget *parent = this;
    if (editor) {
        parent = qobject_cast<QWidget*>(editor->parent());
        editor->deleteLater();
    }
    editor = createValueEditor(param, parent);
    ui_Grid->addWidget(editor, row, ValueColumn);
}

/******************************************************************/

QWidget *QueryParamDlg::createValueEditor(const QString &param, QWidget *parent)
{
    auto type   = m_BndTypes.value(param, Report::ParamType::String).toInt();
    auto editor = Report::createParamEditor(type, editorName(param), parent);

    QComboBox *refCombo = qobject_cast<QComboBox*>(editor);
    if (refCombo) {
        fillReference(refCombo, param);
    }

    return editor;
}

/******************************************************************/

QString QueryParamDlg::editorName(const QString &param) const
{
    QString result(param);
    result.remove(":");
    return result;
}

/******************************************************************/

QString QueryParamDlg::comboName(const QString &param) const
{
    QString result(param);
    result.replace(":","cmb_");
    return result;
}

/******************************************************************/

void QueryParamDlg::setValue(const QString &param, const QVariant &value)
{
    QString objName = editorName(param);
    QWidget *editor = findChild<QWidget *>(objName);
    Report::setParamValue(editor, value);
}

/******************************************************************/

QVariant QueryParamDlg::getValue(const QString &param) const
{
    QString objName = editorName(param);
    QWidget *editor = findChild<QWidget *>(objName);
    return Report::paramValue(editor);
}

/******************************************************************/
