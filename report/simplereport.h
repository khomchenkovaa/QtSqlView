#ifndef SIMPLEREPORT_H
#define SIMPLEREPORT_H

#include "listreport.h"

class SimpleReport : public ListReport
{
    Q_OBJECT
public:
    explicit SimpleReport(QObject *parent = nullptr);

    void setTitle(const QString& title);
    void setHeader(const QString& header);
    void setFooter(const QString& footer);

    void printTo(QTextCursor *cursor) const;

private:
    QString m_Title;
    QString m_Header;
    QString m_Footer;
};

#endif // SIMPLEREPORT_H
