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
    // if (ui->le_lock4->text() == "")
        // NumID.clear();
    QByteArray read = sLock.readAll();

    NumID.push_back(read);

    if (NumID.length() > 10)   // if (NumID.length() == 12)
    {
        ui->le_lock_M->setText(NumID);

        if (NumID.at(0) == '%' && NumID.at(11) == '*')
            ui->le_OK1->setText("OK");
        else
            ui->le_OK1->setText("Error");

        NumID.clear();
    }
}

void MainWindow::readKartGate()
{
    // if (ui->le_lock4->text() == "")
        // NumID2.clear();
    QByteArray read = sGate.readAll();

    NumID2.push_back(read);

    if (NumID2.length() > 10) // ==12
    {
        ui->le_gate_M->setText(NumID2);
        NumID2.clear();
        ui->le_OK2->setText("OK?");
    }
}

//#################################################################################################################################################
//-----write in Locker-----//
//-------------------------------------------------------

static int of;

void MainWindow::on_pb_start_clicked()
{
    if (sLock.isOpen())
    {
        of = ui->le_of->text().toInt();
        ui->pb_start->setDisabled(1);
        loop();
    } else
        QMessageBox::critical(this,"مشکل در اتصال با ریدر کمد","  ارتباط با ریدر کمد برقرار نشد !!! \n" + sLock.errorString());
}


void MainWindow::loop()
{
    int to = ui->le_to->text().toInt();
    int jump = ui->le_jump->text().toInt();
    int time = ui->le_time->text().toInt();

    int plus1 = ui->le_plus1->text().toInt();
    int plus2 = ui->le_plus2->text().toInt();
    int plus3 = ui->le_plus3->text().toInt();
    int plus4 = ui->le_plus4->text().toInt();

    if (of <= to)
    {
        sendSerialLock(of);
        of += jump;
        QTimer::singleShot(time,this,&loop);
    }
    else {
        static int i = 1;
        switch (i) {
        case 1:
            if (plus1 != 0)
            {
                sendSerialLock(plus1);
                QTimer::singleShot(time,this,&loop);
            } else
                QTimer::singleShot(0,this,&loop);
            break;
        case 2:
            if (plus2 != 0)
            {
                sendSerialLock(plus2);
                QTimer::singleShot(time,this,&loop);
            } else
                QTimer::singleShot(0,this,&loop);
            break;
        case 3:
            if (plus3 != 0)
            {
                sendSerialLock(plus3);
                QTimer::singleShot(time,this,&loop);
            } else
                QTimer::singleShot(0,this,&loop);
            break;
        case 4:
            if (plus4 != 0)
            {
                sendSerialLock(plus4);
                QTimer::singleShot(time,this,&loop);
            } else
                QTimer::singleShot(0,this,&loop);
            break;
        default :
            i=0;
            ui->pb_start->setEnabled(1);
            break;
        }
        i++;
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
        if (number >= 256)
        {
            command[1] = 0x01;
            command[2] = (char) number-256;
        }
        else {
            command[1] = 0x00;
            command[2] = (char) number;
        }
        command[3] = ')';
    }
    else if (ui->cb_offline->isChecked()) {
        command.resize(5);
        command[0] = '@';
        command[1] = 'A';
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
    }
    else {
        command[0] = '$';
        if (number >= 256)
        {
            command[1] = 0x01;
            command[2] = (char) number-256;
        }
        else {
            command[1] = 0x00;
            command[2] = (char) number;
        }
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
    sendSerialLock(ui->le_lock1->text().toInt());
}

void MainWindow::on_pb_lock2_clicked()
{
    sendSerialLock(ui->le_lock2->text().toInt());
}

void MainWindow::on_pb_lock3_clicked()
{
    sendSerialLock(ui->le_lock3->text().toInt());
}

void MainWindow::on_pb_lock4_clicked()
{
    sendSerialLock(ui->le_lock4->text().toInt());
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
    }
    else {
        ui->pb_openGate->setText("باز کردن");
        ui->pb_redGate->setText("قرمز");
        ui->groupBox_9->setTitle("گیت");

    }
}



