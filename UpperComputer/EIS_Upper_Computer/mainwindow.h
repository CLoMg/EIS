#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QSerialPort>
#include "QString"

QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();
    QSerialPort *serialPort;    //定义串口接口变量

private slots:
    void on_pushButton_refresh_clicked();

    void on_pushButton_open_clicked();

    void on_pushButton_close_clicked();

    void on_pushButton_send_clicked();
    _Bool Rx_Text_ReadyData_Slot();

    void on_txform_ASCII_clicked();

    void on_txform_HEX_clicked();

    void on_pushButton_rxclear_clicked();

    void on_pushButton_txclear_clicked();

    void on_pushButton_parseclear_clicked();

    void on_pushButton_LightChg_clicked();

    void on_pushButton_datasave_clicked();

    void on_pushButton_typeset_Gas_clicked();

    void on_pushButton_datasave_GAS_clicked();


    void on_pushButton_motor_clicked();

private:
    Ui::MainWindow *ui;
};
#endif // MAINWINDOW_H
