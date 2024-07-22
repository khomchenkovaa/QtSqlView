#ifndef TABLEHEADERSDLG_H
#define TABLEHEADERSDLG_H

#include <QDialog>

QT_BEGIN_NAMESPACE
class QComboBox;
class QGridLayout;
QT_END_NAMESPACE

class TableHeadersDlg : public QDialog
{
    Q_OBJECT

    enum Type {
        String, Integer, Real, Date, DateTime
    };

public:
    explicit TableHeadersDlg(const QStringList &fields, QWidget *parent = nullptr);

    void setHeaders(const QStringList &headers);
    QStringList headers() const;

private:
    void setupUI();
    QWidget *createValueEditor(const QString &param, QWidget *parent = nullptr);
    QString getValue(const QString &param) const;
    void setValue(const QString &param, const QString &value);

private:
    QStringList m_Fields;
    QGridLayout *ui_Grid;
};

#endif // TABLEHEADERSDLG_H
