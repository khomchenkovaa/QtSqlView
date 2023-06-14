#ifndef QUERYPARAMDLG_H
#define QUERYPARAMDLG_H

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
    explicit QueryParamDlg(const QStringList &params, DbConnection *dbc, QWidget *parent = nullptr);
    ~QueryParamDlg();

    QVariantMap bindings() const;

private Q_SLOTS:
    void fillReference(QComboBox *cmb);

private:
    void setupUI();
    QComboBox *createCmb(const QString &param, int row, QWidget *parent = nullptr);
    void updateValueEditor(const QString &param, int row, Type type);
    QWidget *createValueEditor(const QString &param, Type type, QWidget *parent = nullptr);
    QString editorName(const QString &param) const;
    QVariant getValue(const QString &param) const;

private:
    QStringList m_Params;
    QGridLayout *ui_Grid;
    DbConnection *m_Db;
};

#endif // QUERYPARAMDLG_H
