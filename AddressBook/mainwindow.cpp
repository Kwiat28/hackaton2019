#include "mainwindow.h"
#include "ui_mainwindow.h"

#include<QPushButton>
MainWindow::MainWindow(AddressBookController *controller, QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow),
    m_controller(controller)
{
    Q_ASSERT(controller !=nullptr);
    ui->setupUi(this);

    setupConnections();
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::createEntry()
{
    auto entry = m_controller->createEntry();
    if(entry){
        ui->listWidget->addItem(entry->name());
        auto listItem = ui -> listWidget->item(ui->listWidget->count() -1);
        m_entryMap.insert(listItem, entry);
    }
}

void MainWindow::deleteEntry()
{
    auto listItem = ui->listWidget->currentItem();
    if(listItem){
       auto entry = m_entryMap.value(listItem);
       if(entry){
        if(m_controller->deleteEntry(entry)){
            m_entryMap.remove(listItem);
            delete listItem;
        }
       }
    }
}

void MainWindow::editEntry()
{
    auto listItem = ui->listWidget->currentItem();
    if(listItem){
       auto entry = m_entryMap.value(listItem);
       if(entry){
        ui->stackedWidget->setCurrentWidget(ui->detailPage);
        ui->menuEntries->setEnabled(false);
        resetEntry();

       }
    }
}

void MainWindow::saveEntry()
{
    auto listItem = ui->listWidget->currentItem();
    if(listItem){
       auto entry = m_entryMap.value(listItem);
       if(entry){
        entry->setName(ui->nameEdit->text());
        entry->setBirthday(ui->birthdayEdit->date());
        entry->setAddress(ui->addressEdit->toPlainText());
        entry->setPhoneNumbers(ui->phoneNumbersEdit->toPlainText().split("\n"));
        listItem->setText(entry->name());
        //listItem->setText(entry->phoneNumbers());
        discardEntry();

       }
    }
}

void MainWindow::discardEntry()
{
    ui->stackedWidget->setCurrentWidget(ui->listPage);
    ui->menuEntries->setEnabled(true);
}

void MainWindow::resetEntry()
{
    auto listItem = ui->listWidget->currentItem();
    if(listItem){
       auto entry = m_entryMap.value(listItem);
       if(entry){
        ui->nameEdit->setText(entry->name());
        ui->birthdayEdit->setDate(entry->birthday());
        ui->addressEdit->setPlainText(entry->address());
        ui->phoneNumbersEdit->setPlainText(entry->phoneNumbers().join("\n"));

       }
    }
}

void MainWindow::setupConnections()
{
    connect(ui->actionAdd, SIGNAL(triggered(bool)),this,
            SLOT(createEntry()));
    connect(ui->actionRemove, &QAction::triggered,
            this, &MainWindow::deleteEntry);
    connect(ui->actionEdit, &QAction::triggered,
            this, &MainWindow::editEntry);
    connect(ui->buttonBox->button(QDialogButtonBox::Save), &QPushButton::clicked,
            this, &MainWindow::saveEntry);
    connect(ui->buttonBox->button(QDialogButtonBox::Discard), &QPushButton::clicked,
            this, &MainWindow::discardEntry);
    connect(ui->buttonBox->button(QDialogButtonBox::Reset), &QPushButton::clicked,
            this, &MainWindow::resetEntry);

}
