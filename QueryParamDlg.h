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

    enum Type {
        Null, String, Integer, Real, Date, DateTime, Ref
    };

public:
    explicit QueryParamDlg(DbConnection *dbc, QWidget *parent = nullptr);
    ~QueryParamDlg();

    void setBindSql(const QVariantMap &sqlRef);
    QVariantMap bindSql() const;

    void setBindTypes(const QVariantMap &map);
    QVariantMap bindTypes();

    void setupParams(const QStringList& params);
    QVariantMap bindings(const QStringList& params) const;

private Q_SLOTS:
    void fillReference(QComboBox *cmb, const QString& param);

private:
    void setupUI();
    QComboBox *createCmb(const QString &param, int row, QWidget *parent = Q_NULLPTR);
    void updateValueEditor(const QString &param, int row, Type type);
    QWidget *createValueEditor(const QString &param, Type type, QWidget *parent = Q_NULLPTR);
    QString editorName(const QString &param) const;
    QString comboName(const QString &param) const;
    bool fillSqlRef(QComboBox *cmb, const QString& sql, QString *err = Q_NULLPTR);
    QVariant getValue(const QString &param) const;

private:
    QVariantMap   m_BndTypes;
    QVariantMap   m_RefSql;
    QGridLayout  *ui_Grid;
    DbConnection *m_Db;
};

#endif // QUERYPARAMDLG_H
