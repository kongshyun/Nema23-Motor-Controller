# 🔬 기술 개요서 (Technical Overview)

## 🏗️ 아키텍처 설계

### SOLID 원칙 적용

#### 1. Single Responsibility Principle (단일 책임 원칙)
```cpp
MainWindow     → UI 컨트롤 및 사용자 상호작용
SerialHandler  → UART 통신 전담
MotorControl   → 모터 상태 관리 및 로직
IMotorCommand  → 명령 생성 전략 정의
```

#### 2. Open/Closed Principle (개방-폐쇄 원칙)
- 새로운 모터 모드 추가 시 기존 코드 수정 없이 확장 가능
- `IMotorCommand` 인터페이스를 구현하여 새 전략 추가

#### 3. Strategy Pattern 구현
```cpp
interface IMotorCommand {
    QString buildCommand(int rpm, int value);
    bool isValidInput(int rpm, int value);
    MotorMode getMode();
}

class RotationCommand : IMotorCommand {
    // "RPM:X ROT:Y" 형식 생성
}

class TimeCommand : IMotorCommand {
    // "RPM:X TIME:Y" 형식 생성
}
```

#### 4. Factory Pattern 적용
```cpp
class MotorCommandFactory {
    static unique_ptr<IMotorCommand> createCommand(MotorMode mode) {
        switch(mode) {
            case ROTATION: return make_unique<RotationCommand>();
            case TIME: return make_unique<TimeCommand>();
        }
    }
}
```

## 📡 통신 프로토콜

### UART 설정
```
Baud Rate: 115200
Data Bits: 8
Stop Bits: 1
Parity: None
Flow Control: None
```

### 프로토콜 명세
```
┌─────────────────┬────────────────┬──────────────────┐
│ 명령 타입       │ PC → ESP32     │ ESP32 → PC       │
├─────────────────┼────────────────┼──────────────────┤
│ 연결 확인       │ "HELLO"        │ "READY"          │
│ 회전수 제어     │ "RPM:60 ROT:5" │ "TURN:1"..."DONE"│
│ 시간 제어       │ "RPM:80 TIME:10"│ "TURN:1"..."DONE"│
│ 비상 정지       │ "STOP"         │ "STOPPED"        │
└─────────────────┴────────────────┴──────────────────┘
```

### 상태 머신
```
[DISCONNECTED] --HELLO--> [CONNECTING] --READY--> [CONNECTED]
     ↑                                                 ↓
     ↑                                              GO 명령
     ↑                                                 ↓
[ERROR] <--timeout-- [RUNNING] <--TURN:X-- [RUNNING]
                         ↓
                    DONE/STOPPED
                         ↓
                   [CONNECTED]
```

## 🖥️ UI 상태 관리

### 상태 기반 UI 제어
```cpp
enum UIState {
    IDLE,       // 모든 컨트롤 활성화, STOP 비활성화
    RUNNING,    // 입력 컨트롤 비활성화, STOP만 활성화
    CONNECTING  // 모든 컨트롤 비활성화
};

void setUIEnabled(bool enabled) {
    ui->speedSlider->setEnabled(enabled);
    ui->rotationSpinBox->setEnabled(enabled);
    ui->rotationModeRadio->setEnabled(enabled);
    ui->timeModeRadio->setEnabled(enabled);
    ui->goButton->setEnabled(enabled);
    ui->stopButton->setEnabled(!enabled && isMotorRunning);
}
```

### 동적 UI 업데이트
```cpp
void updateUIForMode(MotorMode mode) {
    if (mode == ROTATION) {
        ui->labelRotation->setText("회전 수:");
        ui->rotationSpinBox->setSuffix(" 회전");
        ui->rotationSpinBox->setMaximum(9999);
    } else if (mode == TIME) {
        ui->labelRotation->setText("구동 시간:");
        ui->rotationSpinBox->setSuffix(" 초");
        ui->rotationSpinBox->setMaximum(3600);
    }
}
```

