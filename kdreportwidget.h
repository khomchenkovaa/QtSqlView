#ifndef KDREPORTWIDGET_H
#define KDREPORTWIDGET_H

#include "listreport.h"

#include <QWidget>

QT_BEGIN_NAMESPACE
class QSqlQueryModel;
QT_END_NAMESPACE

namespace Ui {
class KdReportWidget;
}

class KdReportWidget : public QWidget
{
    Q_OBJECT

public:
    explicit KdReportWidget(QWidget *parent = nullptr);
    ~KdReportWidget();

    void setUserQueryModel(QSqlQueryModel *model);
    void updateView();

Q_SIGNALS:
    void tableHeaders();

private Q_SLOTS:
    void print();
    void preview();
    void exportToPdf();
    void exportToHtml();
    void exportToCsv();
    void clearXml();
    void loadXml();
    void saveXml();

private:
    void setupUI();
    void setupActions();
    QSharedPointer<ListReport> createReport();

private:
    Ui::KdReportWidget *ui;
    QSqlQueryModel *m_Model;
};

#endif // KDREPORTWIDGET_H
