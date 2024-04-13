#ifndef QUERYPARAMDLG_H
#define QUERYPARAMDLG_H

#include <QVariantMap>
#include <QDialog>

QT_BEGIN_NAMESPACE
class QComboBox;
class QGridLayout;
QT_END_NAMESPACE

class DbConnection;

class QueryParamDlg : public QDialog
{
    Q_OBJECT

    struct QueryParamDlgPrivate {
        QVariantMap   bindTypes;
        QVariantMap   bindSql;
        DbConnection *dbConn = Q_NULLPTR;
    };

public:
    explicit QueryParamDlg(DbConnection *dbc, QWidget *parent = nullptr);

    void setBindSql(const QVariantMap &sqlRef);
    QVariantMap bindSql() const;

    void setBindTypes(const QVariantMap &map);
    QVariantMap bindTypes() const;

    void setupParams(const QStringList& params);
    QVariantMap bindings(const QStringList& params) const;

    void setDefaults(const QVariantMap &map);

private Q_SLOTS:
    void fillReference(QComboBox *cmb, const QString& param);

private:
    void setupUI();
    QComboBox *createCmb(const QString &param, int row, QWidget *parent = Q_NULLPTR);
    void updateValueEditor(const QString &param, int row);
    QWidget *createValueEditor(const QString &param, QWidget *parent = Q_NULLPTR);
    QString editorName(const QString &param) const;
    QString comboName(const QString &param) const;
    void setValue(const QString &param, const QVariant& value);
    QVariant getValue(const QString &param) const;

private:
    QGridLayout  *ui_Grid;
    QueryParamDlgPrivate d;
};

#endif // QUERYPARAMDLG_H