## ⚡ 성능 및 메모리 관리

### 메모리 사용량
```
기본 실행 시:    ~15MB (Qt 프레임워크 포함)
UI 렌더링:       ~2-3MB
시리얼 버퍼:     ~1KB (512 bytes * 2)
로그 버퍼:       동적 할당 (최대 ~1MB)
```

### 스레드 구조
```
Main Thread (UI)
├── QTimer (DateTime update, 1초 간격)
├── QSerialPort (비동기 I/O)
└── QMessageBox (모달 대화상자)
```

### 최적화 기법
- **지연 로딩**: UI 요소 필요 시에만 생성
- **버퍼링**: 시리얼 데이터 패킷 단위 처리
- **상태 캐싱**: UI 상태 변경 최소화

## 🔒 안전성 및 신뢰성

### 입력 검증
```cpp
bool isValidInput(int rpm, int value) {
    if (rpm < 0 || rpm > 100) return false;
    if (mode == ROTATION && (value < 1 || value > 9999)) return false;
    if (mode == TIME && (value < 1 || value > 3600)) return false;
    return true;
}
```

### 오류 처리
```
1. 시리얼 포트 오류
   → 자동 재연결 시도
   → 사용자 알림

2. 통신 타임아웃
   → 5초 후 연결 해제
   → 오류 상태 표시

3. 잘못된 응답
   → 로그에 경고 메시지
   → 상태 복구 시도
```

### 사용자 안전
- **확인 대화상자**: 위험한 작업 전 사용자 확인
- **UI 잠금**: 부적절한 시점의 조작 방지
- **시각적 피드백**: 현재 상태 명확히 표시

## 📊 확장성 및 유지보수성

### 새로운 모터 모드 추가
```cpp
// 1. 새로운 enum 값 추가
enum class MotorMode {
    ROTATION, TIME, ACCELERATION  // ← 새 모드
};

// 2. 새로운 Command 클래스 구현
class AccelerationCommand : public IMotorCommand {
    QString buildCommand(int rpm, int accel) override {
        return QString("RPM:%1 ACCEL:%2").arg(rpm).arg(accel);
    }
};

// 3. Factory에 등록
case ACCELERATION: 
    return make_unique<AccelerationCommand>();
```

### 코드 품질 지표
```
라인 수:        ~800 lines (주석 포함)
클래스 수:      8개 (UI 제외)
결합도:         낮음 (인터페이스 기반)
응집도:         높음 (단일 책임)
테스트 커버리지: ~70% (핵심 로직)
```

### 향후 개선 방향
```
단기:
- [ ] 설정 파일 저장/로드
- [ ] 로그 파일 내보내기
- [ ] 시뮬레이션 모드

중기:
- [ ] 웹 기반 원격 제어
- [ ] 다중 모터 지원
- [ ] 스크립트 기반 자동화

장기:
- [ ] AI 기반 최적화
- [ ] 클라우드 모니터링
- [ ] 산업용 안전 표준 준수
```

## 🛠️ 개발 도구 및 환경

### 필수 도구
```
Qt Creator 6.x+     → IDE 및 UI 디자이너
Qt 5.15+ / 6.x      → 프레임워크
MinGW / MSVC        → Windows 컴파일러
GCC                 → Linux 컴파일러
```

### 빌드 구성
```cmake
# stepperESP32.pro
QT += core gui serialport widgets
CONFIG += c++17
INCLUDEPATH += $$PWD/inc
SOURCES += $$files($$PWD/src/*.cpp)
HEADERS += $$files($$PWD/inc/*.h)
```

### 디버깅 지원
```cpp
#ifdef QT_DEBUG
    qDebug() << "수신된 메시지:" << data;
    qDebug() << "현재 모드:" << static_cast<int>(currentMode);
#endif
```

---

**개발 기간**: 2주  
**코드 리뷰**: 2회  
**테스트 시간**: 10시간+  
**문서화 비율**: 25%