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
    
    // 시간 콤보박스 초기화
    initializeTimeComboBoxes();
    
    // 초기 모터 상태 설정
    updateMotorStatus("대기 중", "gray");

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
    int value;
    QString settingText;

    if (currentMode == MotorMode::ROTATION) {
        value = ui->rotationSpinBox->value();
        settingText = QString("RPM: %1, ROT: %2").arg(speed).arg(value);
    } else if (currentMode == MotorMode::TIME) {
        int hours = ui->hoursComboBox->currentText().toInt();
        int minutes = ui->minutesComboBox->currentText().toInt();
        int seconds = ui->secondsComboBox->currentText().toInt();
        value = getTotalSeconds();
        settingText = QString("RPM: %1, TIME: %2초 (%3:%4:%5)").arg(speed).arg(value).arg(hours).arg(minutes, 2, 10, QChar('0')).arg(seconds, 2, 10, QChar('0'));
    }
    
    ui->settingLineEdit->setPlainText(settingText);
    isSettingConfirmed = false;
    ui->textEditInputLog->appendPlainText("설정값 : " + settingText);
}


void MainWindow::on_setButton_clicked()
{
    confirmedSpeed = ui->speedSlider->value();
    
    if (currentMode == MotorMode::ROTATION) {
        confirmedValue = ui->rotationSpinBox->value();
        ui->textEditInputLog->appendPlainText(QString("회전수 모드: RPM=%1, 회전수=%2").arg(confirmedSpeed).arg(confirmedValue));
    } else if (currentMode == MotorMode::TIME) {
        int hours = ui->hoursComboBox->currentText().toInt();
        int minutes = ui->minutesComboBox->currentText().toInt();
        int seconds = ui->secondsComboBox->currentText().toInt();
        confirmedValue = getTotalSeconds();
        ui->textEditInputLog->appendPlainText(QString("시간 모드: %1시 %2분 %3초 = 총 %4초").arg(hours).arg(minutes).arg(seconds).arg(confirmedValue));
    }
    
    isSettingConfirmed = true;

    ui->settingLineEdit->setStyleSheet("font-weight: bold;");
    ui->textEditInputLog->appendPlainText("설정 값이 확정되었습니다.");
}


void MainWindow::log(const QString &message)
{
    ui->textEditConnect->setPlainText(message);
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
    updateMotorStatus("구동 중", "#FF4500");  // 밝은 주황색 (OrangeRed)

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
        log(" 모터 제어기와 연결되었습니다.");
        serialHandler->sendCommand("HI");
        qDebug() << "전송메세지 : HI";
        ui->portComboBox->setEnabled(false);
        ui->statusLabel->setStyleSheet("QLabel { background-color: green; border:none;}");
        updateMotorStatus("연결됨", "blue");
    }

    ui->rotationProgressBar->setValue(motorControl.getProgress());
    ui->textEditInputLog->appendPlainText(motorControl.getStatusMessage());
    
    // 모터 완료 또는 정지 시 UI 재활성화
    if (trimmed == "DONE") {
        isMotorRunning = false;
        setUIEnabled(true);
        updateMotorStatus("완료", "blue");
    } else if (trimmed == "STOPPED") {
        isMotorRunning = false;
        setUIEnabled(true);
        updateMotorStatus("정지됨", "#FFA500");  // 주황색
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
        ui->rotationSpinBox->setVisible(true);
        
        // 시간 콤보박스들 숨기기
        ui->hoursComboBox->setVisible(false);
        ui->minutesComboBox->setVisible(false);
        ui->secondsComboBox->setVisible(false);
        ui->timeLabel1->setVisible(false);
        ui->timeLabel2->setVisible(false);
        ui->timeUnitLabel->setVisible(false);
    } else if (mode == MotorMode::TIME) {
        ui->labelRotation->setText("구동 시간:");
        ui->rotationSpinBox->setVisible(false);
        
        // 시간 콤보박스들 보이기
        ui->hoursComboBox->setVisible(true);
        ui->minutesComboBox->setVisible(true);
        ui->secondsComboBox->setVisible(true);
        ui->timeLabel1->setVisible(true);
        ui->timeLabel2->setVisible(true);
        ui->timeUnitLabel->setVisible(true);
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
    
    // 시간 콤보박스들도 포함
    ui->hoursComboBox->setEnabled(enabled);
    ui->minutesComboBox->setEnabled(enabled);
    ui->secondsComboBox->setEnabled(enabled);
    
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
        updateMotorStatus("정지 중", "#FFA500");  // 주황색
    }
}

void MainWindow::initializeTimeComboBoxes()
{
    // 시간 콤보박스 (0-23)
    ui->hoursComboBox->clear();
    for (int i = 0; i < 24; i++) {
        ui->hoursComboBox->addItem(QString::number(i));
    }
    ui->hoursComboBox->setCurrentIndex(0);
    
    // 분 콤보박스 (0-59)
    ui->minutesComboBox->clear();
    for (int i = 0; i < 60; i++) {
        ui->minutesComboBox->addItem(QString("%1").arg(i, 2, 10, QChar('0')));
    }
    ui->minutesComboBox->setCurrentIndex(0);
    
    // 초 콤보박스 (0-59)
    ui->secondsComboBox->clear();
    for (int i = 0; i < 60; i++) {
        ui->secondsComboBox->addItem(QString("%1").arg(i, 2, 10, QChar('0')));
    }
    ui->secondsComboBox->setCurrentIndex(10); // 기본값 10초
}

int MainWindow::getTotalSeconds() const
{
    int hours = ui->hoursComboBox->currentText().toInt();
    int minutes = ui->minutesComboBox->currentText().toInt();
    int seconds = ui->secondsComboBox->currentText().toInt();
    
    return hours * 3600 + minutes * 60 + seconds;
}

void MainWindow::updateMotorStatus(const QString &status, const QString &color)
{
    if (status == "구동 중") {
        // 구동 중일 때는 깜빡이는 효과와 더 큰 테두리
        ui->motorStatusLED->setStyleSheet(QString("QLabel { background-color: %1; border: 2px solid #FF0000; border-radius: 8px; animation: blink 1s infinite;}").arg(color));
    } else {
        // 일반적인 스타일
        ui->motorStatusLED->setStyleSheet(QString("QLabel { background-color: %1; border: 1px solid #666; border-radius: 8px;}").arg(color));
    }
    ui->motorStatusText->setText(status);
}

