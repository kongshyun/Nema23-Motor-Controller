#include "motorcommandfactory.h"
#include "rotationcommand.h"
#include "timecommand.h"

std::unique_ptr<IMotorCommand> MotorCommandFactory::createCommand(MotorMode mode)
{
    switch (mode) {
    case MotorMode::ROTATION:
        return std::make_unique<RotationCommand>();
    case MotorMode::TIME:
        return std::make_unique<TimeCommand>();
    default:
        return std::make_unique<RotationCommand>();
    }
}