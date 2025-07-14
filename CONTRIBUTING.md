# 기여 가이드 (Contributing Guide)

Nema23 Stepper Motor Controller 프로젝트에 기여해주셔서 감사합니다! 🎉

## 🚀 기여 방법

### 1. 이슈 보고
버그를 발견하거나 새로운 기능을 제안하고 싶다면:

1. [Issues](https://github.com/shyun/Nema23-Motor-Controller/issues) 페이지에서 유사한 이슈가 있는지 확인
2. 새로운 이슈 생성 시 적절한 템플릿 사용
3. 명확하고 상세한 설명 제공

### 2. 코드 기여

#### 개발 환경 설정
```bash
# 저장소 Fork 후 Clone
git clone https://github.com/shyun/Nema23-Motor-Controller.git
cd Nema23-Motor-Controller/stepperESP32

# 개발용 브랜치 생성
git checkout -b feature/new-feature-name
```

#### 코딩 표준
- **언어**: C++17 표준 준수
- **네이밍**: camelCase (변수/함수), PascalCase (클래스)
- **들여쓰기**: 4칸 공백
- **주석**: 한글 또는 영문 (일관성 유지)

```cpp
// Good
class MotorController {
private:
    int motorSpeed;          // 모터 속도
    bool isRunning;         // 실행 상태
    
public:
    void setSpeed(int speed);
    bool getRunningState();
};

// Bad
class motor_controller {
    int MotorSpeed;
    void SetSpeed(int Speed);
}
```

#### SOLID 원칙 준수
이 프로젝트는 SOLID 원칙을 따릅니다:
- **S**: 단일 책임 원칙
- **O**: 개방-폐쇄 원칙
- **L**: 리스코프 치환 원칙
- **I**: 인터페이스 분리 원칙
- **D**: 의존성 역전 원칙

새로운 기능 추가 시 기존 아키텍처를 참고해주세요.

### 3. Pull Request 가이드

#### PR 체크리스트
- [ ] 새로운 기능/버그 수정이 완료됨
- [ ] 코드가 컴파일되고 정상 작동함
- [ ] 기존 테스트가 통과함
- [ ] 필요시 새로운 테스트 추가
- [ ] 문서 업데이트 완료
- [ ] 커밋 메시지가 명확함

#### PR 템플릿
```markdown
## 변경 사항 요약
간략한 변경 내용 설명

## 변경 이유
왜 이 변경이 필요한지 설명

## 테스트 방법
1. 테스트 단계 1
2. 테스트 단계 2
3. 예상 결과

## 스크린샷 (UI 변경 시)
변경 전/후 비교 이미지

## 체크리스트
- [ ] 코드 컴파일 확인
- [ ] 기능 테스트 완료
- [ ] 문서 업데이트
```

## 🐛 버그 리포트 템플릿

```markdown
**버그 설명**
명확하고 간결한 버그 설명

**재현 단계**
1. '...' 클릭
2. '...' 스크롤
3. 오류 발생

**예상 동작**
무엇이 일어날 것으로 예상했는지

**실제 동작**
실제로 무엇이 일어났는지

**환경 정보**
- OS: [e.g. Windows 10, Ubuntu 20.04]
- Qt Version: [e.g. 6.2.0]
- Compiler: [e.g. MinGW, GCC]

**추가 정보**
스크린샷, 로그 등
```

## 💡 기능 요청 템플릿

```markdown
**기능 설명**
새로운 기능에 대한 명확한 설명

**문제점**
현재 어떤 문제가 있는지

**제안하는 해결책**
어떻게 해결하고 싶은지

**대안 고려사항**
다른 가능한 해결책들

**추가 컨텍스트**
스크린샷, 예시 등
```

## 📚 개발 가이드

### 새로운 모터 모드 추가 예시
```cpp
// 1. IMotorCommand 인터페이스 구현
class AccelerationCommand : public IMotorCommand {
public:
    QString buildCommand(int rpm, int accel) override {
        return QString("RPM:%1 ACCEL:%2").arg(rpm).arg(accel);
    }
    
    bool isValidInput(int rpm, int accel) const override {
        return rpm > 0 && rpm <= 100 && accel > 0 && accel <= 50;
    }
    
    MotorMode getMode() const override {
        return MotorMode::ACCELERATION;
    }
};

// 2. Factory에 등록
case MotorMode::ACCELERATION:
    return std::make_unique<AccelerationCommand>();

// 3. UI 업데이트 로직 추가
if (mode == MotorMode::ACCELERATION) {
    ui->labelRotation->setText("가속도:");
    ui->rotationSpinBox->setSuffix(" rpm/s");
}
```

### 테스트 가이드
```bash
# 아키텍처 테스트 실행
g++ -std=c++17 test_architecture.cpp -I./inc -o test_arch
./test_arch

# 메모리 누수 확인 (Linux)
valgrind --leak-check=full ./stepperESP32
```

## 🔄 릴리스 프로세스

1. **버전 업데이트**: `stepperESP32.pro`에서 VERSION 수정
2. **CHANGELOG 업데이트**: 새로운 기능과 버그 수정 사항 기록
3. **태그 생성**: `git tag v1.x.x`
4. **릴리스 노트 작성**: GitHub Releases에서 상세 내용 작성

## 🤝 커뮤니티 가이드라인

- 서로를 존중하고 건설적인 피드백 제공
- 이슈나 PR에서 명확하고 정중한 의사소통
- 다양한 의견과 경험을 환영
- 초보자도 환영하며 도움 제공

## 🏆 기여자 인정

모든 기여자는 README.md의 기여자 섹션에 추가됩니다.

---

궁금한 점이 있으시면 언제든 이슈를 통해 문의해주세요! 🚀