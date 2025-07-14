#ifndef MOTORCONTROL_H
#define MOTORCONTROL_H

#include <QString>
#include <QDebug>
#include <memory>
#include "imotorcommand.h"

class MotorControl
{
public:
    MotorControl();
    
    void setCommandStrategy(std::unique_ptr<IMotorCommand> command);
    QString buildCommand(int rpm, int value) const;
    bool isValidInput(int rpm, int value) const;

    void setTarget(int rpm, int value);
    bool processResponse(const QString &message); // true == 연결 성공(READY)

    int getProgress() const;
    QString getStatusMessage() const;
    void reset();
    MotorMode getCurrentMode() const;

private:
    std::unique_ptr<IMotorCommand> commandStrategy;
    int targetValue = 0;
    int currentProgress = 0;
    int rpm = 0;
    QString status = "대기 중";
    bool isReady = false;
};

#endif // MOTORCONTROL_H
