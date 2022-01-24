#include "serial.h"
#include "ui_serial.h"
#include <algorithm>

Serial::Serial(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::Serial)
{
    ui->setupUi(this);

    //user
    system_init();
}

Serial::~Serial()
{
    delete ui;
}


/*--------------------------------------------------------------
 *      functions
 * ------------------------------------------------------------*/
void Serial::system_init()
{
    //port_config
    global_port.setParity(QSerialPort::NoParity);
    global_port.setDataBits(QSerialPort::Data8);
    global_port.setStopBits(QSerialPort::OneStop);

    //connect(信号发出者地址，发什么信号，在哪个类触发（地址），槽函数)
    //BUTTON
    connect(ui->btn_scan, &QPushButton::clicked,this, &Serial::btn_scan_port );
    connect(ui->btn_open, &QPushButton::clicked,this, &Serial::btn_open_port );
    connect(ui->btn_close, &QPushButton::clicked,this, &Serial::btn_close_port );
    connect(ui->btn_send, &QPushButton::clicked,this, &Serial::btn_send_data );
    connect(ui->btn_clear, &QPushButton::clicked,this, &Serial::btn_clear_data );       //clear received data
    connect(ui->btn_clear2, &QPushButton::clicked,this, &Serial::btn_clear2_data );     //clear send data
    //SIGNAL
    connect(&global_port, &QSerialPort::readyRead,this, &Serial::receive_data );        //recieve dada
   // connect(&global_port, &QSerialPort::canReadLine,this, &Serial::receive_data );        //recieve dada

}


/*--------------------------------------------------------------
 *      slots
 * ------------------------------------------------------------*/


/*------------------scan available ports-------------------------*/

void Serial::btn_scan_port(bool)
{
    ui->cmb_port_name->clear();
    foreach (const QSerialPortInfo &info, QSerialPortInfo::availablePorts()){
//        qDebug() << "Name        : " << info.portName();
//        qDebug() << "Description : " << info.description();
//        qDebug() << "Manufacturer: " << info.manufacturer()<<endl;
        ui->cmb_port_name->addItem(info.portName()+" "+info.description());
    }
}


/*----------------open port and initiatate----------------------*/

void Serial:: btn_open_port(bool)
{
    /*--------------------port name----------------------------*/
    QString selectedPort = ui->cmb_port_name->currentText();
    global_port.setPortName(selectedPort.mid(0,5));

    /*--------------------baud rate----------------------------*/
    switch (ui->cmb_baud_rate->currentIndex()) {
    case 0:
        global_port.setBaudRate(QSerialPort::Baud1382400);      //Qt默认最高定义了115200，更高需要进库函数中添加
        break;
    case 1:
        global_port.setBaudRate(QSerialPort::Baud256000);
        break;
    case 2:
        global_port.setBaudRate(QSerialPort::Baud115200);
        break;
    case 3:
        global_port.setBaudRate(QSerialPort::Baud57600);
        break;
    case 4:
        global_port.setBaudRate(QSerialPort::Baud38400);
        break;
    case 5:
        global_port.setBaudRate(QSerialPort::Baud19200);
        break;
    case 6:
        global_port.setBaudRate(QSerialPort::Baud9600);
        break;
    default :
        global_port.setBaudRate(QSerialPort::Baud4800);
        break;
    }

    //open
    if(global_port.open(QIODevice::ReadWrite))
        ui->lab_status->setText("Connected");
    else
        ui->lab_status->setText("Error");
}



/*-----------------------close port------------------------------*/
void Serial::btn_close_port(bool)
{
    ui->lab_status->setText("Disconnected");
    global_port.close();
}

/*----------------send dada--------------------------------------*/
void Serial::btn_send_data(bool)
{
    QString str_input = ui->plainTextEdit->toPlainText();

    if(ui->cbx_flip->checkState()== Qt::Checked ){
        std::reverse(str_input.begin(),str_input.end());  //翻转字符串
    }

    if(ui->cbx_frame->checkState() == Qt::Checked){ //帧控制打开
        QString str_head = ui->pte_head->toPlainText();
        QString str_tail = ui->pte_tail->toPlainText();
        str_input = str_head.append(str_input);
        str_input = str_input.append(str_tail);
    }

//    qDebug() <<data;
    QByteArray ba =  str_input.toLocal8Bit();
    global_port.write(ba );
}

/*----------------clear send dada--------------------------------*/
void Serial::btn_clear2_data(bool)
{
    ui->plainTextEdit->clear();
}

/*----------------clear received dada----------------------------*/
void Serial::btn_clear_data(bool)
{
    ui->textBrowser->clear();
    ui->textBrowser_DEC->clear();
}

/*-----------------receive data----------------------------------*/
//数据缓冲区满后会触发
void Serial::receive_data()
{
    QByteArray array =  global_port.readLine();

    if(ui->cbx_hex->checkState() == Qt::Checked){
        ui->textBrowser->insertPlainText(QString(array.toHex(' ').toUpper()).append(' '));
    }else if(ui->cbx_hex->checkState() == Qt::PartiallyChecked){
        //自定义编码--替换掉ascii-'9'后面的字符成 HEX对应字符
        QString str = QString::fromLocal8Bit(array);//支持中文字符
        str = str.replace(':','A').replace(';','B').replace('<','C').replace('=','D').replace('>','E').replace('?','F') ;
        ui->textBrowser->insertPlainText(str );
        bool ok;
        QString str2 = QString::number(str.mid(0,9).toLongLong(&ok,16))+" "+QString::number(str.mid(9,9).toLongLong(&ok,16))+" "+str.mid(18,1)+"\n";
        ui->textBrowser_DEC->insertPlainText(str2);
        //        qDebug()<<str;
    }
    else{
        QString str = QString::fromLocal8Bit(array);//支持中文字符
        ui->textBrowser->insertPlainText(str);
    }

    ui->textBrowser->moveCursor(QTextCursor::End);
    ui->textBrowser_DEC->moveCursor(QTextCursor::End);
}

