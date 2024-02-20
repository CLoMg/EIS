#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "QSerialPortInfo"   //包含串口头文件
#include "QMessageBox"
#include <QRegularExpression>
#include <QFileDialog>
#include <QTimer>
#include <QtEndian>

_Bool txdata_form = 0;
_Bool rxdata_form = 0;
uint16_t checksum_16(QByteArray,uint16_t);
MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    QStringList serialNamePort;             //串口字符列表
    serialPort = new QSerialPort(this);     //实例化对象
    connect(serialPort,SIGNAL(readyRead()),this,SLOT(Rx_Text_ReadyData_Slot()));
    foreach (const QSerialPortInfo &info,QSerialPortInfo::availablePorts()) {   //获取串口信息
        serialNamePort<<info.portName();    //将信息打印到列表中
    }
    ui->box_seriallist->addItems(serialNamePort);//将列表中的数据填充到ui的控件中
    if(ui->rxform_HEX->isChecked())
        rxdata_form = 0;
    else
        rxdata_form = 1;
    if(ui->txform_HEX->isChecked())
        txdata_form = 0;
    else
        txdata_form = 1;
}

MainWindow::~MainWindow()
{
    delete ui;
}


void MainWindow::on_pushButton_rxclear_clicked()
{
    ui->rx_text->clear();
}

//刷新串口列表
void MainWindow::on_pushButton_refresh_clicked()
{
    QStringList serialNamePort;
    ui->box_seriallist->clear(); //清空当前串口列表
    foreach (const QSerialPortInfo &info,QSerialPortInfo::availablePorts()) {   //获取串口信息
        serialNamePort<<info.portName();    //将信息打印到列表中
    }
    ui->box_seriallist->addItems(serialNamePort);//将列表中的数据填充到ui的控件中
}

//打开串口
void MainWindow::on_pushButton_open_clicked()
{
    QSerialPort::BaudRate baudrate_arr[] = {QSerialPort::Baud1200,QSerialPort::Baud2400,QSerialPort::Baud4800,QSerialPort::Baud9600,QSerialPort::Baud19200,QSerialPort::Baud38400,QSerialPort::Baud57600,QSerialPort::Baud115200};
    QSerialPort::DataBits databits_arr[] = {QSerialPort::Data5,QSerialPort::Data6,QSerialPort::Data7,QSerialPort::Data8};
    QSerialPort::StopBits stopbits_arr[] = {QSerialPort::OneStop,QSerialPort::OneAndHalfStop,QSerialPort::TwoStop};
    QSerialPort::Parity   parity_arr[]   = {QSerialPort::NoParity,QSerialPort::EvenParity,QSerialPort::OddParity};
    QSerialPort::BaudRate baudrate;
    QSerialPort::DataBits databits;
    QSerialPort::StopBits stopbits;
    QSerialPort::Parity   parity;

    baudrate = baudrate_arr[ui->box_baudrate->currentIndex()];        //获取窗口串口波特率
    databits = databits_arr[ui->box_databit->currentIndex()];   //获取数据位
    stopbits = stopbits_arr[ui->box_stopbit->currentIndex()];   //获取停止位
    parity   = parity_arr[ui->box_parity->currentIndex()];       //获取奇偶校验位
    serialPort->setPortName(ui->box_seriallist->currentText());          //获取串口号
    serialPort->setBaudRate(baudrate);                              //获取波特率
    serialPort->setDataBits(databits);                              //设置数据位
    serialPort->setStopBits(stopbits);                              //设置停止位
    serialPort->setParity(parity);                                  //设置奇偶校验位

    if(serialPort->open(QIODevice::ReadWrite) == true){             //获取串口打开状态
        QMessageBox::information(this,"提示","串口打开成功");                //使用MessaBox的形式进行交互显示
    }else{
        QMessageBox::critical(this,"提示","失败");                    //使用MessaBox的形式进行交互显示
    }
}


void MainWindow::on_pushButton_close_clicked()
{
    serialPort->close();
    QMessageBox::information(this,"提示","串口已关闭");
}

