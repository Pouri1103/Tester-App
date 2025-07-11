#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QSerialPort>
#include <QSerialPortInfo>
#include <QDebug>
#include <QString>
#include <QMessageBox>
#include <QTimer>
#include <QComboBox>

//-------------------------

QSerialPort sLock;
QSerialPort sGate;

QByteArray NumID = "";
QByteArray NumID2 = "";

//-------------------------

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    ui->cb_COM->setFocus();
    statusBar()->showMessage("Designed And Developed By POURIYA");

    COMs();
}

MainWindow::~MainWindow()
{
    delete ui;
}

//#################################################################################################################################################
//-----Settings-----//
//-------------------------------------------------------

void MainWindow::COMs()
{
    ui->cb_COM->clear();
    ui->cb_COM_2->clear();

    for (QSerialPortInfo port: QSerialPortInfo::availablePorts())
    {
        QString items = port.portName();

        ui->cb_COM->addItem(items);
        ui->cb_COM_2->addItem(items);
    }

}

//-------------------------------------------------------
    //----Lock----

void MainWindow::on_pb_disCOM_clicked()
{
    if (sLock.isOpen())
        sLock.close();
    ui->cb_COM->setEnabled(1);
    COMs();
}

void MainWindow::on_pb_conCOM_clicked()
{
    if (sLock.isOpen())
        sLock.close();
    sLock.setPortName(ui->cb_COM->currentText());
    sLock.setBaudRate(QSerialPort::Baud9600);
    sLock.setDataBits(QSerialPort::Data8);
    sLock.setParity(QSerialPort::NoParity);
    sLock.setStopBits(QSerialPort::OneStop);
    sLock.setFlowControl(QSerialPort::NoFlowControl);

    if (sLock.open(QIODevice::ReadWrite))
    {
        ui->cb_COM->setDisabled(1);
        connect(&sLock, &QSerialPort::readyRead, this, &MainWindow::readKartLock);
    }
    else
        QMessageBox::critical(this,"عدم اتصال به پورت","   اتصال انجام نشد !!! \n " + sLock.errorString());
}

//-------------------------------------------------------
//----Gate----

void MainWindow::on_pb_disCOM_2_clicked()
{
    if (sGate.isOpen())
        sGate.close();
    ui->cb_COM_2->setEnabled(1);
    COMs();
}

void MainWindow::on_pb_conCOM_2_clicked()
{
    if (sGate.isOpen())
        sGate.close();
    sGate.setPortName(ui->cb_COM_2->currentText());
    sGate.setBaudRate(QSerialPort::Baud9600);
    sGate.setDataBits(QSerialPort::Data8);
    sGate.setParity(QSerialPort::NoParity);
    sGate.setStopBits(QSerialPort::OneStop);
    sGate.setFlowControl(QSerialPort::NoFlowControl);

    if (sGate.open(QIODevice::ReadWrite))
    {
        ui->cb_COM_2->setDisabled(1);
        connect(&sGate, &QSerialPort::readyRead, this, &MainWindow::readKartGate);
    }
    else
        QMessageBox::critical(this,"عدم اتصال به پورت","   اتصال انجام نشد !!! \n " + sGate.errorString());
}

//#################################################################################################################################################
//-----read kart in Locker & Gate-----//
//-------------------------------------------------------

void MainWindow::readKartLock()
{
    QByteArray read = sLock.readAll();
    NumID.append(read);

    int i=0;
    while(i< NumID.length() && NumID.at(i) != '%'){
        i++;
    }
    NumID.remove(0,i);

    if (NumID.length() >= 12)
    {
        ui->le_lock_M->setText(NumID);

        if (NumID.at(0) == '%' && NumID.at(11) == '*')
            ui->pb_OK1->setText("OK");
        else
            ui->pb_OK1->setText("Error");

        NumID.clear();
    }
}

void MainWindow::readKartGate()
{
    QByteArray read = sGate.readAll();

    NumID2.append(read);

    if (NumID2.length() >= 12)
    {
        ui->le_gate_M->setText(NumID2);
        ui->pb_OK2->setText("OK");

        NumID2.clear();
    }
}

void MainWindow::on_pb_OK1_clicked()
{
    ui->le_lock_M->clear();
}


void MainWindow::on_pb_OK2_clicked()
{
    ui->le_gate_M->clear();
}

//#################################################################################################################################################
//-----write in Locker-----//
//-------------------------------------------------------

int of;

void MainWindow::on_pb_start_clicked()
{
    if (sLock.isOpen())
    {
        of = ui->sb_of->value();
        ui->pb_start->setDisabled(1);
        loop();
    } else
        QMessageBox::critical(this,"مشکل در اتصال با ریدر کمد","  ارتباط با ریدر کمد برقرار نشد !!! \n" + sLock.errorString());
}


