#include "TableHeadersDlg.h"

#include <QVariant>

#include <QtWidgets/QGridLayout>
#include <QtWidgets/QLabel>
#include <QtWidgets/QComboBox>
#include <QtWidgets/QLineEdit>
#include <QtWidgets/QSpinBox>
#include <QtWidgets/QDoubleSpinBox>
#include <QtWidgets/QDateEdit>
#include <QtWidgets/QDateTimeEdit>
#include <QtWidgets/QDialogButtonBox>
#include <QtWidgets/QHBoxLayout>

enum {
    FieldColumn,
    TitleColumn
};

/******************************************************************/

TableHeadersDlg::TableHeadersDlg(const QStringList &fields, QWidget *parent) :
    QDialog(parent)
{
    m_Fields = fields;
    setupUI();
}

/******************************************************************/

void TableHeadersDlg::setHeaders(const QStringList &headers)
{
    for (int i=0; i < headers.size(); ++i) {
        if (i < m_Fields.size()) {
            setValue(m_Fields.at(i), headers.at(i));
        }
    }
}

/******************************************************************/

QStringList TableHeadersDlg::headers() const
{
    QStringList result;
    for (const auto &field : m_Fields) {
         result << getValue(field);
    }
    return result;
}

/******************************************************************/

void TableHeadersDlg::setupUI()
{
    setWindowTitle(tr("Edit column titles"));

    ui_Grid = new QGridLayout();
    ui_Grid->setVerticalSpacing(2);
    ui_Grid->addWidget(new QLabel(tr("Field"), this), 0, FieldColumn);
    ui_Grid->addWidget(new QLabel(tr("Title"), this), 0, TitleColumn);
    for (const auto &field : qAsConst(m_Fields)) {
        const int row = ui_Grid->rowCount();
        ui_Grid->addWidget(new QLabel(field, this), row, FieldColumn);
        QWidget *editor = createValueEditor(field, this);
        ui_Grid->addWidget(editor, row, TitleColumn);
    }

    auto buttonBox = new QDialogButtonBox(this);
    buttonBox->setOrientation(Qt::Vertical);
    buttonBox->setStandardButtons(QDialogButtonBox::Cancel|QDialogButtonBox::Ok);

    auto mainLayout = new QHBoxLayout(this);
    mainLayout->addLayout(ui_Grid);
    mainLayout->addWidget(buttonBox);

    QObject::connect(buttonBox, &QDialogButtonBox::accepted,
                     this, &QDialog::accept);
    QObject::connect(buttonBox, &QDialogButtonBox::rejected,
                     this, &QDialog::reject);

}

/******************************************************************/

QWidget *TableHeadersDlg::createValueEditor(const QString &param, QWidget *parent)
{
    QWidget *result = new QLineEdit(parent);
    result->setObjectName(param);
    return result;
}

/******************************************************************/

QString TableHeadersDlg::getValue(const QString &param) const
{
    QLineEdit *stringEdit = findChild<QLineEdit *>(param);
    if (stringEdit) {
        return stringEdit->text();
    }

    return QString();
}

/******************************************************************/

void TableHeadersDlg::setValue(const QString &param, const QString &value)
{
    QLineEdit *stringEdit = findChild<QLineEdit *>(param);
    if (stringEdit) {
        return stringEdit->setText(value);
    }
}

/******************************************************************/
