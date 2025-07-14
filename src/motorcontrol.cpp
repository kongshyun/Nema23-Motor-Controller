#include "motorcontrol.h"
#include "rotationcommand.h"

MotorControl::MotorControl()
    : commandStrategy(std::make_unique<RotationCommand>())
{
}

void MotorControl::setCommandStrategy(std::unique_ptr<IMotorCommand> command)
{
    commandStrategy = std::move(command);
}

QString MotorControl::buildCommand(int rpm, int value) const
{
    if (!commandStrategy) {
        return QString();
    }
    return commandStrategy->buildCommand(rpm, value);
}

bool MotorControl::isValidInput(int rpm, int value) const
{
    if (!commandStrategy) {
        return false;
    }
    return commandStrategy->isValidInput(rpm, value);
}

void MotorControl::setTarget(int r, int value)
{
    rpm = r;
    targetValue = value;
    currentProgress = 0;
    status = "대기 중";
    isReady = false;
}

bool MotorControl::processResponse(const QString &message)
{
    if (message == "READY") {
        qDebug()<<"수신 : READY";
        isReady = true;
        status = "모터 연결됨";
        return true;
    }

    if (message.startsWith("TURN:")) {
        currentProgress = message.section(":", 1, 1).toInt();
        status = QString("진행 중: %1 / %2").arg(currentProgress).arg(targetValue);
    }
    else if (message.contains("DONE")) {
        status = "✔ 완료됨";
    }
    else if (message.contains("STOPPED")) {
        status = "⛔ 정지됨";
    }

    return false;
}

int MotorControl::getProgress() const
{
    if (targetValue == 0) return 0;
    return static_cast<int>((static_cast<float>(currentProgress) / targetValue) * 100);
}

QString MotorControl::getStatusMessage() const
{
    return status;
}

void MotorControl::reset()
{
    currentProgress = 0;
    status = "대기 중";
    isReady = false;
}

MotorMode MotorControl::getCurrentMode() const
{
    if (!commandStrategy) {
        return MotorMode::ROTATION;
    }
    return commandStrategy->getMode();
}