//void MainWindow::serialPortReadyRead_Slot()
//{
//    QString buff;                                                   //定义数据流
//    buff = QString(serialPort->readAll());                          //读取数据
//    ui->rx_text->appendPlainText(buff);//显示串口接收数据
//}
_Bool MainWindow::Rx_Text_ReadyData_Slot()
{
    QString buff;//定义数据流
    QByteArray data;
    static QByteArray data_cache;
    static uint16_t timeout_ms = 150;
    //记录第一次接收到数据的时间
    static QTime timestart;

    //如果当前时间大于超时时间，则认为这是新的一帧数据，输出换行符，并更新接收时间
    if(QTime::currentTime()> timestart.addMSecs(timeout_ms))
    {
        //输出换行符
        ui->rx_text->appendPlainText("");
        ui->rx_text->insertPlainText(QTime::currentTime().toString("hh:mm:ss.zzz")+"\r\n");
        //更新时间
        timestart = QTime::currentTime();
    }
    data = serialPort->readAll();

    if( ui->rxform_HEX->isChecked()){
        //data = buff.toLocal8Bit();
        buff = data.toHex(' ').toUpper();
    }
    else
        buff = data;
    ui->rx_text->insertPlainText(buff);//显示串口接收数据
    //将接收数据放入缓冲区
    data_cache.append(data);

    //如果数据缓冲区长度大于数据结构体最小长度，则进行处理
    while(data_cache.size()> 21){
        //查找帧头
        if(!((data_cache[0] == 0x4a)&&(data_cache[1] == 0x54)))
            data_cache.remove(0,1);
        else
        {
            //计算数据体长度
            ushort len  = data_cache[17]*256 + data_cache[18];
            //限定数据长度，超长则认为接收帧有问题，进行删除
            if(len > 300)
                data_cache.remove(0,data_cache.size()-1);
            //判断数据是否接收完成,长度不够直接return
            if(len+21 > data_cache.size())
                return 0;
            //判断校验和
            ushort checksum_rec = (((uint16_t)data_cache[len+19]) << 8) + (uint8_t)data_cache[len+20];

            if(checksum_16(data_cache,len+19) == checksum_rec)
            {
                //判断项目类型
                switch(data_cache[13]){
                case 0x0F:
                {
                    ui->rx_parse->insertPlainText("0x0F(疏散标) \t");
                    ui->rx_parse->insertPlainText(data_cache.mid(10,2).toHex()+" ");
                    ui->rx_parse->insertPlainText(data_cache.mid(2,8).toHex()+"   ");
                    ui->Mac_Recv_EIS->setText(data_cache.mid(2,8).toHex());
                    ui->rx_parse->insertPlainText(data_cache.mid(14,1).toHex()+"   ");
                    ui->rx_parse->insertPlainText(data_cache.mid(15,2).toHex()+"   ");
                    ui->rx_parse->insertPlainText(data_cache.mid(17,2).toHex()+"   ");
                    ui->rx_parse->insertPlainText(data_cache.mid(19,len).toHex()+"   ");
                    QByteArray payload = data_cache.mid(19,len);
                    if((data_cache[15] == 0x10)&&(data_cache[16] == 0x34)){
                        ui->rx_parse->insertPlainText("states:"+QString::number((uchar)payload[0],16).toUpper());
                        for(int i =0;i<(len-1)/5;i++){
                            QString item;
                            _Bool ok;
                            switch(payload[1+i*5]){
                            case 0x04:{
                                QString value_str = payload.mid(2+i*5,4).toHex();
                                uint32_t value = value_str.toUInt(&ok,16);
                                item = " PM2.5:" + QString::number(value,10) ;
                            }
                                break;
                            case 0x05:{
                                QString value_str = payload.mid(2+i*5,4).toHex();
                                uint32_t value = value_str.toUInt(&ok,16);
                                item = " PM10:" + QString::number(value,10) ;
                            }
                            break;
                            case 0x09:{
                                QString value_str = payload.mid(2+i*5,4).toHex();
                                uint32_t value = value_str.toUInt(&ok,16);
                                item = " CO2:" + QString::number(value,10) ;
                            }
                            break;
                            case 0x0A:{
                                QString value_str = payload.mid(2+i*5,4).toHex();
                                float value = (float)value_str.toUInt(&ok,16) / 100;
                                item = " Temp:" + QString::number(value,'f',2) ;
                            }
                            break;
                            case 0x0B:{
                                QString value_str = payload.mid(2+i*5,4).toHex();
                                float value = (float)value_str.toUInt(&ok,16) / 100;
                                item = " Humi:" + QString::number(value,'f',2) ;
                            }
                            break;
                            default:
                                break;
                            }
                            ui->rx_parse->insertPlainText(item);
                        }
                    }
                    ui->rx_parse->appendPlainText("");
                }
                    break;
                case 0x0E:
                {
                    ui->rx_parse->insertPlainText("0x0E(气体) \t");
                    ui->rx_parse->insertPlainText(data_cache.mid(10,2).toHex()+" ");
                    ui->rx_parse->insertPlainText(data_cache.mid(2,8).toHex()+"   ");
                    ui->Mac_Recv_GAS->setText(data_cache.mid(2,8).toHex());
                    ui->rx_parse->insertPlainText(data_cache.mid(14,1).toHex()+"   ");
                    ui->rx_parse->insertPlainText(data_cache.mid(15,2).toHex()+"   ");
                    ui->rx_parse->insertPlainText(data_cache.mid(17,2).toHex()+"   ");
                    ui->rx_parse->insertPlainText(data_cache.mid(19,len).toHex()+"   ");
                    QByteArray payload = data_cache.mid(19,len);
                    if((data_cache[15] == 0x10)&&(data_cache[16] == 0x34)){
                        QString prop_name[] = {"none","CH4","NH3","CO","SO2","H2S","NO2","CO2","TEMP","HUMI","PM2.5","PM10"};
                        for(int i =0;i<(len)/6;i++){
                            QString item;
                            _Bool ok;
                            QString value_str = payload.mid(2+i*6,4).toHex();
                            uint value = qToBigEndian(value_str.toUInt(&ok,16));
                            item = prop_name[payload[i*6]] + " " + QString::number(*(float*)&value,'f',2) + " ";

                            ui->rx_parse->insertPlainText(item);
                        }
                    }
                    ui->rx_parse->appendPlainText("");
                }
                    break;
                default:
                    break;
                }
            }
            //从缓冲区中，删除本次解析的数据帧
            data_cache.remove(0,len+21);
        }
    }
    return 0;

}

