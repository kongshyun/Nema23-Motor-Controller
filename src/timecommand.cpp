#include "timecommand.h"

QString TimeCommand::buildCommand(int rpm, int duration) const
{
    return QString("RPM:%1 TIME:%2").arg(rpm).arg(duration);
}

bool TimeCommand::isValidInput(int rpm, int duration) const
{
    return (rpm > 0 && duration > 0);
}

MotorMode TimeCommand::getMode() const
{
    return MotorMode::TIME;
}

QString TimeCommand::getValueLabel() const
{
    return "시간(초)";
}