void MainWindow::loop()
{
    int to = ui->sb_to->value();
    int jump = ui->sb_jump->value();
    int time = ui->sb_time->value();

    int plus1 = ui->sb_plus_1->value();
    int plus2 = ui->sb_plus_2->value();
    int plus3 = ui->sb_plus_3->value();
    int plus4 = ui->sb_plus_4->value();

    if (of <= to)
    {
        sendSerialLock(of);
        of += jump;
        QTimer::singleShot(time,this,&MainWindow::loop);
    }
    else {
        static int i = 1;
        switch (i) {
        case 1:
            if (plus1 != 0)
            {
                sendSerialLock(plus1);
                QTimer::singleShot(time,this,&MainWindow::loop);
            } else
                QTimer::singleShot(0,this,&MainWindow::loop);
            break;
        case 2:
            if (plus2 != 0)
            {
                sendSerialLock(plus2);
                QTimer::singleShot(time,this,&MainWindow::loop);
            } else
                QTimer::singleShot(0,this,&MainWindow::loop);
            break;
        case 3:
            if (plus3 != 0)
            {
                sendSerialLock(plus3);
                QTimer::singleShot(time,this,&MainWindow::loop);
            } else
                QTimer::singleShot(0,this,&MainWindow::loop);
            break;
        case 4:
            if (plus4 != 0)
                sendSerialLock(plus4);
            break;
        default :
            i=1;
            ui->pb_start->setEnabled(1);
            break;
        }
        i++;

        if (ui->cb_repeat->isChecked() && ui->sb_repeat->value() > 0 && i == 5)
        {
            ui->sb_repeat->setValue(ui->sb_repeat->value() - 1);
            of = ui->sb_of->value();
            QTimer::singleShot(time,this,&MainWindow::loop);
            i=1;
        }
        else if (i == 5)
        {
            i=1;
            ui->pb_start->setEnabled(1);
            ui->cb_repeat->setChecked(0);
        }
    }
}

////////////////////////////////////////////////////////////////

void MainWindow::sendSerialLock(int number)
{
    QByteArray command;
    command.resize(4);
    if (ui->cb_remote->isChecked())
    {
        command[0] = '(';
        command[1] = (char) number / 256;
        command[2] = (char) number % 256;
        command[3] = ')';
    }
    else if (ui->cb_offline->isChecked()) {
        command.resize(5);
        command[0] = '@';
        command[1] = 'A';
        command[2] = (char) number / 256;
        command[3] = (char) number % 256;
        command[4] = '#';
    }
    else {
        command[0] = '$';
        command[1] = (char) number / 256;
        command[2] = (char) number % 256;
        command[3] = '&';
    }

    if(sLock.isOpen())
        sLock.write(command);
    else
        QMessageBox::critical(this,"مشکل در اتصال با ریدر کمد","  ارتباط با ریدر کمد برقرار نشد !!! \n" + sLock.errorString());
}

////////////////////////////////////////////////////////////////

void MainWindow::on_pb_lock1_clicked()
{
    sendSerialLock(ui->sb_lock_1->value());
}

void MainWindow::on_pb_lock2_clicked()
{
    sendSerialLock(ui->sb_lock_2->value());
}

void MainWindow::on_pb_lock3_clicked()
{
    sendSerialLock(ui->sb_lock_3->value());
}

void MainWindow::on_pb_lock4_clicked()
{
    sendSerialLock(ui->sb_lock_4->value());
}
////////////////////////////////////////////////////////////////

void MainWindow::on_pb_manualLock_clicked()
{
    sendSerialLock(ui->spinBox->value());
}

//#################################################################################################################################################
//-----open & red in Gate-----//
//-------------------------------------------------------

void MainWindow::on_pb_openGate_clicked()
{
    QByteArray command;
    if (ui->cb_offline->isChecked())
    {
        int number = ui->spinBox->value();
        command.resize(5);
        command[0] = '@';
        command[1] = 'B';
        if (number >= 256)
        {
            command[2] = 0x01;
            command[3] = (char) number-256;
        }
        else {
            command[2] = 0x00;
            command[3] = (char) number;
        }
        command[4] = '#';

        if(sLock.isOpen())
            sLock.write(command);
        else
            QMessageBox::critical(this,"مشکل در اتصال با ریدر کمد","  ارتباط با ریدر کمد برقرار نشد !!! \n" + sLock.errorString());
    }
    else {
        command.append("$(");
        command.append(0xFC);
        command.append(0x8A);
        command.append("000");
        command.append(")$");

        if(sGate.isOpen())
            sGate.write(command);
        else
            QMessageBox::critical(this,"مشکل در اتصال با گیت","  ارتباط با گیت برقرار نشد !!! \n" + sGate.errorString());
    }

}


void MainWindow::on_pb_redGate_clicked()
{
    QByteArray command;
    if (ui->cb_offline->isChecked())
    {
        command.resize(5);
        command[0] = '@';
        command[1] = 'C';
        command[2] = 0x00;
        command[3] = 0x00;
        command[4] = '#';

        if(sLock.isOpen())
            sLock.write(command);
        else
            QMessageBox::critical(this,"مشکل در اتصال با ریدر کمد","  ارتباط با ریدر کمد برقرار نشد !!! \n" + sLock.errorString());
    }
    else {
        command.append("$(");
        command.append(0xFC);
        command.append(0xA8);
        command.append("000");
        command.append(")$");

        if(sGate.isOpen())
            sGate.write(command);
        else
            QMessageBox::critical(this,"مشکل در اتصال با گیت","  ارتباط با گیت برقرار نشد !!! \n" + sGate.errorString());
    }


}

//#################################################################################################################################################
//-----offline reader-----//
//-------------------------------------------------------

void MainWindow::on_cb_remote_clicked()
{
    ui->cb_offline->setChecked(0);
    ui->pb_openGate->setText("باز کردن");
    ui->pb_redGate->setText("قرمز");
    ui->groupBox_9->setTitle("گیت");
    ui->le_lock_M->setText("");
}
//-------------------------------------------------------

void MainWindow::on_cb_offline_clicked(bool checked)
{
    ui->cb_remote->setChecked(0);

    if (checked == 1)
    {
        ui->pb_openGate->setText("F2");
        ui->pb_redGate->setText("F3");
        ui->groupBox_9->setTitle("ریدر آفلاین");
        ui->le_lock_M->setText("ثبت در حافظه");
    }
    else {
        ui->pb_openGate->setText("باز کردن");
        ui->pb_redGate->setText("قرمز");
        ui->groupBox_9->setTitle("گیت");
        ui->le_lock_M->setText("");
    }
}