void MainWindow::on_pushButton_send_clicked()
{
    if(txdata_form == 0)
        serialPort->write(QByteArray::fromHex(ui->tx_text->toPlainText().toLatin1()));
    else
        serialPort->write(ui->tx_text->toPlainText().toUtf8());
}

//hex字符串转ascii字符串
void MainWindow::on_txform_ASCII_clicked()
{
    /* 将Hex字符串转换为Ascii字符串 */
    QString temp;
    QByteArray text = ui->tx_text->toPlainText().toUtf8();

    temp =text.fromHex(text);
    ui->tx_text->setText(temp);	//写入文本框
    txdata_form = 1;
}

void MainWindow::on_txform_HEX_clicked()
{
    QString temp;
    QByteArray data = ui->tx_text->toPlainText().toLocal8Bit();
    temp = data.toHex(' ').toUpper();
    ui->tx_text->setText(temp);
    txdata_form = 0;
}


void MainWindow::on_pushButton_txclear_clicked()
{
    ui->tx_text->clear();
}

uint16_t checksum_16(QByteArray senddata,uint16_t len)
{
    uint16_t sum=0;//预置16位crc寄存器，初值全部为1

    int i=0;//定义计数
    for(i=0;i<len;i++)//循环计算每个数据
    {
        sum += (uint8_t)senddata[i];
    }

    return sum;
}

void MainWindow::on_pushButton_parseclear_clicked()
{
    ui->rx_parse->clear();
}


void MainWindow::on_pushButton_LightChg_clicked()
{
    QString tmp;
    QByteArray cmd;
    uint16_t checksum = 0;
    char item;
    tmp.append("4A54");
    QString mac_input = ui->Mac_Input->text().remove(QRegularExpression("\\s"));
    if(mac_input.size()!=16)
        tmp.append("FFFFFFFFFFFFFFFF");
    else
        tmp.append(mac_input);
    tmp.append("0000000F0110480001");

    cmd = QByteArray::fromHex(tmp.toUtf8());
    if(ui->Light_OFF->isChecked())
        item = 0x00;
    else if(ui->Light_LO->isChecked())
         item = 0x01;
    else if(ui->Light_RO->isChecked())
         item = 0x02;
    else
         item = 0x03;
    cmd.append(item);
   // cmd.append(QByteArray::fromHex(item));
    checksum = checksum_16(cmd,cmd.size());
    cmd.append(char(checksum>>8));
    cmd.append((char)checksum);
//    cmd.append((uint8_t)(checksum>>8));
//    cmd.append((uint8_t)checksum);

    serialPort->write(cmd);

}

