#ifndef KDXMLREPORT_H
#define KDXMLREPORT_H

#include "listreport.h"

#include <QDomDocument>

class KdXmlReport : public ListReport
{
    Q_OBJECT
public:
    explicit KdXmlReport(QObject *parent = nullptr);

    bool setXml(const QString& text, QString *errorMsg=nullptr, int *errorLine=nullptr, int *errorColumn=nullptr);

    void toPreview() const;

private:
    QDomDocument m_Xml;
};

#endif // KDXMLREPORT_H
