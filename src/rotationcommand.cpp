#include "rotationcommand.h"

QString RotationCommand::buildCommand(int rpm, int rotations) const
{
    return QString("RPM:%1 ROT:%2").arg(rpm).arg(rotations);
}

bool RotationCommand::isValidInput(int rpm, int rotations) const
{
    return (rpm > 0 && rotations > 0);
}

MotorMode RotationCommand::getMode() const
{
    return MotorMode::ROTATION;
}

QString RotationCommand::getValueLabel() const
{
    return "회전수";
}