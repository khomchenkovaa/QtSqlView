#include "QueryParamDlg.h"

#include "dbconnection.h"

#include <QVariant>

#include <QGridLayout>
#include <QLabel>
#include <QComboBox>
#include <QLineEdit>
#include <QSpinBox>
#include <QDoubleSpinBox>
#include <QDateEdit>
#include <QDateTimeEdit>
#include <QDialogButtonBox>
#include <QVBoxLayout>

#include <QInputDialog>
#include <QMessageBox>

#include <QSqlQuery>
#include <QSqlError>

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
    bool done = fillSqlRef(cmb, sql);
    while (!done) {
        sql = QInputDialog::getMultiLineText(this, tr("Reference"), tr("SQL"), sql, &ok);
        if (!ok) break; // canceled
        if (sql.isEmpty()) {
            QMessageBox::warning(this, tr("Reference"),
                                 tr("Error in SQL\nSQL text is Empty"));
        } else {
            QString err;
            done = fillSqlRef(cmb, sql, &err);
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
    Type typeSelected = static_cast<Type>(m_BndTypes.value(param, Type::String).toInt());

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
    QWidget *result = Q_NULLPTR;
    QSizePolicy sizePolicy(QSizePolicy::Maximum, QSizePolicy::Fixed);
    sizePolicy.setHorizontalStretch(0);
    sizePolicy.setVerticalStretch(0);
    Type type = static_cast<Type>(m_BndTypes.value(param, Type::String).toInt());

    switch (type) {
    case Type::Null:
        result = new QLabel("NULL", parent);
        break;
    case Type::String:
        result = new QLineEdit(parent);
        break;
    case Type::Integer:
        result = new QSpinBox(parent);
        qobject_cast<QSpinBox*>(result)->setMaximum(std::numeric_limits<int>::max());
        sizePolicy.setHeightForWidth(result->sizePolicy().hasHeightForWidth());
        result->setSizePolicy(sizePolicy);
        break;
    case Type::Real:
        result = new QDoubleSpinBox(parent);
        qobject_cast<QDoubleSpinBox*>(result)->setMaximum(std::numeric_limits<double>::max());
        sizePolicy.setHeightForWidth(result->sizePolicy().hasHeightForWidth());
        result->setSizePolicy(sizePolicy);
        break;
    case Type::Date:
        result = new QDateEdit(QDate::currentDate(), parent);
        sizePolicy.setHeightForWidth(result->sizePolicy().hasHeightForWidth());
        result->setSizePolicy(sizePolicy);
        break;
    case Type::DateTime:
        result = new QDateTimeEdit(QDateTime::currentDateTime(), parent);
        sizePolicy.setHeightForWidth(result->sizePolicy().hasHeightForWidth());
        result->setSizePolicy(sizePolicy);
        break;
    case Type::Ref:
        result = new QComboBox(parent);
        sizePolicy.setHeightForWidth(result->sizePolicy().hasHeightForWidth());
        result->setSizePolicy(sizePolicy);
        fillReference(qobject_cast<QComboBox*>(result), param);
        break;
    default:
        result = new QWidget(parent);
    }
    result->setObjectName(editorName(param));
    return result;
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

bool QueryParamDlg::fillSqlRef(QComboBox *cmb, const QString &sql, QString *err)
{
    if (!cmb) return false;
    if (sql.isEmpty()) return false;

    const QSignalBlocker blocker(cmb);
    cmb->clear();

    QSqlQuery query(sql, m_Db->db);
    if (query.isSelect()) {
        while (query.next()) {
            cmb->addItem(query.value(1).toString(), query.value(0));
        }
        return true;
    }

    if (query.lastError().type() != QSqlError::NoError && err) {
        err->append(query.lastError().text());
    }
    return false;
}

/******************************************************************/

void QueryParamDlg::setValue(const QString &param, const QVariant &value)
{
    QString objName = editorName(param);
    QWidget *editor = findChild<QWidget *>(objName);
    if (!editor) return;

    QComboBox *cmbRef = qobject_cast<QComboBox*>(editor);
    if (cmbRef) {
        int idx = cmbRef->findData(value);
        cmbRef->setCurrentIndex(idx);
    }

    QLineEdit *stringEdit = qobject_cast<QLineEdit*>(editor);
    if (stringEdit) {
        stringEdit->setText(value.toString());
    }

    QSpinBox *intEdit = qobject_cast<QSpinBox*>(editor);
    if (intEdit) {
        intEdit->setValue(value.toInt());
    }

    QDoubleSpinBox *realEdit = qobject_cast<QDoubleSpinBox*>(editor);
    if (realEdit) {
        realEdit->setValue(value.toDouble());
    }

    QDateEdit *dateEdit = qobject_cast<QDateEdit*>(editor);
    if (dateEdit) {
        dateEdit->setDate(value.toDate());
    }

    QDateTimeEdit *dateTimeEdit = qobject_cast<QDateTimeEdit*>(editor);
    if (dateTimeEdit) {
        dateTimeEdit->setDateTime(value.toDateTime());
    }
}

/******************************************************************/

QVariant QueryParamDlg::getValue(const QString &param) const
{
    QString objName = editorName(param);
    QWidget *editor = findChild<QWidget *>(objName);
    if (!editor) return QVariant();

    QComboBox *cmbRef = qobject_cast<QComboBox*>(editor);
    if (cmbRef) {
        return cmbRef->currentData();
    }

    QLineEdit *stringEdit = qobject_cast<QLineEdit*>(editor);
    if (stringEdit) {
        return stringEdit->text();
    }

    QSpinBox *intEdit = qobject_cast<QSpinBox*>(editor);
    if (intEdit) {
        return intEdit->value();
    }

    QDoubleSpinBox *realEdit = qobject_cast<QDoubleSpinBox*>(editor);
    if (realEdit) {
        return realEdit->value();
    }

    QDateEdit *dateEdit = qobject_cast<QDateEdit*>(editor);
    if (dateEdit) {
        return dateEdit->date();
    }

    QDateTimeEdit *dateTimeEdit = qobject_cast<QDateTimeEdit*>(editor);
    if (dateTimeEdit) {
        return dateTimeEdit->dateTime();
    }

    return QVariant();
}

/******************************************************************/