void MainWindow::on_pushButton_datasave_clicked()
{
    QString mac_recv;
    QString to_write;

    static QString csvFile;

    if(csvFile.isEmpty()){
        //1.选择导出的csv文件保存路径
        csvFile = QFileDialog::getExistingDirectory(this);

        //2.文件名采用系统时间戳生成唯一的文件
        QDateTime current_date_time =QDateTime::currentDateTime();
        QString current_date =current_date_time.toString("yyyy_MM_dd");
        csvFile += tr("/EIS_Test_Log_%2.csv").arg(current_date);
    }

    //3.用QFile打开.csv文件 如果不存在则会自动新建一个新的文件
    QFile file(csvFile);
    // 如果文件不存在，则创建第1行
    if ( !file.exists())
    {
        //如果文件存在执行的操作，此处为空，因为文件不可能存在
        file.open( QIODevice::ReadWrite | QIODevice::Text );
        QTextStream out(&file);
        //创建第一行
        out<<tr("MAC,")<<tr("Light,")<<tr("CO2,")<<tr("Temp_Humi,")<<tr("PM,")<<tr("CMD,")<<tr("Result");//表头
        //关闭文件
        file.close();
    }
    //追加写入
    file.open( QIODevice::ReadWrite | QIODevice::Text);
    statusBar()->showMessage(tr("正在导出数据。。。。。。"));
    QTextStream out(&file);
    mac_recv = ui->Mac_Recv_EIS->text();
    //拼接待写入的数据
    to_write = mac_recv + tr(",")+ QString::number(ui->checkBox_Light->isChecked()) + tr(",") +  QString::number(ui->checkBox_CO2->isChecked())
               + tr(",") +  QString::number(ui->checkBox_Temp->isChecked()) + tr(",") +  QString::number(ui->checkBox_PM->isChecked()) + tr(",")
               +  QString::number(ui->checkBox_cmd->isChecked()) + tr(",");//表头

    if((ui->checkBox_Light->isChecked())&&(ui->checkBox_CO2->isChecked())&&(ui->checkBox_Temp->isChecked())&&(ui->checkBox_PM->isChecked())&&(ui->checkBox_cmd->isChecked()))
        to_write += tr("OK");
    else
        to_write += tr("FAILED");

    //先将flile全部读出到lines
    QStringList lines;
    while(!out.atEnd())
        lines.append(out.readLine());
    //查找lines中是否存在相同mac
    int index = -1;
    for(int i =0;i < lines.size();i++){
        if(lines.at(i).contains(mac_recv))
        {
            index = i;
            break;
        }
    }
    //如果存在相同mac，则询问是否需要覆盖
    if(index != -1){
        QMessageBox msgBox;
        msgBox.setText("提示");
        msgBox.setInformativeText("设备已录入，是否需要覆盖？");
        msgBox.setStandardButtons(QMessageBox::Ok | QMessageBox::Cancel);
        msgBox.setDefaultButton(QMessageBox::Cancel);
        int ret = msgBox.exec();
        //确认要覆盖，则将line进行替换
        if(ret == QMessageBox::Ok){
            //替换原有lines
            lines.replace(index,to_write);

            statusBar()->showMessage(mac_recv+tr("覆盖写入完成。。。。。。"));
        }
    }
    //没有相同的，则追加在尾部
    else{
        lines.append(to_write);
        statusBar()->showMessage(mac_recv+tr("追加写入完成。。。。。。"));
    }
    //重置文件，只保留head
    file.resize(0);
    out.seek(0);
    //重新写入文件
 //   file.write(lines);
    for(int i = 0;i<lines.size();i++)
        out<<lines.at(i)<<"\n";
    file.close();

}

