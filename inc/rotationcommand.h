#ifndef ROTATIONCOMMAND_H
#define ROTATIONCOMMAND_H

#include "imotorcommand.h"

class RotationCommand : public IMotorCommand
{
public:
    QString buildCommand(int rpm, int rotations) const override;
    bool isValidInput(int rpm, int rotations) const override;
    MotorMode getMode() const override;
    QString getValueLabel() const override;
};

#endif // ROTATIONCOMMAND_H