#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QTimer>
#include <QComboBox>

QT_BEGIN_NAMESPACE
namespace Ui {
class MainWindow;
}
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

    ////////////////////////////////////////////

    void readKartLock();
    void readKartGate();
    void loop();
    void sendSerialLock(int number);
    void COMs();

private slots:

    void on_pb_disCOM_clicked();

    void on_pb_conCOM_clicked();

    ////////////////////

    void on_pb_disCOM_2_clicked();

    void on_pb_conCOM_2_clicked();

    ////////////////////////////////////////////

    void on_pb_start_clicked();

    void on_pb_lock1_clicked();

    void on_pb_lock2_clicked();

    void on_pb_lock3_clicked();

    void on_pb_lock4_clicked();

    void on_pb_manualLock_clicked();

    ////////////////////////////////////////////

    void on_pb_openGate_clicked();

    void on_pb_redGate_clicked();

    ////////////////////////////////////////////

    void on_cb_offline_clicked(bool checked);

    void on_cb_remote_clicked();

    ////////////////////////////////////////////

    void on_pb_OK1_clicked();

    void on_pb_OK2_clicked();

private:
    Ui::MainWindow *ui;
};
#endif // MAINWINDOW_H
