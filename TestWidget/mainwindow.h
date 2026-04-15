#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include "sqlmanagerwrapper.h"
QT_BEGIN_NAMESPACE
namespace Ui {
class MainWindow;
}
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = nullptr);
    ~MainWindow() override;

    void setDbHost();
    void setDbUsrname();
    void setDbPort();
    void setDbPassword();

    void Connect_to_SQL();

    SqlManagerWrapper* wapper;

private:
    Ui::MainWindow *ui;
    QString DbHost ;
    QString DbUsername;
    int DbPort;
    QString DbPassword;
};
#endif // MAINWINDOW_H
