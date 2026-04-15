#include "mainwindow.h"
#include "./ui_mainwindow.h"
#include "sqlmanagerwrapper.h"
MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    wapper = new SqlManagerWrapper(this);
    this->DbHost = "localhost";
    this->DbPassword = "";
    this->DbPort = 3306;
    this->DbUsername = "root";
    connect(ui->HostlineEdit_2,
            &QLineEdit::textChanged,
            this,
            &MainWindow::setDbHost);
    connect(ui->usernameLineEdit_2,
            &QLineEdit::textChanged,
            this,
            &MainWindow::setDbUsrname);
    connect(ui->PortLineEdit,
            &QLineEdit::textChanged,
            this,
            &MainWindow::setDbPort);
    connect(ui->passwordLineEdit_2,
            &QLineEdit::textChanged,
            this,
            &MainWindow::setDbPassword);
    connect(ui->DbConnectionButton,
            &QAbstractButton::clicked,
            this,
            &MainWindow::Connect_to_SQL);
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::setDbHost()
{
    this->DbHost = ui->HostlineEdit_2->text();
}

void MainWindow::setDbUsrname()
{
    this->DbUsername = ui->usernameLineEdit_2->text();
}

void MainWindow::setDbPassword()
{
    this->DbPassword = ui->passwordLineEdit_2->text();
}

void MainWindow::setDbPort()
{
    this->DbPort = ui->PortLineEdit->text().toInt();
}

void MainWindow::Connect_to_SQL()
{
    bool T = wapper->connect(this->DbHost,
                    this->DbUsername,
                    this->DbPassword,
                    "",
                    this->DbPort);

    if(T)
    {
        ui->plainTextEdit->insertPlainText("Successful");
    }
    else
    {
        ui->plainTextEdit->insertPlainText("Unsuccessful");
    }
}