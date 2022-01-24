#ifndef SERIAL_H
#define SERIAL_H

#include <QMainWindow>

/*---------user---------------*/
#include <QSerialPort>
#include<QSerialPortInfo>
//debug
#include <QDebug>
/*------------------------*/


QT_BEGIN_NAMESPACE
namespace Ui { class Serial; }
QT_END_NAMESPACE

class Serial : public QMainWindow
{
    Q_OBJECT

public:
    Serial(QWidget *parent = nullptr);
    ~Serial();


private slots:
    //button
    void btn_open_port(bool);
    void btn_close_port(bool);
    void btn_scan_port(bool);
    void btn_send_data(bool);
    void btn_clear_data(bool);
    void btn_clear2_data(bool);

    //data receive
    void receive_data();

private:
    Ui::Serial *ui;
    /*------function--------*/
    void system_init();


    /*------variable--------*/

    QSerialPort global_port;

};
#endif // SERIAL_H
