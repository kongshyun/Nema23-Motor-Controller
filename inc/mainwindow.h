#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QTimer>
#include <QDateTime>
#include <QSerialPortInfo>
#include <QString>
#include <QMessageBox>
#include "serialhandler.h"
#include "motorcontrol.h"
#include "motorcommandfactory.h"
#include "imotorcommand.h"

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
    ~MainWindow();

private slots:
    void updateDateTime();
    void on_getButton_clicked();
    void on_setButton_clicked();
    void on_goButton_clicked();
    void on_portComboBox_currentIndexChanged(const QString &portName);
    void on_connectButton_clicked();
    void on_rotationModeRadio_toggled(bool checked);
    void on_timeModeRadio_toggled(bool checked);
    void on_stopButton_clicked();

    void handleSerialResponse(const QString &data);
private:
    Ui::MainWindow *ui;
    QTimer *timer;
    SerialHandler *serialHandler;
    QString selectedPortName;


    //내부 상태 관리용 변수
    bool isSettingConfirmed;
    int confirmedSpeed;
    int confirmedValue;
    MotorMode currentMode;
    bool isMotorRunning;

    MotorControl motorControl;

    void populateSerialPorts();
    void log(const QString &message);
    void updateUIForMode(MotorMode mode);
    void setUIEnabled(bool enabled);



};
#endif // MAINWINDOW_H
