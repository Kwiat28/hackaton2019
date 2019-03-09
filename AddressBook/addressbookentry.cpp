#include "addressbookentry.h"

AddressBookEntry::AddressBookEntry(QObject *parent) : QObject(parent)
{

}

QString AddressBookEntry::name() const
{
    return m_name;
}

void AddressBookEntry::setName(const QString &name)
{
    if (m_name !=name){
    m_name = name;
    emit nameChanged();
    }
}

QString AddressBookEntry::address() const
{
    return m_address;
}

void AddressBookEntry::setAddress(const QString &address)
{
    if (m_address != address){
    m_address = address;
    emit addressChanged();
    }
}

QDate AddressBookEntry::birthday() const
{
    return m_birthday;
}

void AddressBookEntry::setBirthday(const QDate &birthday)
{
    if(m_birthday != birthday){
    m_birthday = birthday;
    emit birthdayChanged();
    }
}

QStringList AddressBookEntry::phoneNumbers() const
{
    return m_phoneNumbers;
}

void AddressBookEntry::setPhoneNumbers(const QStringList &phoneNumbers)
{
    if(m_phoneNumbers != phoneNumbers){
    m_phoneNumbers = phoneNumbers;
    emit phoneNumbersChanged();
    }
}
