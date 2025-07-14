#include "mainwindow.h"
#include "ui_mainwindow.h"

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
    , timer(new QTimer(this))
    , serialHandler(new SerialHandler(this))
    , isSettingConfirmed(false)
    , currentMode(MotorMode::ROTATION)
    , isMotorRunning(false)
{
    ui->setupUi(this);

    connect(timer, &QTimer::timeout, this, &MainWindow::updateDateTime);
    timer->start(1000); //1초마다 실행
    updateDateTime();// 초기 날짜/시간 표시
    log("연결 대기");
    connect(ui->portComboBox,
            &QComboBox::currentTextChanged,
            this,
            &MainWindow::on_portComboBox_currentIndexChanged);


    connect(serialHandler, &SerialHandler::dataReceived,
            this, &MainWindow::handleSerialResponse);


    populateSerialPorts();

    connect(ui->speedSlider,&QSlider::sliderMoved,this,[=](int value){
        int step=ui->speedSlider->singleStep();
        int snapped=qRound((float)value/step)*step;
        ui->speedSlider->setValue(snapped);
    });

    // Initialize mode selection with radio buttons
    motorControl.setCommandStrategy(MotorCommandFactory::createCommand(currentMode));
    updateUIForMode(currentMode);
    
    // 초기 UI 상태 설정 (모터 정지 상태)
    setUIEnabled(true);

}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::populateSerialPorts()
{
    ui->portComboBox->clear();
    ui->portComboBox->addItem("Select Port");
    const auto ports = QSerialPortInfo::availablePorts();
    for (const QSerialPortInfo &port : ports) {
        ui->portComboBox->addItem(port.portName());
    }
}


// ---------- 슬롯 구현 ----------

void MainWindow::on_getButton_clicked()
{
    int speed = ui->speedSlider->value();
    int value = ui->rotationSpinBox->value();

    MotorMode currentMode = motorControl.getCurrentMode();
    QString modeText = (currentMode == MotorMode::ROTATION) ? "ROT" : "TIME";
    QString settingText = QString("RPM: %1, %2: %3").arg(speed).arg(modeText).arg(value);
    ui->settingLineEdit->setPlainText(settingText);

    isSettingConfirmed=false;
    ui->textEditInputLog->appendPlainText("설정값 : "+settingText);
}


void MainWindow::on_setButton_clicked()
{
    confirmedSpeed = ui->speedSlider->value();
    confirmedValue = ui->rotationSpinBox->value();
    isSettingConfirmed = true;

    ui->settingLineEdit->setStyleSheet("font-weight: bold;");
    ui->textEditInputLog->appendPlainText("설정 값이 확정되었습니다.");
}


void MainWindow::log(const QString &message)
{
    ui->textEditConnect->appendPlainText(message);
}
void MainWindow::on_goButton_clicked()
{
    if (!isSettingConfirmed) {
        ui->textEditInputLog->appendPlainText(" SET 버튼을 누르세요");
        return;
    }

    if (!motorControl.isValidInput(confirmedSpeed, confirmedValue)) {
        ui->textEditInputLog->appendPlainText("❌ 유효하지 않은 설정값입니다");
        return;
    }

    QString command = motorControl.buildCommand(confirmedSpeed, confirmedValue);
    motorControl.setTarget(confirmedSpeed, confirmedValue);
    serialHandler->sendCommand(command);
    ui->textEditInputLog->appendPlainText("📤 명령 전송됨: " + command);
    
    // 모터 구동 시작 - UI 비활성화
    isMotorRunning = true;
    setUIEnabled(false);

    isSettingConfirmed = false;
    ui->settingLineEdit->setStyleSheet("font-weight: normal;");
}

void MainWindow::updateDateTime()
{
    QDateTime current = QDateTime::currentDateTime();

    // 요일 한글 변환
    QMap<int, QString> weekdays;
    weekdays[1] = "MON";
    weekdays[2] = "TUE";
    weekdays[3] = "WED";
    weekdays[4] = "THU";
    weekdays[5] = "FRI";
    weekdays[6] = "SAT";
    weekdays[7] = "SUN";

    QString weekday = weekdays[current.date().dayOfWeek()];

    // 오전/오후 판단
    QString ampm = current.time().hour() < 12 ? "AM" : "PM";

    // 12시간제로 변환
    int hour = current.time().hour() % 12;
    if (hour == 0) hour = 12;

    // 날짜/시간 문자열 생성
    QString dateTimeStr = QString::asprintf("%04d.%02d.%02d %s %s %02d:%02d:%02d",
                                            current.date().year(),
                                            current.date().month(),
                                            current.date().day(),
                                            qPrintable(weekday),
                                            qPrintable(ampm),
                                            hour,
                                            current.time().minute(),
                                            current.time().second());

    ui->dateTimeLabel->setText(dateTimeStr);
}

