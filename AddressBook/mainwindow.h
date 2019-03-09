#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include "addressbookcontroller.h"
#include <QMainWindow>
#include <QHash>

namespace Ui {
class MainWindow;
}

class QListWidgetItem;

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(AddressBookController *controller, QWidget *parent = 0);
    ~MainWindow();

public slots:

    void createEntry();
    void deleteEntry();
    void editEntry();
    void saveEntry();
    void discardEntry();
    void resetEntry();

private:
    Ui::MainWindow *ui;
    AddressBookController *m_controller;
    QHash<QListWidgetItem*, AddressBookEntry*> m_entryMap;
    void setupConnections();
};

#endif // MAINWINDOW_H
