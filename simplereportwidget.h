#ifndef SIMPLEREPORTWIDGET_H
#define SIMPLEREPORTWIDGET_H

#include <QWidget>

QT_BEGIN_NAMESPACE
class QSqlQueryModel;
QT_END_NAMESPACE

namespace Ui {
class SimpleReportWidget;
}

class SimpleReportWidget : public QWidget
{
    Q_OBJECT

public:
    explicit SimpleReportWidget(QWidget *parent = nullptr);
    ~SimpleReportWidget();

    void setUserQueryModel(QSqlQueryModel *model);
    void updateView();

Q_SIGNALS:
    void tableHeaders();

private Q_SLOTS:
    void printReport();
    void exportToPdf();
    void clearFields();

private:
    void setupUI();

private:
    Ui::SimpleReportWidget *ui;
    QSqlQueryModel *m_Model;
};

#endif // SIMPLEREPORTWIDGET_H
