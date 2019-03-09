#include "mainwindow.h"
#include <QApplication>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    AddressBook addressBook;
    AddressBookController controller (&addressBook);
    MainWindow w(&controller);
    w.show();

    return a.exec();
}