void MainWindow::on_portComboBox_currentIndexChanged(const QString &portName)
{
    selectedPortName = portName;
    log("포트 선택됨: " + portName);
    ui->textEditConnect->moveCursor(QTextCursor::End);
}

void MainWindow::on_connectButton_clicked()
{
    if(selectedPortName.isEmpty() || selectedPortName =="Serial Port"){
        log("✅ 포트를 선택하세요.");
        return;
    }
    if(serialHandler->openSerialPort(selectedPortName)){
        log("포트를 열었습니다. 모터 연결 확인 중...");
        serialHandler->sendCommand("HELLO\n");
        qDebug()<<"전송메세지 : HELLO ";
    }else{
        log("❌ 포트 열기 실패: " + selectedPortName);
    }
    ui->textEditConnect->moveCursor(QTextCursor::End);

}



void MainWindow::handleSerialResponse(const QString &data)
{
    QString trimmed = data.trimmed();
    qDebug() << "수신된 메시지:" << trimmed;

    if (motorControl.processResponse(data)) {
        log("✅ 모터와 연결되었습니다.");
        ui->portComboBox->setEnabled(false);
        ui->statusLabel->setStyleSheet("QLabel { background-color: green; border:none;}");
    }

    ui->rotationProgressBar->setValue(motorControl.getProgress());
    ui->textEditInputLog->appendPlainText(motorControl.getStatusMessage());
    
    // 모터 완료 또는 정지 시 UI 재활성화
    if (trimmed == "DONE" || trimmed == "STOPPED") {
        isMotorRunning = false;
        setUIEnabled(true);
    }
}


void MainWindow::on_rotationModeRadio_toggled(bool checked)
{
    if (checked) {
        currentMode = MotorMode::ROTATION;
        motorControl.setCommandStrategy(MotorCommandFactory::createCommand(currentMode));
        updateUIForMode(currentMode);
    }
}

void MainWindow::on_timeModeRadio_toggled(bool checked)
{
    if (checked) {
        currentMode = MotorMode::TIME;
        motorControl.setCommandStrategy(MotorCommandFactory::createCommand(currentMode));
        updateUIForMode(currentMode);
    }
}

void MainWindow::updateUIForMode(MotorMode mode)
{
    if (mode == MotorMode::ROTATION) {
        ui->labelRotation->setText("회전 수:");
        ui->rotationSpinBox->setSuffix(" 회전");
        ui->rotationSpinBox->setMaximum(9999);
        ui->rotationSpinBox->setValue(1);
    } else if (mode == MotorMode::TIME) {
        ui->labelRotation->setText("구동 시간:");
        ui->rotationSpinBox->setSuffix(" 초");
        ui->rotationSpinBox->setMaximum(3600); // 최대 1시간
        ui->rotationSpinBox->setValue(10);
    }
}

void MainWindow::setUIEnabled(bool enabled)
{
    // 모터 구동 중에는 설정 변경 불가
    ui->speedSlider->setEnabled(enabled);
    ui->rotationSpinBox->setEnabled(enabled);
    ui->rotationModeRadio->setEnabled(enabled);
    ui->timeModeRadio->setEnabled(enabled);
    ui->goButton->setEnabled(enabled);
    ui->setButton->setEnabled(enabled);
    ui->getButton->setEnabled(enabled);
    
    // STOP 버튼은 모터 구동 중에만 활성화
    ui->stopButton->setEnabled(!enabled && isMotorRunning);
}

void MainWindow::on_stopButton_clicked()
{
    // 확인 대화상자 표시
    QMessageBox::StandardButton reply = QMessageBox::question(
        this,
        "모터 정지 확인",
        "정말 모터를 멈추시겠습니까?",
        QMessageBox::Ok | QMessageBox::Cancel,
        QMessageBox::Cancel
    );
    
    if (reply == QMessageBox::Ok) {
        // 정지 신호 전송
        serialHandler->sendCommand("STOP");
        ui->textEditInputLog->appendPlainText("🛑 정지 신호 전송됨");
        
        // UI 상태 즉시 변경 (ESP32 응답 전에)
        isMotorRunning = false;
        setUIEnabled(true);
    }
}

