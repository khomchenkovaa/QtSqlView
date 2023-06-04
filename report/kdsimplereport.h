#ifndef KDSIMPLEREPORT_H
#define KDSIMPLEREPORT_H

#include "simplereport.h"

#include <KDReports>

class KdSimpleReport : public SimpleReport
{
    Q_OBJECT
public:
    explicit KdSimpleReport(QObject *parent = nullptr);

    bool toPrinter() const;
    int  toPreviewDialog() const;
    bool toPdfFile(const QString& fileName) const;
    bool toHtmlFile(const QString& fileName) const;

private:
    void createReport(KDReports::Report* report) const;
};

#endif // KDSIMPLEREPORT_H
