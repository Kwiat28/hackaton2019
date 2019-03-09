#include "addressbook.h"

AddressBook::AddressBook(QObject *parent) : QObject(parent)
{

}

AddressBook::Entries AddressBook::entries() const
{
    return m_entries;
}

AddressBookEntry *AddressBook::createEntry()
{
    auto result = new AddressBookEntry( this );
    if(result != nullptr){
    m_entries.append(result);
    emit entryAdded(result);

    }
    return result;
}

bool AddressBook::deleteEntry(AddressBookEntry *entry)
{
    if (m_entries.contains(entry)){
    emit entryRemoved(entry);
    m_entries.removeOne(entry);
    delete entry;
    return true;
    }
    return false;
}
