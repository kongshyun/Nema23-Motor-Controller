#ifndef IMOTORCOMMAND_H
#define IMOTORCOMMAND_H

#include <QString>

enum class MotorMode {
    ROTATION,
    TIME
};

class IMotorCommand
{
public:
    virtual ~IMotorCommand() = default;
    virtual QString buildCommand(int rpm, int value) const = 0;
    virtual bool isValidInput(int rpm, int value) const = 0;
    virtual MotorMode getMode() const = 0;
    virtual QString getValueLabel() const = 0;
};

#endif // IMOTORCOMMAND_H