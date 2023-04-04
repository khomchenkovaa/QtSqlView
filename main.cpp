#include "MainWindow.h"

#include <QApplication>

#define ORGANIZATION_NAME   "SNIIP"
#define ORGANIZATION_DOMAIN "sniip.ru"
#define APPLICATION_NAME    "QtSqlView"

int main(int argc, char **argv)
{
    QCoreApplication::setOrganizationName(ORGANIZATION_NAME);
    QCoreApplication::setOrganizationDomain(ORGANIZATION_DOMAIN);
    QCoreApplication::setApplicationName(APPLICATION_NAME);

    QApplication a( argc, argv );

    MainWindow mainWindow;
    mainWindow.show();
    a.connect( &a, SIGNAL(lastWindowClosed()), &a, SLOT(quit()) );

    return a.exec();
}
