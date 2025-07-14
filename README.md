# 🎛️ Nema23 Stepper Motor Controller

![Qt](https://img.shields.io/badge/Qt-5%2F6-green.svg)
![C++](https://img.shields.io/badge/C%2B%2B-17-blue.svg)
![License](https://img.shields.io/badge/license-MIT-blue.svg)
![Platform](https://img.shields.io/badge/platform-Windows%20%7C%20Linux-lightgrey.svg)

ESP32를 통해 Nema23 스테퍼 모터를 정밀 제어하는 Qt 기반 GUI 애플리케이션입니다.

## 📸 스크린샷

> 🚧 **스크린샷 추가 예정**  
> 애플리케이션 실행 화면과 주요 기능들을 보여주는 이미지를 준비 중입니다.

## ✨ 주요 특징

- 🎯 **정밀 제어**: 회전수 단위의 정확한 모터 제어
- ⏱️ **시간 기반**: 지정된 시간 동안 연속 회전
- 🛡️ **안전 기능**: 구동 중 설정 변경 방지 및 비상 정지
- 📊 **실시간 모니터링**: 진행률과 상태를 실시간으로 확인
- 🏗️ **확장 가능**: SOLID 원칙 기반의 모듈러 아키텍처

## 🎯 주요 기능

### 모터 제어 모드
- **회전수 모드**: RPM과 회전수를 설정하여 정확한 회전 제어
- **시간 모드**: RPM과 구동 시간을 설정하여 시간 기반 제어

### 실시간 모니터링
- 진행률 표시바로 모터 상태 확인
- 실시간 로그로 모든 명령과 응답 추적
- 연결 상태 LED 표시

### 안전 기능
- 모터 구동 중 설정 변경 방지 (UI 자동 비활성화)
- STOP 버튼 확인 대화상자로 안전한 정지
- 입력값 유효성 검사

## 🖥️ 사용자 인터페이스

### 연결 설정
- 시리얼 포트 자동 감지 및 선택
- ESP32와 UART 통신 (115200 baud)
- 연결 상태 실시간 표시

### 모터 제어 패널
```
┌─ 모드 선택 ─────────────────────┐
│ ○ 회전수    ○ 시간              │
├─ 회전 속도 ────────────────────┤
│ [슬라이더: 0-100 RPM]           │
├─ 설정값 ──────────────────────┤
│ 회전수: [____] 회전             │
│ 또는                           │
│ 시간: [____] 초                 │
├─ 제어 버튼 ──────────────────┤
│ [GET] [SET] [GO]   [STOP]      │
└────────────────────────────────┘
```

### 상태 모니터링
- **진행률 바**: 현재 작업 진행 상황
- **로그 창**: 명령 전송/수신 기록
- **설정값 표시**: 현재 확인된 설정

## 🔧 통신 프로토콜

### ESP32로 전송되는 명령
```
연결: HELLO
회전수 모드: RPM:60 ROT:10
시간 모드: RPM:60 TIME:30
정지: STOP
```

### ESP32에서 수신되는 응답
```
연결 확인: READY
진행 상황: TURN:5
완료: DONE
정지: STOPPED
```

## 🏗️ 소프트웨어 아키텍처

### SOLID 원칙 적용
- **Strategy 패턴**: 회전수/시간 모드별 명령 생성
- **Factory 패턴**: 모드에 따른 적절한 Command 객체 생성
- **Single Responsibility**: 각 클래스는 단일 책임

### 핵심 컴포넌트
```cpp
MainWindow          // UI 컨트롤러
├── SerialHandler   // UART 통신 관리
├── MotorControl    // 모터 상태 및 로직
└── Command 전략들   // 명령 생성 (Strategy 패턴)
    ├── RotationCommand
    └── TimeCommand
```

## 📁 프로젝트 구조

```
stepperESP32/
├── inc/                    # 헤더 파일
│   ├── mainwindow.h
│   ├── serialhandler.h
│   ├── motorcontrol.h
│   ├── imotorcommand.h     # Strategy 인터페이스
│   ├── rotationcommand.h
│   ├── timecommand.h
│   └── motorcommandfactory.h
├── src/                    # 구현 파일
│   ├── mainwindow.cpp
│   ├── serialhandler.cpp
│   ├── motorcontrol.cpp
│   ├── rotationcommand.cpp
│   ├── timecommand.cpp
│   └── motorcommandfactory.cpp
├── images/                 # UI 리소스
├── mainwindow.ui          # Qt Designer UI 파일
├── stepperESP32.pro       # qmake 프로젝트 파일
├── test_architecture.cpp  # SOLID 아키텍처 검증
└── CLAUDE.md              # 개발 가이드
```

## 🛠️ 빌드 방법

```bash
# qmake로 Makefile 생성
qmake stepperESP32.pro

# 컴파일
make

# Windows (MinGW)
mingw32-make
```

## 🚀 빠른 시작

### 1️⃣ 하드웨어 준비
```
ESP32 + Nema23 스테퍼 모터 + 드라이버
USB 케이블로 PC와 ESP32 연결
```

### 2️⃣ 소프트웨어 빌드
```bash
git clone https://github.com/kongshyun/Nema23-Motor-Controller.git
cd Nema23-Motor-Controller/stepperESP32
qmake stepperESP32.pro
make  # 또는 mingw32-make (Windows)
```

### 3️⃣ 사용 방법
1. **연결**: ESP32 포트 선택 → Connect
2. **설정**: 모드 선택 → RPM/값 입력 → SET
3. **실행**: GO 버튼 클릭
4. **정지**: STOP 버튼 (확인 후)

## 🔒 안전 기능

### UI 상태 관리
- **모터 정지 시**: 모든 설정 변경 가능, STOP 버튼 비활성화
- **모터 구동 시**: 설정 변경 불가, STOP 버튼만 활성화

### 입력 검증
- RPM 범위: 0-100
- 회전수: 최대 9999회전
- 시간: 최대 3600초 (1시간)

## 📊 개발 현황

### ✅ 완료된 기능
- [x] 기본 GUI 인터페이스
- [x] 시리얼 통신 (UART)
- [x] 회전수/시간 모드 선택
- [x] 실시간 진행률 표시
- [x] 모터 구동 중 UI 잠금
- [x] STOP 버튼 확인 대화상자
- [x] SOLID 아키텍처 적용
- [x] 입력값 유효성 검사

### 🔄 개선 가능한 부분
- [ ] 설정값 저장/로드 기능
- [ ] 다국어 지원 (현재 한국어)
- [ ] 로그 파일 저장
- [ ] 고급 모터 제어 옵션

## 🔧 개발 환경

- **언어**: C++17
- **프레임워크**: Qt 5/6
- **빌드 시스템**: qmake
- **통신**: QSerialPort (115200 baud)
- **IDE**: Qt Creator (권장)

## 🤝 기여하기

1. Fork the Project
2. Create your Feature Branch (`git checkout -b feature/AmazingFeature`)
3. Commit your Changes (`git commit -m 'Add some AmazingFeature'`)
4. Push to the Branch (`git push origin feature/AmazingFeature`)
5. Open a Pull Request

## 📄 라이선스

이 프로젝트는 MIT 라이선스 하에 배포됩니다. 자세한 내용은 `LICENSE` 파일을 참조하세요.

## 📞 문의

**개발자**: FREAKERSPACE  
**프로젝트 링크**: [https://github.com/kongshyun/Nema23-Motor-Controller](https://github.com/kongshyun/Nema23-Motor-Controller)

## 🙏 감사의 말

- Qt 프레임워크 팀
- ESP32 커뮤니티
- 오픈소스 기여자들

---

⭐ 이 프로젝트가 도움이 되었다면 스타를 눌러주세요!