#ifndef KDSIMPLEREPORT_H
#define KDSIMPLEREPORT_H

#include "simplereport.h"

#include <QDomDocument>

class KdSimpleReport : public SimpleReport
{
    Q_OBJECT
public:
    explicit KdSimpleReport(QObject *parent = nullptr);

    void toPreview() const;
};

#endif // KDSIMPLEREPORT_H