void MainWindow::on_pushButton_datasave_GAS_clicked()
{
    QString mac_recv;
    QString to_write;

    static QString csvFile;

    if(csvFile.isEmpty()){
        //1.选择导出的csv文件保存路径
        csvFile = QFileDialog::getExistingDirectory(this);

        //2.文件名采用系统时间戳生成唯一的文件
        QDateTime current_date_time =QDateTime::currentDateTime();
        QString current_date =current_date_time.toString("yyyy_MM_dd");
        csvFile += tr("/GAS_Test_Log_%2.csv").arg(current_date);
    }

    //3.用QFile打开.csv文件 如果不存在则会自动新建一个新的文件
    QFile file(csvFile);
    // 如果文件不存在，则创建第1行
    if ( !file.exists())
    {
        //如果文件存在执行的操作，此处为空，因为文件不可能存在
        file.open( QIODevice::ReadWrite | QIODevice::Text );
        QTextStream out(&file);
        //创建第一行
        out<<tr("MAC,")<<tr("CH4,")<<tr("NH3,")<<tr("H2S,")<<tr("CO,")<<tr("SO2,")<<tr("NO2,")<<tr("CO2,")<<tr("PM2.5,")<<tr("Result");//表头
        //关闭文件
        file.close();
    }
    //追加写入
    file.open( QIODevice::ReadWrite | QIODevice::Text);
    statusBar()->showMessage(tr("正在导出数据。。。。。。"));
    QTextStream out(&file);
    mac_recv = ui->Mac_Recv_GAS->text();
    //拼接待写入的数据
    to_write = mac_recv + tr(",")+ QString::number(ui->checkBox_CH4_GAS->isChecked()) + tr(",") +  QString::number(ui->checkBox_NH3_GAS->isChecked())
               + tr(",") +  QString::number(ui->checkBox_CO_GAS->isChecked()) + tr(",") +  QString::number(ui->checkBox_H2S_GAS->isChecked()) + tr(",")
               +  QString::number(ui->checkBox_SO2_GAS->isChecked()) + tr(",")+  QString::number(ui->checkBox_NO2_GAS->isChecked()) + tr(",")
               +  QString::number(ui->checkBox_CO2_GAS->isChecked()) + tr(",")+  QString::number(ui->checkBox_PM_GAS->isChecked()) + tr(",")
               +  QString::number(ui->checkBox_Temp_Humi_GAS->isChecked()) + tr(",")+QString::number(ui->checkBox_Motor_GAS->isChecked()) + tr(",");//表头

    if(ui->checkBox_Result_GAS->isChecked())
        to_write += tr("OK");
    else
        to_write += tr("FAILED");

    //先将flile全部读出到lines
    QStringList lines;
    while(!out.atEnd())
        lines.append(out.readLine());
    //查找lines中是否存在相同mac
    int index = -1;
    for(int i =0;i < lines.size();i++){
        if(lines.at(i).contains(mac_recv))
        {
            index = i;
            break;
        }
    }
    //如果存在相同mac，则询问是否需要覆盖
    if(index != -1){
        QMessageBox msgBox;
        msgBox.setText("提示");
        msgBox.setInformativeText("设备已录入，是否需要覆盖？");
        msgBox.setStandardButtons(QMessageBox::Ok | QMessageBox::Cancel);
        msgBox.setDefaultButton(QMessageBox::Cancel);
        int ret = msgBox.exec();
        //确认要覆盖，则将line进行替换
        if(ret == QMessageBox::Ok){
            //替换原有lines
            lines.replace(index,to_write);

            statusBar()->showMessage(mac_recv+tr("覆盖写入完成。。。。。。"));
        }
    }
    //没有相同的，则追加在尾部
    else{
        lines.append(to_write);
        statusBar()->showMessage(mac_recv+tr("追加写入完成。。。。。。"));
    }
    //重置文件，只保留head
    file.resize(0);
    out.seek(0);
    //重新写入文件
    //   file.write(lines);
    for(int i = 0;i<lines.size();i++)
        out<<lines.at(i)<<"\n";
    file.close();
}

void MainWindow::on_pushButton_typeset_Gas_clicked()
{
    QString tmp;
    QByteArray cmd;
    uint16_t checksum = 0;
    char item,reserve;
    tmp.append("4A54");
    QString mac_input = ui->Mac_Input_GAS->text().remove(QRegularExpression("\\s"));
    if(mac_input.size()!=16)
        tmp.append("FFFFFFFFFFFFFFFF");
    else
        tmp.append(mac_input);
    tmp.append("0000000E0110380004");

    cmd = QByteArray::fromHex(tmp.toUtf8());

    item = ui->box_sensor_u6->currentIndex();
    //如果串口6接的是PM2.5传感器，则需要将PM10传感器ID写入指令数据体byte0
    if(item == 0x0A)
        reserve = 0x0B;
    //否则，将指令数据体byte0 设为0x00
    else
        reserve = 0x00;
    cmd.append(reserve);
    cmd.append(item);
    item = ui->box_sensor_u5->currentIndex();
    cmd.append(item);
    item = ui->box_sensor_u4->currentIndex();
    cmd.append(item);
    checksum = checksum_16(cmd,cmd.size());
    cmd.append(char(checksum>>8));
    cmd.append((char)checksum);

    serialPort->write(cmd);
}


void MainWindow::on_pushButton_motor_clicked()
{
    QString tmp;
    QByteArray cmd;
    uint16_t checksum = 0;
    char item,reserve;
    tmp.append("4A54");
    QString mac_input = ui->Mac_Input_GAS->text().remove(QRegularExpression("\\s"));
    if(mac_input.size()!=16)
        tmp.append("FFFFFFFFFFFFFFFF");
    else
        tmp.append(mac_input);
    tmp.append("0000000E0110390001");

    cmd = QByteArray::fromHex(tmp.toUtf8());

    item = ui->rotation_turns->value();
    cmd.append(item);
    checksum = checksum_16(cmd,cmd.size());
    cmd.append(char(checksum>>8));
    cmd.append((char)checksum);

    serialPort->write(cmd);
}

