#ifndef ADDRESSBOOKCONTROLLER_H
#define ADDRESSBOOKCONTROLLER_H

#include "addressbook.h"

#include <QObject>

class AddressBookController : public QObject
{
    Q_OBJECT
public:
    explicit AddressBookController(AddressBook *addressBook, QObject *parent = 0);

    AddressBookEntry *createEntry();
    bool deleteEntry (AddressBookEntry *entry);

signals:

public slots:

private:

    AddressBook *m_addressBook;
};

#endif // ADDRESSBOOKCONTROLLER_H
