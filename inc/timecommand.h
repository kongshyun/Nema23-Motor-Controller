#ifndef TIMECOMMAND_H
#define TIMECOMMAND_H

#include "imotorcommand.h"

class TimeCommand : public IMotorCommand
{
public:
    QString buildCommand(int rpm, int duration) const override;
    bool isValidInput(int rpm, int duration) const override;
    MotorMode getMode() const override;
    QString getValueLabel() const override;
};

#endif // TIMECOMMAND_H