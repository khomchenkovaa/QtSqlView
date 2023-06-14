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

QueryParamDlg::QueryParamDlg(const QStringList &params, DbConnection *dbc, QWidget *parent) :
    QDialog(parent)
{
    m_Params = params;
    m_Db = dbc;
    setupUI();
}

/******************************************************************/

QueryParamDlg::~QueryParamDlg()
{

}

/******************************************************************/

QVariantMap QueryParamDlg::bindings() const
{
    QVariantMap result;
    for (auto param : m_Params) {
        QVariant value = getValue(param);
        result.insert(param, value);
    }
    return result;
}

/******************************************************************/

void QueryParamDlg::fillReference(QComboBox *cmb)
{
    const QSignalBlocker blocker(cmb);
    cmb->clear();

    QString sql;
    bool ok = false;
    bool done = false;
    while (!done) {
        sql = QInputDialog::getMultiLineText(this, tr("Reference"), tr("SQL"), sql, &ok);
        if (!ok) break; // canceled
        if (sql.isEmpty()) {
            QMessageBox::warning(this, tr("Reference"),
                                 tr("Error in SQL\nSQL text is Empty"));

        } else {
            QSqlQuery query(sql, m_Db->db);
            if (query.isSelect()) {
                while (query.next()) {
                    cmb->addItem(query.value(1).toString(), query.value(0));
                }
                done = true;
            } else {
                 if (query.lastError().type() != QSqlError::NoError) {
                    QMessageBox::warning(this, tr("Reference"),
                                         tr("Error in SQL\n%1").arg(query.lastError().text()));
                }
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
    for (QString param : m_Params) {
        const int row = ui_Grid->rowCount();
        ui_Grid->addWidget(new QLabel(param, this), row, NameColumn);
        QComboBox *cmbType = createCmb(param, row, this);
        ui_Grid->addWidget(cmbType, row, TypeColumn);
        QWidget *editor = createValueEditor(param, Type::String, this);
        ui_Grid->addWidget(editor, row, ValueColumn);
    }

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
    QComboBox *result = new QComboBox(parent);
    result->addItems(items);
    result->setCurrentIndex(Type::String);

    connect(result, QOverload<int>::of(&QComboBox::currentIndexChanged), [this, param, row](int type){
        updateValueEditor(param, row, static_cast<Type>(type));
    });

    return result;
}

/******************************************************************/

void QueryParamDlg::updateValueEditor(const QString &param, int row, Type type)
{
    QString objName = editorName(param);
    QWidget *editor = findChild<QWidget *>(objName);
    QWidget *parent = this;
    if (editor) {
        parent = qobject_cast<QWidget*>(editor->parent());
        editor->deleteLater();
    }
    editor = createValueEditor(param, type, parent);
    ui_Grid->addWidget(editor, row, ValueColumn);
}

/******************************************************************/

QWidget *QueryParamDlg::createValueEditor(const QString &param, Type type, QWidget *parent)
{
    QWidget *result = Q_NULLPTR;
    QSizePolicy sizePolicy(QSizePolicy::Maximum, QSizePolicy::Fixed);
    sizePolicy.setHorizontalStretch(0);
    sizePolicy.setVerticalStretch(0);

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
        fillReference(qobject_cast<QComboBox*>(result));
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
